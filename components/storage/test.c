#include <sys/random.h>
#include <sys/param.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_random.h"
#include "esp_log.h"
#include "storage.h"

static const char *TAG = "RANDOM";

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)
{
    // Flags are ignored because:
    // - esp_random is non-blocking so it works for both blocking and non-blocking calls,
    // - don't have opportunity so set som other source of entropy.

    ESP_LOGD(TAG, "getrandom(buf=0x%x, buflen=%d, flags=%u)", (int) buf, buflen, flags);

    if (buf == NULL) {
        errno = EFAULT;
        ESP_LOGD(TAG, "getrandom returns -1 (EFAULT)");
        return -1;
    }

    esp_fill_random(buf, buflen);

    ESP_LOGD(TAG, "getrandom returns %d", buflen);
    return buflen;
}

static void set_hwclock(const char* time){
    struct tm tm = {0};
    strptime(time, "%Y-%m-%d %H:%M:%S", &tm);
    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
}


esp_err_t test_generate_data(int count, time_t* begin, time_t* end){
    set_hwclock("2022-09-02 10:00:00");
    time_t t = time(0);
    struct tm* tm = localtime(&t);  
    *begin = mktime(tm);
    //ds18b20_data_t ds18b20 = {0};
    bmx280_data_t bmx280 = {0};
    uint32_t random;
    for(int i=0; i<count; i++){
        t = mktime(tm);
        //ds18b20.date_time = t;
        bmx280.date_time = t;
        getrandom(&random, sizeof(uint32_t), 0);
        //ds18b20.temperature = random/10000000;
        bmx280.temperature = random/10000000;
        bmx280.humidity = random/10000000/100*(99-1)+1;
        bmx280.pressure = random/1000000*0.83;
        /*esp_err_t ret = insert_ds18b20(&ds18b20);
        if(ret != ESP_OK){
            return ret;
        }*/
        esp_err_t ret = insert_bmx280(&bmx280);
        if(ret != ESP_OK){
            return ret;
        }
        ESP_LOGI(TAG, "Count: %d", i);
        tm->tm_sec += 5;
    }
    *end = t;
    return ESP_OK;
}