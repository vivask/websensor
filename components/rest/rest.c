/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h" 
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include "handlers.h"

static const char *TAG = "REST";

#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)


#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    const char captive[] = "/generate_204";
    char filepath[FILE_PATH_MAX];
    ESP_LOGD(TAG, "URI : %s", req->uri);
    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/' || strncmp(req->uri, captive, sizeof(captive)) == 0) {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t start_rest_server(const char *base_path) 
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 27;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.lru_purge_enable = true;
    config.max_open_sockets = 5;

    ESP_LOGI(TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    /* URI handler for fetching setting info */
    httpd_uri_t settings_info_get_uri = {
        .uri = "/api/v1/settings/info",
        .method = HTTP_GET,
        .handler = settings_info_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &settings_info_get_uri);

    /* URI handler for setting hwclock */
    httpd_uri_t settings_hwclock_post_uri = {
        .uri = "/api/v1/settings/hwclock",
        .method = HTTP_POST,
        .handler = settings_hwclock_post_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &settings_hwclock_post_uri);

    /* URI handler for peripheral status */
    httpd_uri_t peripheral_info_get_uri = {
        .uri = "/api/v1/peripheral/info",
        .method = HTTP_GET,
        .handler = get_peripheral_status_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &peripheral_info_get_uri);

    /* URI handler for fetching ds18b20 all measurements */
    httpd_uri_t ds18b20_data_get_all_uri = {
        .uri = "/api/v1/ds18b20/read/all/*",
        .method = HTTP_GET,
        .handler = ds18b20_data_get_all_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &ds18b20_data_get_all_uri);

    /* URI handler for fetching ds18b20 min measurements */
    httpd_uri_t ds18b20_data_get_min_uri = {
        .uri = "/api/v1/ds18b20/read/temperature/min",
        .method = HTTP_GET,
        .handler = ds18b20_data_get_min_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &ds18b20_data_get_min_uri);

    /* URI handler for fetching ds18b20 max measurements */
    httpd_uri_t ds18b20_data_get_max_uri = {
        .uri = "/api/v1/ds18b20/read/temperature/max",
        .method = HTTP_GET,
        .handler = ds18b20_data_get_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &ds18b20_data_get_max_uri);

    /* URI handler for fetching ds18b20 avg measurements */
    httpd_uri_t ds18b20_data_get_avg_uri = {
        .uri = "/api/v1/ds18b20/read/temperature/avg",
        .method = HTTP_GET,
        .handler = ds18b20_data_get_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &ds18b20_data_get_avg_uri);

    /* URI handler for fetching bmx280 all measurements */
    httpd_uri_t bmx280_data_get_all_uri = {
        .uri = "/api/v1/bmx280/read/all/*",
        .method = HTTP_GET,
        .handler = bmx280_data_get_all_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_all_uri);

    /* URI handler for fetching bmx280 min temperature measurements */
    httpd_uri_t bmx280_data_get_temperature_min_uri = {
        .uri = "/api/v1/bmx280/read/temperature/min",
        .method = HTTP_GET,
        .handler = bmx280_data_get_temperature_min_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_temperature_min_uri);

    /* URI handler for fetching bmx280 max temperature measurements */
    httpd_uri_t bmx280_data_get_temperature_max_uri = {
        .uri = "/api/v1/bmx280/read/temperature/max",
        .method = HTTP_GET,
        .handler = bmx280_data_get_temperature_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_temperature_max_uri);

    /* URI handler for fetching bmx280 avg temperature measurements */
    httpd_uri_t bmx280_data_get_temperature_avg_uri = {
        .uri = "/api/v1/bmx280/read/temperature/avg",
        .method = HTTP_GET,
        .handler = bmx280_data_get_temperature_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_temperature_avg_uri);

    /* URI handler for fetching bmx280 min humidity measurements */
    httpd_uri_t bmx280_data_get_humidity_min_uri = {
        .uri = "/api/v1/bmx280/read/humidity/min",
        .method = HTTP_GET,
        .handler = bmx280_data_get_humidity_min_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_humidity_min_uri);

    /* URI handler for fetching bmx280 max humidity measurements */
    httpd_uri_t bmx280_data_get_humidity_max_uri = {
        .uri = "/api/v1/bmx280/read/humidity/max",
        .method = HTTP_GET,
        .handler = bmx280_data_get_humidity_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_humidity_max_uri);

    /* URI handler for fetching bmx280 avg humidity measurements */
    httpd_uri_t bmx280_data_get_humidity_avg_uri = {
        .uri = "/api/v1/bmx280/read/humidity/avg",
        .method = HTTP_GET,
        .handler = bmx280_data_get_humidity_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_humidity_avg_uri);

    /* URI handler for fetching bmx280 min pressure measurements */
    httpd_uri_t bmx280_data_get_pressure_min_uri = {
        .uri = "/api/v1/bmx280/read/pressure/min",
        .method = HTTP_GET,
        .handler = bmx280_data_get_pressure_min_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_pressure_min_uri);

    /* URI handler for fetching bmx280 max pressure measurements */
    httpd_uri_t bmx280_data_get_pressure_max_uri = {
        .uri = "/api/v1/bmx280/read/pressure/max",
        .method = HTTP_GET,
        .handler = bmx280_data_get_pressure_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_pressure_max_uri);

    /* URI handler for fetching bmx280 avg pressure measurements */
    httpd_uri_t bmx280_data_get_pressure_avg_uri = {
        .uri = "/api/v1/bmx280/read/pressure/avg",
        .method = HTTP_GET,
        .handler = bmx280_data_get_pressure_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &bmx280_data_get_pressure_avg_uri);

    /* URI handler for fetching aht all measurements */
    httpd_uri_t aht_data_get_all_uri = {
        .uri = "/api/v1/aht/read/all/*",
        .method = HTTP_GET,
        .handler = aht_data_get_all_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_all_uri);

    /* URI handler for fetching aht avg temperature measurements */
    httpd_uri_t aht_data_get_temperature_avg_uri = {
        .uri = "/api/v1/aht/read/temperature/avg",
        .method = HTTP_GET,
        .handler = aht_data_get_temperature_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_temperature_avg_uri);

    /* URI handler for fetching aht avg humidity measurements */
    httpd_uri_t aht_data_get_humidity_avg_uri = {
        .uri = "/api/v1/aht/read/humidity/avg",
        .method = HTTP_GET,
        .handler = aht_data_get_humidity_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_humidity_avg_uri);

    /* URI handler for fetching aht min temperature measurements */
    httpd_uri_t aht_data_get_temperature_min_uri = {
        .uri = "/api/v1/aht/read/temperature/min",
        .method = HTTP_GET,
        .handler = aht_data_get_temperature_min_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_temperature_min_uri);

    /* URI handler for fetching aht avg humidity measurements */
    httpd_uri_t aht_data_get_humidity_min_uri = {
        .uri = "/api/v1/aht/read/humidity/min",
        .method = HTTP_GET,
        .handler = aht_data_get_humidity_avg_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_humidity_min_uri);

    /* URI handler for fetching aht max temperature measurements */
    httpd_uri_t aht_data_get_temperature_max_uri = {
        .uri = "/api/v1/aht/read/temperature/max",
        .method = HTTP_GET,
        .handler = aht_data_get_temperature_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_temperature_max_uri);

    /* URI handler for fetching aht max humidity measurements */
    httpd_uri_t aht_data_get_humidity_max_uri = {
        .uri = "/api/v1/aht/read/humidity/max",
        .method = HTTP_GET,
        .handler = aht_data_get_humidity_max_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &aht_data_get_humidity_max_uri);

    /* URI handler for getting web server files */
    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}
