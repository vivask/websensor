/**
 * @file peripheral.h
 * @defgroup peripheral peripheral
 * @{
 *
 * ESP-IDF Peripheral
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

void peripheral_initialize();
void peripheral_start(time_t begin, time_t end);
void peripheral_stop();
bool ds18b20_available();
bool bmx280_available();

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __PERIPHERAL_H__
