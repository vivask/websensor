idf_component_register(
    SRCS rest.c handlers.c peripheral.c
    INCLUDE_DIRS .
    REQUIRES log esp_http_server json storage i2cdev bmp280 ds18x20 aht
)
