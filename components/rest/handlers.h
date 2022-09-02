/**
 * @file handlers.h
 * @defgroup rest rest
 * @{
 *
 * ESP-IDF REST Server
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#ifdef CONFIG_WEB_TEST_MODE
    void test_settings_set(time_t begin, time_t end);
    void test_settings_get(time_t* begin, time_t* end);
#endif

esp_err_t settings_info_get_handler(httpd_req_t *req);
esp_err_t settings_hwclock_post_handler(httpd_req_t *req);
esp_err_t settings_begin_post_handler(httpd_req_t *req);
esp_err_t settings_end_post_handler(httpd_req_t *req);
esp_err_t ds18b20_data_get_all_handler(httpd_req_t *req);
esp_err_t ds18b20_data_get_min_handler(httpd_req_t *req);
esp_err_t ds18b20_data_get_max_handler(httpd_req_t *req);
esp_err_t ds18b20_data_get_avg_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_all_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_temperature_min_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_temperature_max_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_temperature_avg_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_humidity_min_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_humidity_max_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_humidity_avg_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_pressure_min_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_pressure_max_handler(httpd_req_t *req);
esp_err_t bmx280_data_get_pressure_avg_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __HANDLERS_H__
