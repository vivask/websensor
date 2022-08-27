idf_component_register(
    SRCS sdmmc.c spiffs.c semihost.c repo.c
    INCLUDE_DIRS .
    REQUIRES log spiffs fatfs esp32-idf-sqlite3;
)
