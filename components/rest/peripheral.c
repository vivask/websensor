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

#define I2C_SCL_IO              GPIO_NUM_22               
#define I2C_SDA_IO              GPIO_NUM_23               
#define DS18B20_GPIO            GPIO_NUM_17

#define DELAY_TIME_BETWEEN_ITEMS_S 2.5

static const char *TAG = "peripheral";

static bmp280_t bmp280_dev;
static ds18x20_addr_t ds18x20_addrs;
static char DS18B20_ADDRESS[32] = {0};

static bool ds18b20_initialized = false;
static bool bmp280_initialized = false;

static TaskHandle_t xTaskPool = NULL;

static time_t tBeginPool = 0;
static time_t tEndPool = 0;

void peripheral_initialize(){
    esp_err_t ret;
    
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
}

static void pool_ds18b20() {
    if(!ds18b20_initialized || tBeginPool == 0 || tEndPool == 0){
        return;
    }

    time_t now = time(0);
    localtime(&now);
    if( now < tBeginPool || now > tEndPool ){
        return;
    }

    ds18b20_data_t data;
    esp_err_t err = ds18x20_measure_and_read_multi(DS18B20_GPIO, &ds18x20_addrs, 1, &data.temperature);
    if(err != ESP_OK){
        ESP_LOGE(TAG, "DS18B20: %s: No ack, sensor not connected...skip...", esp_err_to_name(err));
    }else{
        ESP_LOGI(TAG, "Temperature: %.2f C", data.temperature);
        data.date_time = now;
        insert_ds18b20(&data);
    }
}

static void pool_bmp280() {
    if(!bmp280_initialized || tBeginPool == 0 || tEndPool == 0){
        return;
    }

    time_t now = time(0);
    localtime(&now);
    if( now < tBeginPool || now > tEndPool ){
        return;
    }

    bmx280_data_t data;
    esp_err_t err = bmp280_read_float(&bmp280_dev, &data.temperature, &data.pressure, &data.humidity);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "%s: %s: No ack, bme280 not connected...skip...",
        (bmp280_dev.id == BME280_CHIP_ID) ? "BME280" : "BMP280", esp_err_to_name(err));
    }else {
        ESP_LOGI(TAG, "Pressure: %.2f Pa, Temperature: %.2f C, Humidity: %.2f ", data.pressure, data.temperature, data.humidity);
        data.date_time = now;
        insert_bmx280(&data);
    }
}

static void task_pool(void* arg){

    while(1){
        pool_bmp280();
        vTaskDelay(1000*DELAY_TIME_BETWEEN_ITEMS_S / portTICK_RATE_MS);
        pool_ds18b20();
        vTaskDelay(1000*DELAY_TIME_BETWEEN_ITEMS_S / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void peripheral_start(time_t begin, time_t end){
    tBeginPool = begin;
    tEndPool = end;
    remove_all_data();
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