/**
 * @file repo.h
 * @defgroup storage storage
 * @{
 *
 * ESP-IDF Repository sqlite3
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __REPO_H__
#define __REPO_H__

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

typedef struct{
    void* data;
    int rows;
}sql_result_t;

esp_err_t database_initialize();
esp_err_t insert_ds18b20(ds18b20_data_t *data);
esp_err_t insert_bmx280(bmx280_data_t *data);
esp_err_t clear_ds18b20();
esp_err_t clear_bmx280();
esp_err_t fetch_ds18b20(const char* query);
sql_result_t* get_ds18b20();
void free_ds18b20();
esp_err_t fetch_bmx280(const int from, const int to);
sql_result_t* get_bmx280();
void free_bmx280();

void get_date_time(char* date_time, time_t time);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __REPO_H__
