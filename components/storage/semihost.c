#include "esp_vfs_semihost.h"
#include "storage.h"



#if CONFIG_WEB_DEPLOY_SEMIHOST
esp_err_t init_fs(void)
{
    esp_err_t ret = esp_vfs_semihost_register(CONFIG_WEB_MOUNT_POINT, CONFIG_HOST_PATH_TO_MOUNT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register semihost driver (%s)!", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif
