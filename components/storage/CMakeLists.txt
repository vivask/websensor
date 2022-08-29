idf_component_register(
    SRCS sdmmc.c spiffs.c semihost.c
    INCLUDE_DIRS .
    REQUIRES log spiffs fatfs json;
)
