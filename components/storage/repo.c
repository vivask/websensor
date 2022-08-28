#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"
#include "sqlite3.h"
#include "storage.h"
#include "repo.h"

static const char *TAG = "repo";

static char db_file_name[] = "sqlite.db";
static sqlite3 *db = NULL;

static sql_result_t res_ds18b20;
static sql_result_t res_bmx280;

void get_date_time(char* date_time, time_t time){
    //struct tm* t = localtime(&time); 
    struct tm t = {0};
    localtime_r(&time, &t);
    sprintf(date_time, "20%02d-%02d-%02d %02d:%02d:%02d", t.tm_year - 100, t.tm_mon + 1,
           t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    ESP_LOGI(TAG, "TIME: %s", date_time);
}

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
   printf("%s: ", (const char*)data);
   for (i = 0; i<argc; i++){
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int db_open(const char *filename, sqlite3 **db) {
   int rc = sqlite3_open(filename, db);
   if (rc) {
       ESP_LOGE(TAG, "Can't open database: %s", sqlite3_errmsg(*db));
       return rc;
   } else {
       ESP_LOGI(TAG, "Opened database successfully");
   }
   return rc;
}

char *zErrMsg = 0;
static int db_exec(sqlite3 *db, const char *sql) {
   ESP_LOGI(TAG, "%s", sql);
   int64_t start = esp_timer_get_time();
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       ESP_LOGE(TAG, "SQL error: %s", zErrMsg);
       sqlite3_free(zErrMsg);
   } else {
       ESP_LOGI(TAG, "Operation done successfully");
   }
   ESP_LOGI(TAG, "Time taken: %lld", esp_timer_get_time()-start);
   return rc;
}

esp_err_t database_initialize(){
    int rc;

    char path[32];
    sprintf(path, "%s/%s", CONFIG_WEB_BASE_PATH, db_file_name);

    sqlite3_initialize();
    if( db_open(path, &db) ){
        return ESP_FAIL;
    }

    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS init (id TINYINT);");
    if( rc != SQLITE_OK){
        sqlite3_close(db);
        return ESP_FAIL;
    }

    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS bmx280 (time DATETIME PRIMARY KEY, temperature FLOAT NOT NULL, pressure FLOAT NOT NULL, humidity FLOAT);");
    if( rc != SQLITE_OK){
        sqlite3_close(db);
        return ESP_FAIL;
    }

    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS ds18b20 (time DATETIME PRIMARY KEY, temperature FLOAT NOT NULL);");
    if( rc != SQLITE_OK){
        sqlite3_close(db);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t insert_ds18b20(ds18b20_data_t* data){
    char time[32];
    char query[256];
    get_date_time(time, data->date_time);
    sprintf(query, "INSERT INTO ds18b20 VALUES ('%s', %f);", time, data->temperature);
    int rc = db_exec(db, query);
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    return ESP_OK;
}


esp_err_t insert_bmx280(bmx280_data_t* data){
    char time[32];
    char query[256];
    get_date_time(time, data->date_time);
    sprintf(query, "INSERT INTO bmx280 VALUES ('%s', %f, %f, %f);", time, data->temperature, data->pressure, data->humidity);
    int rc = db_exec(db, query);
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t clear_ds18b20(){
    int rc = db_exec(db, "DELETE FROM ds18b20;");
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t clear_bmx280(){
    int rc = db_exec(db, "DELETE FROM bmx280;");
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t fetch_ds18b20(const char* query){
    sqlite3_stmt*        stmt;
    int                  rows = 0;

    int rc = sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        rows++;
    }
    if(rows == 0){
        sqlite3_finalize( stmt );
        return ESP_OK;
    }
    sqlite3_reset( stmt );
    res_ds18b20.data = malloc(sizeof(ds18b20_data_t)*rows);
    ds18b20_data_t* p_res = (ds18b20_data_t*)res_ds18b20.data;
    res_ds18b20.rows = rows;
    struct tm tm = {0};
    int idx = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW){
        ds18b20_data_t item;
        strptime((char*)sqlite3_column_text(stmt, 0), "%Y-%m-%d %H:%M:%S", &tm);
        item.date_time = mktime(&tm);
        item.temperature = atof((char*)sqlite3_column_text(stmt, 1));
        p_res[idx++] = item;
    }
    sqlite3_finalize(stmt);
    return ESP_OK;    
}

sql_result_t* get_ds18b20(){
    return &res_ds18b20;
}

void free_ds18b20(){
    free(res_ds18b20.data);
    res_ds18b20.data = NULL;
    res_ds18b20.rows = 0;
}

esp_err_t fetch_bmx280(const int from, const int to){
    sqlite3_stmt*        stmt;
    int                  rows = 0;
    const char           query[] = "SELECT * FROM bmx280;";

    int rc = sqlite3_prepare(db, query, sizeof(query), &stmt, NULL);
    if( rc != SQLITE_OK){
        return ESP_FAIL;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        rows++;
    }
    if(rows == 0){
        sqlite3_finalize( stmt );
        return ESP_OK;
    }
    sqlite3_reset( stmt );
    res_bmx280.data = malloc(sizeof(bmx280_data_t)*rows);
    bmx280_data_t* p_res = (bmx280_data_t*)res_ds18b20.data;
    res_bmx280.rows = rows;
    struct tm tm = {0};
    int idx = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW){
        bmx280_data_t item;
        strptime((char*)sqlite3_column_text(stmt, 0), "%Y-%m-%d %H:%M:%S", &tm);
        item.date_time = mktime(&tm);
        item.temperature = atof((char*)sqlite3_column_text(stmt, 1));
        item.pressure = atof((char*)sqlite3_column_text(stmt, 2));
        item.humidity = atof((char*)sqlite3_column_text(stmt, 3));
        p_res[idx++] = item;
    }
    sqlite3_finalize(stmt);
    return ESP_OK;    
}

sql_result_t* get_bmx280(){
    return &res_bmx280;
}

void free_bmx280(){
    free(res_bmx280.data);    
    res_bmx280.data = NULL;
    res_bmx280.rows = 0;
}