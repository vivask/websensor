#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "storage.h"
#include "i2cdev.h"
#include "bmp280.h"
#include "ds18x20.h"
#include "aht.h"
#include "test.h"

#define I2C_SCL_IO              CONFIG_WEB_I2C_SCL_IO               
#define I2C_SDA_IO              CONFIG_WEB_I2C_SDA_IO               
#define DS18B20_GPIO            CONFIG_WEB_DS18B20_GPIO

#define DELAY_TIME_BETWEEN_ITEMS_S 1.5

static const char *TAG = "peripheral";

static bmp280_t bmp280_dev;
static ds18x20_addr_t ds18x20_addrs;
static char DS18B20_ADDRESS[32] = {0};
static aht_t aht_dev;

static bool ds18b20_initialized = false;
static bool ds18b20_active = false;
static bool bmp280_initialized = false;
static bool bmp280_active = false;
static bool aht_initialized = false;
static bool aht_active = false;

static TaskHandle_t xTaskPool = NULL;

static time_t tBeginPool = 0;
static time_t tEndPool = 0;

time_t validate_begin_time(time_t begin);
time_t validate_end_time(time_t end);

#ifdef CONFIG_WEB_TEST_MODE
    void test_settings_set(time_t begin, time_t end);
    void test_settings_get(time_t* begin, time_t* end);
#endif

void peripheral_initialize(){
    esp_err_t ret;
    
    //initialize DS18B20
    size_t ds18x20_sensor_count = 0;
    gpio_set_pull_mode(DS18B20_GPIO, GPIO_PULLUP_ONLY);
    ret = ds18x20_scan_devices(DS18B20_GPIO, &ds18x20_addrs, 1, &ds18x20_sensor_count);
    ret = (ret != ESP_OK || ds18x20_addrs == 0) ? ESP_FAIL : ret;

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "%s: DS18B20 initialisation fail", esp_err_to_name(ret));
    }else{
        sprintf(DS18B20_ADDRESS, "%08X%08X", (uint32_t)(ds18x20_addrs >> 32), (uint32_t)ds18x20_addrs);
        ESP_LOGI(TAG, "DS18B20 : %s initialisation success", DS18B20_ADDRESS);
        ds18b20_initialized = true;
    }

    //initialize BMX280
    ret = i2cdev_init();
    bmp280_params_t bmp20_params;
    bmp280_init_default_params(&bmp20_params);
    memset(&bmp280_dev, 0, sizeof(bmp280_t));
    ret = (bmp280_init_desc(&bmp280_dev, BMP280_I2C_ADDRESS_0, 0, I2C_SDA_IO, I2C_SCL_IO) == ESP_OK &&
           bmp280_init(&bmp280_dev, &bmp20_params) == ESP_OK) ? ESP_OK : ESP_FAIL;
    bool bme280p = bmp280_dev.id == BME280_CHIP_ID;
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "%s: %s initialisation fail", esp_err_to_name(ret), bme280p ? "BME280" : "BMP280");
    }else{
        ESP_LOGI(TAG, "%s initialisation success", bme280p ? "BME280" : "BMP280");
        bmp280_initialized = true;
    }

    //ininitialize AHT
    ret = aht_init_desc(&aht_dev, AHT_I2C_ADDRESS_GND, 0, I2C_SDA_IO, I2C_SCL_IO);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "AHT: %s initialisation fail", esp_err_to_name(ret));
    }else{
        ESP_LOGI(TAG, "AHT initialisation success");
        aht_initialized = true;
    }

#ifdef CONFIG_WEB_TEST_MODE
    time_t begin, end;
    remove_all_sensor_data();
    test_generate_data_aht(750, &begin, &end);
    test_generate_data_bmx280(750, &begin, &end);
    test_generate_data_ds18b20(750, &begin, &end);
    test_settings_set(begin, end);
#else
    remove_all_sensor_data();
#endif

}

static void pool_ds18b20() {
    if(!ds18b20_initialized || tBeginPool == 0 || tEndPool == 0){
        ds18b20_active = false;
        return;
    }

    time_t now = time(0);
    localtime(&now);
    if( now < tBeginPool || now > tEndPool ){
        ds18b20_active = false;
        return;
    }

    ds18b20_data_t data;
    esp_err_t err = ds18x20_measure_and_read_multi(DS18B20_GPIO, &ds18x20_addrs, 1, &data.temperature);
    if(err != ESP_OK){
        ESP_LOGE(TAG, "DS18B20: %s: No ack, sensor not connected...skip...", esp_err_to_name(err));
        ds18b20_active = false;
    }else{
        ESP_LOGI(TAG, "Temperature: %.2f C", data.temperature);
        data.date_time = now;
        insert_ds18b20(&data);
        ds18b20_active = true;
    }
}

static void pool_bmp280() {
    if(!bmp280_initialized || tBeginPool == 0 || tEndPool == 0){
        bmp280_active = false;
        return;
    }

    time_t now = time(0);
    localtime(&now);
    if( now < tBeginPool || now > tEndPool ){
        bmp280_active = false;
        return;
    }

    bmx280_data_t data;
    esp_err_t err = bmp280_read_float(&bmp280_dev, &data.temperature, &data.pressure, &data.humidity);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "%s: %s: No ack, bme280 not connected...skip...",
        (bmp280_dev.id == BME280_CHIP_ID) ? "BME280" : "BMP280", esp_err_to_name(err));
        bmp280_active = false;
    }else {
        ESP_LOGI(TAG, "Pressure: %.2f Pa, Temperature: %.2f C, Humidity: %.2f ", data.pressure, data.temperature, data.humidity);
        data.date_time = now;
        insert_bmx280(&data);
        bmp280_active = true;
    }
}

static void pool_aht(){
    if(!aht_initialized || tBeginPool == 0 || tEndPool == 0){
        aht_active = false;
        return;
    }

    time_t now = time(0);
    localtime(&now);
    if( now < tBeginPool || now > tEndPool ){
        aht_active = false;
        return;
    }

    aht_data_t data;
    esp_err_t err = aht_get_data(&aht_dev, &data.temperature, &data.humidity);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "AHT: No ack, not connected, error: %s", esp_err_to_name(err));
        aht_active = false;
    }else {
        ESP_LOGI(TAG, "Temperature: %.2f C, Humidity: %.2f ", data.temperature, data.humidity);
        data.date_time = now;
        insert_aht(&data);
        aht_active = true;
    }
}

static void task_pool(void* arg){

    while(1){
        pool_bmp280();
        vTaskDelay(1000*DELAY_TIME_BETWEEN_ITEMS_S / portTICK_RATE_MS);
        pool_ds18b20();
        vTaskDelay(1000*DELAY_TIME_BETWEEN_ITEMS_S / portTICK_RATE_MS);
        pool_aht();
        vTaskDelay(1000*DELAY_TIME_BETWEEN_ITEMS_S / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void peripheral_start(time_t begin, time_t end){
    tBeginPool = validate_begin_time(begin);
    tEndPool = validate_end_time(end);   
#ifndef CONFIG_WEB_TEST_MODE    
    remove_all_sensor_data();
#endif
    xTaskCreatePinnedToCore(task_pool, "task_pool", configMINIMAL_STACK_SIZE*8, &xTaskPool, 10, NULL, 0);
}

void peripheral_stop(){
    if(xTaskPool){
        vTaskDelete( xTaskPool );
        xTaskPool = NULL;
        tBeginPool = 0;
        tEndPool = 0;
    }
}

bool ds18b20_available() {
    return ds18b20_initialized;
}

bool bmx280_available() {
    return bmp280_initialized;
}

bool aht_available() {
    return aht_initialized;
}

bool is_activity() {
    return ds18b20_active || bmp280_active || aht_active;
}