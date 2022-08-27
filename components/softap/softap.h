/**
 * @file softap.h
 * @defgroup softap softap
 * @{
 *
 * ESP-IDF WiFi Access Point
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __SOFTAP_H__
#define __SOFTAP_H__

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_init_softap(void);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __SOFTAP_H__
