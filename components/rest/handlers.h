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

esp_err_t light_brightness_post_handler(httpd_req_t *req);
esp_err_t temperature_data_get_handler(httpd_req_t *req);

esp_err_t settings_info_get_handler(httpd_req_t *req);
esp_err_t settings_hwclock_post_handler(httpd_req_t *req);
esp_err_t settings_begin_post_handler(httpd_req_t *req);
esp_err_t settings_end_post_handler(httpd_req_t *req);
esp_err_t ds18b20_data_post_handler(httpd_req_t *req);
esp_err_t bmx280_data_post_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __HANDLERS_H__
