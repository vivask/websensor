if(CONFIG_WEB_TEST_MODE)
idf_component_register(
        SRCS sdmmc.c spiffs.c semihost.c storage.c test.c
    INCLUDE_DIRS .
    REQUIRES log spiffs fatfs json;
)
else()
idf_component_register(
        SRCS sdmmc.c spiffs.c semihost.c storage.c
    INCLUDE_DIRS .
    REQUIRES log spiffs fatfs json;
)
endif()

