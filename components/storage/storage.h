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

esp_err_t init_fs(void);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __STORAGE_H__
