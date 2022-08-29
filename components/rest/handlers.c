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
#include "spiffs.h"

#define DEFAULT_QUERY_SIZE 256

static const char *TAG = "handlers";

static bool hwclock_init = false;
static time_t begin_loging = 0;
static time_t end_loging = 0;

static void get_date(char* buf, const time_t time){
    struct tm* t = localtime(&time);  
    sprintf(buf, "20%02d-%02d-%02d", t->tm_year - 100, t->tm_mon + 1, t->tm_mday); 
}

static void get_time(char* buf, const time_t time){
    struct tm* t = localtime(&time);
    sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

static char* get_value_from_uri(httpd_req_t *req, const char* uri){
    return req->uri + strlen(uri) + 1;
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

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    ret = fetch_all_ds18b20(root);
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
    httpd_resp_sendstr(req, json);
    free((void *)json);
    cJSON_Delete(root);
    return ESP_OK;
}