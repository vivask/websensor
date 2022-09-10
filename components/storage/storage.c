#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "esp_log.h"
#include "storage.h"

#define MOUNT_POINT "/" CONFIG_WEB_MOUNT_POINT

static const char* DS18B20_FILE_NAME = MOUNT_POINT"/ds18b20.bin";
static const char* BMX280_FILE_NAME = MOUNT_POINT"/bmx280.bin";
static const char* AHT_FILE_NAME = MOUNT_POINT"/aht.bin";

static const char *TAG = "STORAGE";

static void get_date_time(char* date_time, time_t time){
    struct tm* t = localtime(&time); 
    sprintf(date_time, "20%02d-%02d-%02d %02d:%02d:%02d", t->tm_year - 100, t->tm_mon + 1,
           t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    ESP_LOGD(TAG, "TIME: %s", date_time);
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


static void aht_data_to_json(cJSON* items, const aht_data_t* data){
        char time[20];
        cJSON *item = cJSON_CreateObject();
        get_date_time(time, data->date_time);
        cJSON_AddStringToObject(item, "date_time", time);
        cJSON_AddNumberToObject(item, "temperature", data->temperature);
        cJSON_AddNumberToObject(item, "humidity", data->humidity);
        cJSON_AddItemToArray(items, item);
}

void remove_all_sensor_data(){
    struct stat st;
    if (stat(DS18B20_FILE_NAME, &st) == 0) {
        unlink(DS18B20_FILE_NAME);
    }
    if (stat(BMX280_FILE_NAME, &st) == 0) {
        unlink(BMX280_FILE_NAME);
    }
    if (stat(AHT_FILE_NAME, &st) == 0) {
        unlink(AHT_FILE_NAME);
    }
}

bool aht_data_is_exist() {
    struct stat st;
    return (stat(AHT_FILE_NAME, &st) == 0);
}

bool ds18b20_data_is_exist() {
    struct stat st;
    return (stat(DS18B20_FILE_NAME, &st) == 0);
}

bool bmh280_data_is_exist() {
    struct stat st;
    return (stat(BMX280_FILE_NAME, &st) == 0);
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
    ESP_LOGD(TAG, "Success file [%s] writing: %d bytes", DS18B20_FILE_NAME, sz);
    ESP_LOGD(TAG, "Size: %d bytes", (int)st.st_size);
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
    stat(BMX280_FILE_NAME, &st);
    ESP_LOGD(TAG, "Success file [%s] writing: %d bytes", BMX280_FILE_NAME, sz);
    ESP_LOGD(TAG, "File %s size: %d bytes", BMX280_FILE_NAME, (int)st.st_size);
    return ESP_OK;
}

esp_err_t insert_aht(const aht_data_t* data){
    FILE* f; 
    struct stat st;
    if (stat(AHT_FILE_NAME, &st) == 0) {
        f = fopen(AHT_FILE_NAME, "ab");
    }else{
        f = fopen(AHT_FILE_NAME, "wb");
    }
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file [%s] for writing", AHT_FILE_NAME);
        return ESP_FAIL;
    }
    size_t sz = fwrite(data, sizeof(aht_data_t), 1, f);
    fclose(f);
    if(sz == 0){
        ESP_LOGE(TAG, "Error writing to file %s", AHT_FILE_NAME);
        return ESP_FAIL;
    }
    stat(AHT_FILE_NAME, &st);
    ESP_LOGD(TAG, "Success file [%s] writing: %d bytes", AHT_FILE_NAME, sz);
    ESP_LOGD(TAG, "File %s size: %d bytes", AHT_FILE_NAME, (int)st.st_size);
    return ESP_OK;
}

static size_t get_ds18b20_pages(int items_on_page){
    struct stat st;
    stat(DS18B20_FILE_NAME, &st);
    float f_pages = st.st_size/sizeof(ds18b20_data_t);
    int INT = trunc(f_pages/items_on_page);
    float FLT = f_pages/items_on_page - trunc(f_pages/items_on_page);
    ESP_LOGD(TAG, "File size: %d, Item size: %d, INT: %d, FLT: %d", (int)st.st_size, (int)sizeof(ds18b20_data_t), INT, (int)((FLT != 0.0) ? 1.0 : 0.0));
    return  INT + (int)((FLT != 0.0) ? 1.0 : 0.0);
} 

static size_t get_bmx280_pages(int items_on_page){
    struct stat st;
    stat(BMX280_FILE_NAME, &st);
    float f_pages = st.st_size/sizeof(bmx280_data_t);
    int INT = trunc(f_pages/items_on_page);
    float FLT = f_pages/items_on_page - trunc(f_pages/items_on_page);
    ESP_LOGD(TAG, "File size: %d, Item size: %d, INT: %d, FLT: %d", (int)st.st_size, (int)sizeof(ds18b20_data_t), INT, (int)((FLT != 0.0) ? 1.0 : 0.0));
    return  INT + (int)((FLT != 0.0) ? 1.0 : 0.0);
} 

static size_t get_aht_pages(int items_on_page){
    struct stat st;
    stat(AHT_FILE_NAME, &st);
    float f_pages = st.st_size/sizeof(aht_data_t);
    int INT = trunc(f_pages/items_on_page);
    float FLT = f_pages/items_on_page - trunc(f_pages/items_on_page);
    ESP_LOGI(TAG, "File size: %d, Item size: %d, INT: %d, FLT: %d", (int)st.st_size, (int)sizeof(ds18b20_data_t), INT, (int)((FLT != 0.0) ? 1.0 : 0.0));
    return  INT + (int)((FLT != 0.0) ? 1.0 : 0.0);
} 

esp_err_t fetch_all_ds18b20(cJSON* root, slect_params_t* params){
    esp_err_t ret = ESP_OK;
    int count = 0, idx=0;
    int start = (params->page_no==1) ? 0 : (params->page_no-1)*params->items_on_page;
    int end = start + params->items_on_page;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", DS18B20_FILE_NAME);
    }else{
        do{
            ds18b20_data_t data;
            size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
            if(idx >= start) {           
                if(sz != sizeof(ds18b20_data_t) && ferror(f)){
                    ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
                    ret = ESP_FAIL;
                    break; 
                }
                if( feof(f) ) break;
                count++;
                ds18b20_data_to_json(items, &data);
            }
            idx++;
        }while( idx < end && !feof(f) );
        fclose(f);
    }
    size_t pages = get_ds18b20_pages(params->items_on_page);   
    ESP_LOGD(TAG, "Items: %d, Pages: %d, Count: %d", params->items_on_page, pages, count);
    cJSON_AddNumberToObject(root, "pages", pages);
    cJSON_AddNumberToObject(root, "size", count);
    return ret;
}

//fetch the minimum values per hour in a given period
esp_err_t fetch_min_ds18b20(cJSON* root, time_t begin, time_t end){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", DS18B20_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        ds18b20_data_t data, min;
        memset(&min, 0x00, sizeof(ds18b20_data_t));
        do{
            size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
            if(sz != sizeof(ds18b20_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                if( min.date_time == 0 || min.temperature > data.temperature){
                    memcpy(&min, &data, sizeof(ds18b20_data_t));
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    ds18b20_data_to_json(items, &min);
                    next = next_hour(next);
                    memset(&min, 0x00, sizeof(ds18b20_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;    
}

//fetch the maximum values per hour in a given period
esp_err_t fetch_max_ds18b20(cJSON* root, time_t begin, time_t end){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Not exist file: %s", DS18B20_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        ds18b20_data_t data, max;
        memset(&max, 0x00, sizeof(ds18b20_data_t));
        do{
            size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
            if(sz != sizeof(ds18b20_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                if( max.date_time == 0 || data.temperature > max.temperature){
                    memcpy(&max, &data, sizeof(ds18b20_data_t));
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    ds18b20_data_to_json(items, &max);
                    next = next_hour(next);
                    memset(&max, 0x00, sizeof(ds18b20_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;    
}

//fetch the average values per hour in a given period
esp_err_t fetch_avg_ds18b20(cJSON* root, time_t begin, time_t end){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(DS18B20_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", DS18B20_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        ds18b20_data_t data, avg;
        memset(&avg, 0x00, sizeof(ds18b20_data_t));
        int idx = 0;
        float tmp = 0.0;
        float average[24];
        do{
            size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
            if(sz != sizeof(ds18b20_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                tmp += data.temperature;
                idx++;
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    average[count] = tmp / idx;
                    count++;
                    next = next_hour(next);
                    idx = 0;
                }
            }
        }while( !feof(f) && data.date_time < end);
        fseek(f, 0L, SEEK_SET);
        count = 0;
        next = next_hour(begin);
        float dev = average[0];
        do{
            size_t sz = fread(&data, sizeof(ds18b20_data_t), 1, f);
            if(sz != sizeof(ds18b20_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", DS18B20_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time > begin) {
                if( avg.date_time == 0 || dev > fabs(average[count] - data.temperature )){
                    dev = fabs(average[count] - data.temperature);
                    memcpy(&avg, &data, sizeof(ds18b20_data_t));
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    ds18b20_data_to_json(items, &avg);
                    next = next_hour(next);
                    memset(&avg, 0x00, sizeof(ds18b20_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;    
}

esp_err_t fetch_all_bmx280(cJSON* root, slect_params_t* params){
    esp_err_t ret = ESP_OK;
    int count = 0, idx=0;
    int start = (params->page_no==1) ? 0 : (params->page_no-1)*params->items_on_page;
    int end = start + params->items_on_page;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(BMX280_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", BMX280_FILE_NAME);
    }else{
        do{
            bmx280_data_t data;
            size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
            if(idx >= start) {           
                if(sz != sizeof(bmx280_data_t) && ferror(f)){
                    ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
                    ret = ESP_FAIL;
                    break; 
                }
                if( feof(f) ) break;
                count++;
                bmx280_data_to_json(items, &data);
            }
            idx++;
        }while( idx < end && !feof(f) );
        fclose(f);
    }
    size_t pages = get_bmx280_pages(params->items_on_page);
    cJSON_AddNumberToObject(root, "pages", pages);
    cJSON_AddNumberToObject(root, "size", count);
    return ret;
}

typedef enum {
    TEMPERATURE, HUMIDITY, PRESSURE
}sensor_weigth_t;

//fetch the minimum values per hour in a given period
static esp_err_t fetch_min_bmx280(cJSON* root, time_t begin, time_t end, sensor_weigth_t param){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(BMX280_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", BMX280_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        bmx280_data_t data, min;
        memset(&min, 0x00, sizeof(bmx280_data_t));
        do{
            size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
            if(sz != sizeof(bmx280_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                switch (param)
                {
                case TEMPERATURE:
                    if( min.date_time == 0 || min.temperature > data.temperature){
                        memcpy(&min, &data, sizeof(bmx280_data_t));
                    }
                    break;
                case HUMIDITY:
                    if( min.date_time == 0 || min.humidity > data.humidity){
                        memcpy(&min, &data, sizeof(bmx280_data_t));
                    }
                    break;
                case PRESSURE:
                    if( min.date_time == 0 || min.pressure > data.pressure){
                        memcpy(&min, &data, sizeof(bmx280_data_t));
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Undefined parameter: %d", param);
                    return ESP_FAIL;
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    bmx280_data_to_json(items, &min);
                    next = next_hour(next);
                    memset(&min, 0x00, sizeof(bmx280_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

//fetch the maximum values per hour in a given period
static esp_err_t fetch_max_bmx280(cJSON* root, time_t begin, time_t end, sensor_weigth_t param){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(BMX280_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Not exist file: %s", BMX280_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        bmx280_data_t data, max;
        memset(&max, 0x00, sizeof(bmx280_data_t));
        do{
            size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
            if(sz != sizeof(bmx280_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                switch (param)
                {
                case TEMPERATURE:
                    if( max.date_time == 0 || data.temperature > max.temperature ){
                        memcpy(&max, &data, sizeof(bmx280_data_t));
                    }
                    break;
                case HUMIDITY:
                    if( max.date_time == 0 || data.humidity > max.humidity ){
                        memcpy(&max, &data, sizeof(bmx280_data_t));
                    }
                    break;
                case PRESSURE:
                    if( max.date_time == 0 || data.pressure > max.pressure ){
                        memcpy(&max, &data, sizeof(bmx280_data_t));
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Undefined parameter: %d", param);
                    return ESP_FAIL;
                }

                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    bmx280_data_to_json(items, &max);
                    next = next_hour(next);
                    memset(&max, 0x00, sizeof(bmx280_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end );
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

static esp_err_t fetch_avg_bmx280(cJSON* root, time_t begin, time_t end, sensor_weigth_t param){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(BMX280_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", BMX280_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        bmx280_data_t data, avg;
        memset(&avg, 0x00, sizeof(bmx280_data_t));
        int idx = 0;
        float tmp_temperature = 0.0, tmp_humidity = 0.0, tmp_pressure = 0.0;
        float average_temperature[24];
        float average_humidity[24];
        float average_pressure[24];
        do{
            size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
            if(sz != sizeof(bmx280_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                tmp_temperature += data.temperature;
                tmp_humidity += data.humidity;
                tmp_pressure += data.pressure;
                idx++;
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    average_temperature[count] = tmp_temperature / idx;
                    average_humidity[count] = tmp_humidity / idx;
                    average_pressure[count] = tmp_pressure / idx;
                    count++;
                    next = next_hour(next);
                    idx = 0;
                }
            }
        }while( !feof(f) && data.date_time < end);
        fseek(f, 0L, SEEK_SET);
        count = 0;
        next = next_hour(begin);
        float dev_temperature = average_temperature[0];
        float dev_humidity = average_humidity[0];
        float dev_pressure = average_pressure[0];
        do{
            size_t sz = fread(&data, sizeof(bmx280_data_t), 1, f);
            if(sz != sizeof(bmx280_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", BMX280_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                switch (param)
                {
                case TEMPERATURE:
                    if( avg.date_time == 0 || dev_temperature > fabs(average_temperature[count] - data.temperature) ){
                        dev_temperature = fabs(average_temperature[count] - data.temperature);
                        memcpy(&avg, &data, sizeof(bmx280_data_t));
                    }
                    break;                
                case HUMIDITY:
                    if( avg.date_time == 0 || dev_humidity > fabs(average_humidity[count] - data.humidity) ){
                        dev_humidity = fabs(average_humidity[count] - data.humidity);
                        memcpy(&avg, &data, sizeof(bmx280_data_t));
                    }
                    break;          
                case PRESSURE:
                    if( avg.date_time == 0 || dev_pressure > fabs(average_pressure[count] - data.pressure) ){
                        dev_pressure = fabs(average_pressure[count] - data.pressure);
                        memcpy(&avg, &data, sizeof(bmx280_data_t));
                    }
                    break;          
                default:
                    ESP_LOGE(TAG, "Undefined parameter: %d", param);
                    return ESP_FAIL;
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    bmx280_data_to_json(items, &avg);
                    next = next_hour(next);
                    memset(&avg, 0x00, sizeof(bmx280_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

esp_err_t fetch_min_temperature_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_min_bmx280(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_max_temperature_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_max_bmx280(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_avg_temperature_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_avg_bmx280(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_min_humidity_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_min_bmx280(root, begin, end, HUMIDITY);
}

esp_err_t fetch_max_humidity_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_max_bmx280(root, begin, end, HUMIDITY);
}

esp_err_t fetch_avg_humidity_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_avg_bmx280(root, begin, end, HUMIDITY);
}

esp_err_t fetch_min_pressure_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_min_bmx280(root, begin, end, PRESSURE);
}

esp_err_t fetch_max_pressure_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_max_bmx280(root, begin, end, PRESSURE);
}

esp_err_t fetch_avg_pressure_bmx280(cJSON* root, time_t begin, time_t end){
    return fetch_avg_bmx280(root, begin, end, PRESSURE);
}

esp_err_t fetch_all_aht(cJSON* root, slect_params_t* params){
    esp_err_t ret = ESP_OK;
    int count = 0, idx=0;
    int start = (params->page_no==1) ? 0 : (params->page_no-1)*params->items_on_page;
    int end = start + params->items_on_page;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(AHT_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", AHT_FILE_NAME);
    }else{
        do{
            aht_data_t data;
            size_t sz = fread(&data, sizeof(aht_data_t), 1, f);
            if(idx >= start) {           
                if(sz != sizeof(aht_data_t) && ferror(f)){
                    ESP_LOGE(TAG, "Error reading to file %s", AHT_FILE_NAME);
                    ret = ESP_FAIL;
                    break; 
                }
                if( feof(f) ) break;
                count++;
                aht_data_to_json(items, &data);
            }
            idx++;
        }while( idx < end && !feof(f) );
        fclose(f);
    }
    size_t pages = get_aht_pages(params->items_on_page);   
    ESP_LOGI(TAG, "Items: %d, Pages: %d, Count: %d", params->items_on_page, pages, count);
    cJSON_AddNumberToObject(root, "pages", pages);
    cJSON_AddNumberToObject(root, "size", count);
    return ret;
}

static esp_err_t fetch_avg_aht(cJSON* root, time_t begin, time_t end, sensor_weigth_t weight){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(AHT_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", AHT_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        aht_data_t data, avg;
        memset(&avg, 0x00, sizeof(aht_data_t));
        int idx = 0;
        float tmp_temperature = 0.0, tmp_humidity = 0.0;
        float average_temperature[24];
        float average_humidity[24];
        do{
            size_t sz = fread(&data, sizeof(aht_data_t), 1, f);
            if(sz != sizeof(aht_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", AHT_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time > begin) {
                tmp_temperature += data.temperature;
                tmp_humidity += data.humidity;
                idx++;
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    average_temperature[count] = tmp_temperature / idx;
                    average_humidity[count] = tmp_humidity / idx;
                    count++;
                    next = next_hour(next);
                    idx = 0;
                }
            }
        }while( !feof(f) && data.date_time < end );
        fseek(f, 0L, SEEK_SET);
        count = 0;
        float dev_temperature = average_temperature[0];
        float dev_humidity = average_humidity[0];
        next = next_hour(begin);
        do{
            size_t sz = fread(&data, sizeof(aht_data_t), 1, f);
            if(sz != sizeof(aht_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", AHT_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time > begin) {
                switch (weight)
                {
                case TEMPERATURE:
                    if( avg.date_time == 0 || dev_temperature > fabs(average_temperature[count] - data.temperature) ){
                        dev_temperature = fabs(average_temperature[count] - data.temperature);
                        memcpy(&avg, &data, sizeof(aht_data_t));
                    }
                    break;                
                case HUMIDITY:
                    if( avg.date_time == 0 || dev_humidity > fabs(average_humidity[count] - data.humidity) ){
                        dev_humidity = fabs(average_humidity[count] - data.humidity);
                        memcpy(&avg, &data, sizeof(aht_data_t));
                    }
                    break;          
                default:
                    ESP_LOGE(TAG, "Undefined weight: %d", weight);
                    return ESP_FAIL;
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    aht_data_to_json(items, &avg);
                    next = next_hour(next);
                    memset(&avg, 0x00, sizeof(aht_data_t));
                    count++;
                }
            }
        }while( !feof(f) && data.date_time < end  );
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

//fetch the minimum values per hour in a given period
static esp_err_t fetch_min_aht(cJSON* root, time_t begin, time_t end, sensor_weigth_t param){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(AHT_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGW(TAG, "Not exist file: %s", AHT_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        aht_data_t data, min;
        memset(&min, 0x00, sizeof(aht_data_t));
        do{
            size_t sz = fread(&data, sizeof(aht_data_t), 1, f);
            if(sz != sizeof(aht_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", AHT_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                switch (param)
                {
                case TEMPERATURE:
                    if( min.date_time == 0 || min.temperature > data.temperature){
                        memcpy(&min, &data, sizeof(aht_data_t));
                    }
                    break;
                case HUMIDITY:
                    if( min.date_time == 0 || min.humidity > data.humidity){
                        memcpy(&min, &data, sizeof(aht_data_t));
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Undefined parameter: %d", param);
                    return ESP_FAIL;
                }
                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    aht_data_to_json(items, &min);
                    next = next_hour(next);
                    memset(&min, 0x00, sizeof(aht_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end);
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

//fetch the maximum values per hour in a given period
static esp_err_t fetch_max_aht(cJSON* root, time_t begin, time_t end, sensor_weigth_t param){
    esp_err_t ret = ESP_OK;
    int count = 0;
    cJSON *items = cJSON_AddArrayToObject(root, "items");
    FILE* f = fopen(AHT_FILE_NAME, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Not exist file: %s", AHT_FILE_NAME);
    }else{
        time_t next = next_hour(begin);
        aht_data_t data, max;
        memset(&max, 0x00, sizeof(aht_data_t));
        do{
            size_t sz = fread(&data, sizeof(aht_data_t), 1, f);
            if(sz != sizeof(aht_data_t) && ferror(f)){
                ESP_LOGE(TAG, "Error reading to file %s", AHT_FILE_NAME);
                ret = ESP_FAIL;
                break; 
            }
            if(data.date_time < begin) {
                continue;
            }else{
                switch (param)
                {
                case TEMPERATURE:
                    if( max.date_time == 0 || data.temperature > max.temperature ){
                        memcpy(&max, &data, sizeof(aht_data_t));
                    }
                    break;
                case HUMIDITY:
                    if( max.date_time == 0 || data.humidity > max.humidity ){
                        memcpy(&max, &data, sizeof(aht_data_t));
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Undefined parameter: %d", param);
                    return ESP_FAIL;
                }

                if( data.date_time >= next || feof(f) || data.date_time >= end ){
                    count++;
                    aht_data_to_json(items, &max);
                    next = next_hour(next);
                    memset(&max, 0x00, sizeof(aht_data_t));
                }
            }
        }while( !feof(f) && data.date_time < end );
        fclose(f);
    }
    cJSON_AddNumberToObject(root, "size", count);
    return ret;        
}

esp_err_t fetch_avg_temperature_aht(cJSON* root, time_t begin, time_t end){
    return fetch_avg_aht(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_avg_humidity_aht(cJSON* root, time_t begin, time_t end){
    return fetch_avg_aht(root, begin, end, HUMIDITY);
}

esp_err_t fetch_min_temperature_aht(cJSON* root, time_t begin, time_t end){
    return fetch_min_aht(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_min_humidity_aht(cJSON* root, time_t begin, time_t end){
    return fetch_min_aht(root, begin, end, HUMIDITY);
}

esp_err_t fetch_max_temperature_aht(cJSON* root, time_t begin, time_t end){
    return fetch_max_aht(root, begin, end, TEMPERATURE);
}

esp_err_t fetch_max_humidity_aht(cJSON* root, time_t begin, time_t end){
    return fetch_max_aht(root, begin, end, HUMIDITY);
}