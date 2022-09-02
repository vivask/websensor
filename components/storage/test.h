/**
 * @file test.h
 * @defgroup test test
 * @{
 *
 * ESP-IDF Storage for web server files
 *
 * Ported from esp-open-rtos
 *
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t test_generate_data(int count, time_t* begin, time_t* end);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif  // __TEST_H__
