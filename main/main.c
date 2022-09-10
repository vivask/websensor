/* HTTP Restful API Server 

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "storage.h"
#include "softap.h"
#include "rest.h"
#include "peripheral.h"
#include "dns_server.h"


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

 
    ESP_ERROR_CHECK(init_fs());

    ESP_ERROR_CHECK(wifi_init_softap());

    peripheral_initialize();

    ESP_ERROR_CHECK(start_rest_server(CONFIG_WEB_BASE_PATH));

    start_dns_server();
}
