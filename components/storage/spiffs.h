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

#ifdef __cplusplus
extern "C" {
#endif

size_t spiffs_available_bytes();


#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __SPIFFS_H__
