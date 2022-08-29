/**
 * @file spiffs.h
 * @defgroup storage storage
 * @{
 *
 * ESP-IDF Repository spiffs
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __SPIFFS_H__
#define __SPIFFS_H__

#include <stdint.h>
#include <esp_err.h>
#include <cJSON.h>
#include "storage.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t spiffs_available_bytes();
void remove_all_data();
esp_err_t insert_ds18b20(const ds18b20_data_t* data);
esp_err_t insert_bmx280(const bmx280_data_t* data);
esp_err_t fetch_all_ds18b20(cJSON* root);
esp_err_t fetch_min_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_ds18b20(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_all_bmx280(cJSON* root);
esp_err_t fetch_min_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_max_bmx280(cJSON* root, time_t begin, time_t end);
esp_err_t fetch_avg_bmx280(cJSON* root, time_t begin, time_t end);


#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __SPIFFS_H__
