#include <sys/stat.h>
#include <sys/unistd.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "storage.h"

static const char *TAG = "SPIFFS";

#if CONFIG_WEB_DEPLOY_SF

static esp_vfs_spiffs_conf_t spiffs_conf = {
    .base_path = CONFIG_WEB_BASE_PATH,
    .partition_label = NULL,
    .max_files = CONFIG_MAX_FILES,
    .format_if_mount_failed = true
};

esp_err_t init_fs(void)
{

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

#ifdef CONFIG_SPIFFS_CHECK_ON_START
    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(spiffs_conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return ret;
    } 
    ESP_LOGI(TAG, "SPIFFS_check() successful");   
#endif

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d, available: %d", total, used, total-used);
    }
    return ESP_OK;
}

size_t spiffs_available_bytes(){
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return 0;
    } 
    return total-used; 
}

#endif