/**
 * @file storage.h
 * @defgroup storage storage
 * @{
 *
 * ESP-IDF Storage for web server files
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdint.h>
#include <esp_err.h>
#include <cJSON.h>
#if CONFIG_WEB_DEPLOY_SF
#include "spiffs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    time_t date_time;
    float temperature;
    float pressure;
    float humidity;
}bmx280_data_t;

typedef struct{
    time_t date_time;
    float temperature;
}ds18b20_data_t;

typedef struct{
    time_t date_time;
    float temperature;
    float humidity;
}aht_data_t;

typedef struct{
    int items_on_page;
    int page_no;
}slect_params_t;

esp_err_t init_fs(void);
void remove_all_sensor_data();
bool aht_data_is_exist();
bool ds18b20_data_is_exist();
bool bmh280_data_is_exist();
esp_err_t insert_ds18b20(const ds18b20_data_t* data);
esp_err_t insert_bmx280(const bmx280_data_t* data);
esp_err_t insert_aht(const aht_data_t* data);
esp_err_t fetch_all_ds18b20(cJSON* root, slect_params_t* params);
esp_err_t fetch_min_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_all_bmx280(cJSON* root, slect_params_t* params);
esp_err_t fetch_min_temperature_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_temperature_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_temperature_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_min_humidity_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_humidity_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_humidity_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_min_pressure_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_pressure_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_pressure_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_all_aht(cJSON* root, slect_params_t* params);
esp_err_t fetch_avg_temperature_aht(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_humidity_aht(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_min_temperature_aht(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_min_humidity_aht(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_temperature_aht(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_humidity_aht(cJSON* root, time_t begin, time_t end);


#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __STORAGE_H__
