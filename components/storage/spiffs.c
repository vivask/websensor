#include <sys/stat.h>
#include <sys/unistd.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "spiffs.h"

static const char *TAG = "SPIFFS";

#if CONFIG_WEB_DEPLOY_SF

#define DS18B20_FILE_NAME CONFIG_WEB_BASE_PATH "/ds18b20.bin"
#define BMX280_FILE_NAME CONFIG_WEB_BASE_PATH "/bmx280.bin"

static esp_vfs_spiffs_conf_t spiffs_conf = {
    .base_path = CONFIG_WEB_BASE_PATH,
    .partition_label = NULL,
    .max_files = CONFIG_MAX_FILES,
    .format_if_mount_failed = true
};

static void get_date_time(char* date_time, time_t time){
    struct tm* t = localtime(&time); 
    sprintf(date_time, "20%02d-%02d-%02d %02d:%02d:%02d", t->tm_year - 100, t->tm_mon + 1,
           t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    ESP_LOGI(TAG, "TIME: %s", date_time);
}

static time_t next_hour(const time_t t){
    struct tm* tm = localtime(&t);
    tm->tm_hour++;
    return mktime(tm);
}

static void ds18b20_data_to_json(cJSON* items, const ds18b20_data_t* data){
        char time[20];
        cJSON *item = cJSON_CreateObject();
        get_date_time(time, data->date_time);
        cJSON_AddStringToObject(item, "date_time", time);
        cJSON_AddNumberToObject(item, "temperature", data->temperature);
        cJSON_AddItemToArray(items, item);
}

static void bmx280_data_to_json(cJSON* items, const bmx280_data_t* data){
        char time[20];
        cJSON *item = cJSON_CreateObject();
        get_date_time(time, data->date_time);
        cJSON_AddStringToObject(item, "date_time", time);
        cJSON_AddNumberToObject(item, "temperature", data->temperature);
        cJSON_AddNumberToObject(item, "pressure", data->pressure);
        cJSON_AddNumberToObject(item, "humidity", data->humidity);
        cJSON_AddItemToArray(items, item);
}

esp_err_t init_fs(void)
{

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

#ifdef CONFIG_SPIFFS_CHECK_ON_START
    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(spiffs_conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return ret;
    } 
    ESP_LOGI(TAG, "SPIFFS_check() successful");   
#endif

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d, available: %d", total, used, total-used);
    }
  
    return ESP_OK;
}

size_t spiffs_available_bytes(){
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return 0;
    } 
    return total-used; 
}

void remove_all_data(){
    struct stat st;
    if (stat(DS18B20_FILE_NAME, &st) == 0) {
        unlink(DS18B20_FILE_NAME);
    }
    if (stat(BMX280_FILE_NAME, &st) == 0) {
        unlink(BMX280_FILE_NAME);
    }
}

esp_err_t insert_ds18b20(const ds18b20_data_t* data){
    FILE* f; 
    struct stat st;
    if (stat(DS18B20_FILE_NAME, &st) == 0) {
        f = fopen(DS18B20_FILE_NAME, "ab");
    }else{
        f = fopen(DS18B20_FILE_NAME, "wb");
    }
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for writing", DS18B20_FILE_NAME);
        return ESP_FAIL;
    }
    size_t sz = fwrite(data, sizeof(ds18b20_data_t), 1, f);
    fclose(f);
    if(sz == 0){
        ESP_LOGE(TAG, "Error writing to file %s", DS18B20_FILE_NAME);
        return ESP_FAIL;
    }
    stat(DS18B20_FILE_NAME, &st);
    ESP_LOGI(TAG, "Success file [%s] writing: %d bytes", DS18B20_FILE_NAME, sz);
    ESP_LOGI(TAG, "File %s size: %d bytes", DS18B20_FILE_NAME, (int)st.st_size);
    return ESP_OK;
}

esp_err_t insert_bmx280(const bmx280_data_t* data){
    FILE* f; 
    struct stat st;
    if (stat(BMX280_FILE_NAME, &st) == 0) {
        f = fopen(BMX280_FILE_NAME, "ab");
    }else{
        f = fopen(BMX280_FILE_NAME, "wb");
    }
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for writing", BMX280_FILE_NAME);
        return ESP_FAIL;
    }
    size_t sz = fwrite(data, sizeof(bmx280_data_t), 1, f);
    fclose(f);
    if(sz == 0){
        ESP_LOGE(TAG, "Error writing to file %s", BMX280_FILE_NAME);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t fetch_all_ds18b20(cJSON* root){
    esp_err_t ret = ESP_OK;
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for reading", DS18B20_FILE_NAME);
        return ESP_FAIL;
    }
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    do{
        ds18b20_data_t data;
        size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
        if(sz != sizeof(ds18b20_data_t) && ferror(f)){
            ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
            ret = ESP_FAIL;
            break; 
        }
        if( feof(f) ) break;
        count++;
        ds18b20_data_to_json(items, &data);
    }while( !feof(f) );
    cJSON_AddNumberToObject(root, "size", count);
    fclose(f);
    return ret;
}

esp_err_t fetch_min_ds18b20(cJSON* root, time_t begin, time_t end){
    esp_err_t ret = ESP_OK;
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for reading", DS18B20_FILE_NAME);
        return ESP_FAIL;
    }
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    time_t next = next_hour(begin);
    ds18b20_data_t data, min = {0};
    do{
        size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
        if(sz != sizeof(ds18b20_data_t) && ferror(f)){
            ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
            ret = ESP_FAIL;
            break; 
        }
        if( min.date_time == 0 || min.temperature < data.temperature){
            min = data;
        }
        if( data.date_time >= next || feof(f) || data.date_time >= end ){
            count++;
            ds18b20_data_to_json(items, &min);
            next = next_hour(next);
            min.date_time = 0;
        }
    }while( !feof(f) && data.date_time < end);
    cJSON_AddNumberToObject(root, "size", count);
    fclose(f);
    return ret;    
}

esp_err_t fetch_all_bmx280(cJSON* root){
    esp_err_t ret = ESP_OK;
    FILE* f = fopen(BMX280_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for reading", BMX280_FILE_NAME);
        return ESP_FAIL;
    }
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    do{
        bmx280_data_t data;
        size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
        if(sz != sizeof(bmx280_data_t) && ferror(f)){
            ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
            ret = ESP_FAIL;
            break; 
        }
        if( feof(f) ) break;
        count++;
        bmx280_data_to_json(items, &data);
    }while( !feof(f) );
    cJSON_AddNumberToObject(root, "size", count);
    fclose(f);
    return ret;
}
#endif