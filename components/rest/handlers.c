/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h" 
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include "handlers.h"
#include "peripheral.h"
#include "storage.h"

#define DEFAULT_QUERY_SIZE 256

static const char *TAG = "HANDLERS";

static bool hwclock_init = false;
static time_t begin_loging = 0;
static time_t end_loging = 0;

#ifdef CONFIG_WEB_TEST_MODE
void test_settings_set(time_t begin, time_t end){
    hwclock_init = true;
    begin_loging = begin;
    end_loging = end;    
}

void test_settings_get(time_t* begin, time_t* end){
    *begin = begin_loging;
    *end = end_loging;
}
#endif

static void get_date(char* buf, const time_t time){
    struct tm* t = localtime(&time);  
    sprintf(buf, "20%02d-%02d-%02d", t->tm_year - 100, t->tm_mon + 1, t->tm_mday); 
}

static void get_time(char* buf, const time_t time){
    struct tm* t = localtime(&time);
    sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

static int split (const char *txt, char delim, char ***tokens)
{
    int *tklen, *t, count = 1;
    char **arr, *p = (char *) txt;

    while (*p != '\0') if (*p++ == delim) count += 1;
    t = tklen = calloc (count, sizeof (int));
    for (p = (char *) txt; *p != '\0'; p++) *p == delim ? *t++ : (*t)++;
    *tokens = arr = malloc (count * sizeof (char *));
    t = tklen;
    p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
    while (*txt != '\0')
    {
        if (*txt == delim)
        {
            p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
            txt++;
        }
        else *p++ = *txt++;
    }
    free (tklen);
    return count;
}

static char* get_value_from_uri(httpd_req_t *req, const char* uri){
    return req->uri + strlen(uri) + 1;
}

static esp_err_t get_select_params(slect_params_t* params, httpd_req_t *req, const char* uri){
    char* params_str = get_value_from_uri(req, uri);
    char **tokens;
    int count = split(params_str, ':', &tokens);
    if(count != 2){
        return ESP_FAIL;
    }
    params->items_on_page = atoi(tokens[0]);
    params->page_no = atoi(tokens[1]);
    return ESP_OK;
}

static char* get_request_buffer(httpd_req_t *req, esp_err_t* err){
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        *err = ESP_FAIL;
        return NULL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            *err = ESP_FAIL;
            return NULL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';
    *err = ESP_OK;
    return buf;
}

static bool start_peripheral_devices(){
    if(hwclock_init && begin_loging && end_loging){
        peripheral_start(begin_loging, end_loging);  
        return true;      
    }else{
        peripheral_stop(); 
    }    
    return false;
}

esp_err_t settings_info_get_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    char sys_date[11] = {0}, sys_time[9] = {0};
    char begin_date[11] = {0}, begin_time[9] = {0};
    char end_date[11] = {0}, end_time[9] = {0};
    if(hwclock_init){
        time_t now;
        time(&now);
        get_date(sys_date, now);
        get_time(sys_time, now);
    }
    if(begin_loging){
        get_date(begin_date, begin_loging);
        get_time(begin_time, begin_loging);
    }
    if(end_loging){
        get_date(end_date, end_loging);
        get_time(end_time, end_loging);
    }
    cJSON_AddStringToObject(root, "sys_date", sys_date);
    cJSON_AddStringToObject(root, "sys_time", sys_time);
    cJSON_AddStringToObject(root, "begin_date", begin_date);
    cJSON_AddStringToObject(root, "begin_time", begin_time);
    cJSON_AddStringToObject(root, "end_date", end_date);
    cJSON_AddStringToObject(root, "end_time", end_time);
    cJSON_AddBoolToObject(root, "ds18b20_available", ds18b20_available());
    cJSON_AddBoolToObject(root, "bmx280_available", bmx280_available());
#ifdef CONFIG_WEB_DEPLOY_SF    
    cJSON_AddNumberToObject(root, "space_capacity", spiffs_available_bytes());
#endif
    const char *settings_info = cJSON_Print(root);
    httpd_resp_sendstr(req, settings_info);
    free((void *)settings_info);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t settings_hwclock_post_handler(httpd_req_t *req){
    esp_err_t ret;
    char* buf = get_request_buffer(req, &ret);
    if( ret != ESP_OK ){
        return ret;
    }

    cJSON *root = cJSON_Parse(buf);
    char date_time[20];
    sprintf(date_time, "%s %s", cJSON_GetObjectItem(root, "date")->valuestring, cJSON_GetObjectItem(root, "time")->valuestring);
    cJSON_Delete(root);
    
    struct tm tm = {0};
    strptime(date_time, "%Y-%m-%d %H:%M:%S", &tm);
    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
    hwclock_init = true;

    bool status = start_peripheral_devices();
    httpd_resp_set_type(req, "application/json");
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "message", "Hwclock setup success");
    cJSON_AddBoolToObject(root, "status", status);
    const char *json_string = cJSON_Print(root);
    httpd_resp_sendstr(req, json_string);
    free((void *)json_string);
    cJSON_Delete(root);

    return ESP_OK;    
}

time_t validate_begin_time(time_t begin){
    return (hwclock_init && end_loging !=0 && begin > end_loging) ? end_loging-1 : begin;
}

time_t validate_end_time(time_t end){
    return (hwclock_init && begin_loging != 0 && end < begin_loging) ? begin_loging+1 : end;
}

esp_err_t settings_begin_post_handler(httpd_req_t *req){
    esp_err_t ret;
    char* buf = get_request_buffer(req, &ret);
    if( ret != ESP_OK ){
        return ret;
    }

    cJSON *root = cJSON_Parse(buf);
    char date_time[20];
    sprintf(date_time, "%s %s", cJSON_GetObjectItem(root, "date")->valuestring, cJSON_GetObjectItem(root, "time")->valuestring);
    cJSON_Delete(root);

    struct tm tm = {0};   
    strptime(date_time, "%Y-%m-%d %H:%M:%S", &tm);
    begin_loging = mktime(&tm);
    
    bool status = start_peripheral_devices();
    httpd_resp_set_type(req, "application/json");
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "message", "Begin loging setup success");
    cJSON_AddBoolToObject(root, "status", status);
    const char *json_string = cJSON_Print(root);
    httpd_resp_sendstr(req, json_string);
    free((void *)json_string);
    cJSON_Delete(root);

    return ESP_OK;    
}

esp_err_t settings_end_post_handler(httpd_req_t *req){
    esp_err_t ret;
    char* buf = get_request_buffer(req, &ret);
    if( ret != ESP_OK ){
        return ret;
    }

    cJSON *root = cJSON_Parse(buf);
    char date_time[20];
    sprintf(date_time, "%s %s", cJSON_GetObjectItem(root, "date")->valuestring, cJSON_GetObjectItem(root, "time")->valuestring);
    cJSON_Delete(root);

    struct tm tm = {0};
    strptime(date_time, "%Y-%m-%d %H:%M:%S", &tm);
    end_loging = mktime(&tm);
    
    bool status = start_peripheral_devices();
    httpd_resp_set_type(req, "application/json");
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "message", "End loging setup success");
    cJSON_AddBoolToObject(root, "status", status);
    const char *json_string = cJSON_Print(root);
    httpd_resp_sendstr(req, json_string);
    free((void *)json_string);
    cJSON_Delete(root);

    return ESP_OK;    
}

esp_err_t ds18b20_data_get_all_handler(httpd_req_t *req){
    esp_err_t ret;

    slect_params_t params;
    ret = get_select_params(&params, req, "/api/v1/ds18b20/read/all");
    if(ret != ESP_OK){
        return ret;
    }

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_all_ds18b20(root, &params);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    ESP_LOGI(TAG, "JSON: %s", json);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    
    return ESP_OK;
}

esp_err_t ds18b20_data_get_min_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_min_ds18b20(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    ESP_LOGI(TAG, "JSON: %s", json);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ds18b20_data_get_max_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_max_ds18b20(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    ESP_LOGI(TAG, "JSON: %s", json);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ds18b20_data_get_avg_handler(httpd_req_t *req){
      esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_avg_ds18b20(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    ESP_LOGI(TAG, "JSON: %s", json);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;  
}

esp_err_t bmx280_data_get_all_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_all_bmx280(root);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    ESP_LOGI(TAG, "JSON: %s", json);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_temperature_min_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_min_temperature_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_temperature_max_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_max_temperature_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_temperature_avg_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_avg_temperature_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_humidity_min_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_min_humidity_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_humidity_max_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_max_humidity_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_humidity_avg_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_avg_humidity_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_pressure_min_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_min_pressure_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_pressure_max_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_max_pressure_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t bmx280_data_get_pressure_avg_handler(httpd_req_t *req){
    esp_err_t ret;

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_avg_pressure_bmx280(root, begin_loging, end_loging);
    if(ret != ESP_OK){
        cJSON_Delete(root);
        return ret;
    }
    const char *json = cJSON_Print(root);
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}

