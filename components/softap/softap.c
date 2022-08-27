/*  WiFi softAP 

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define WEB_WIFI_SSID      CONFIG_WEB_WIFI_SSID
#define WEB_WIFI_PASS      CONFIG_WEB_WIFI_PASSWORD
#define WEB_WIFI_CHANNEL   CONFIG_WEB_WIFI_CHANNEL
#define WEB_MAX_STA_CONN   CONFIG_WEB_MAX_STA_CONN

static const char *TAG = "WIFI";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

esp_err_t wifi_init_softap(void)
{
    esp_err_t err;
    err = esp_netif_init();
    if( err != ESP_OK ) {
        return err;
    }

    err = esp_event_loop_create_default();
    if( err != ESP_OK ) {
        return err;
    }

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif);

    esp_netif_ip_info_t ip_info;
    memset(&ip_info, 0x00, sizeof(ip_info));
    inet_pton(AF_INET, CONFIG_WEB_AP_IP, &ip_info.ip);
    inet_pton(AF_INET, CONFIG_WEB_AP_GATEWAY, &ip_info.gw);
    inet_pton(AF_INET, CONFIG_WEB_AP_NETMASK, &ip_info.netmask);
    esp_netif_dhcps_stop(ap_netif);
    esp_netif_set_ip_info(ap_netif, &ip_info);
    esp_netif_dhcps_start(ap_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if( err != ESP_OK ) {
        return err;
    }

    err = esp_event_handler_instance_register(WIFI_EVENT,
                                              ESP_EVENT_ANY_ID,
                                              &wifi_event_handler,
                                              NULL,
                                              NULL);
    if( err != ESP_OK ) {
        return err;
    }


    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WEB_WIFI_SSID,
            .ssid_len = strlen(WEB_WIFI_SSID),
            .channel = WEB_WIFI_CHANNEL,
            .password = WEB_WIFI_PASS,
            .max_connection = WEB_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(WEB_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if( err != ESP_OK ) {
        return err;
    }
    err = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if( err != ESP_OK ) {
        return err;
    }
    err = esp_wifi_start();
    if( err != ESP_OK ) {
        return err;
    }

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             WEB_WIFI_SSID, WEB_WIFI_PASS, WEB_WIFI_CHANNEL);
    return ESP_OK;
}
