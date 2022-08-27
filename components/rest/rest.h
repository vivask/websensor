/**
 * @file rest.h
 * @defgroup rest rest
 * @{
 *
 * ESP-IDF REST Server
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __REST_H__
#define __REST_H__

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t start_rest_server(const char *base_path);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __REST_H__
