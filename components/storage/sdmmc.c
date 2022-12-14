#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "storage.h"


static const char *TAG = "SDMMC";

#define MOUNT_POINT "/" CONFIG_WEB_MOUNT_POINT

#if CONFIG_USE_SPI_MODE
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO    CONFIG_SD_MISO_IO
#define PIN_NUM_MOSI    CONFIG_SD_MOSI_IO
#define PIN_NUM_CLK     CONFIG_SD_CLK_IO
#define PIN_NUM_CS      CONFIG_SD_CS_IO
#endif //USE_SPI_MODE

#if CONFIG_WEB_DEPLOY_SD
esp_err_t init_fs(void)
{
    esp_err_t ret;
#if CONFIG_USE_SPI_MODE  
    ESP_LOGI(TAG, "Using SPI peripheral"); 
    
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
    }    

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

#else
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    //sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    

    // To use 1-line SD mode, uncomment the following line:
    // host.flags = SDMMC_HOST_FLAG_1BIT;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes
#endif //USE_SPI_MODE

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = CONFIG_MAX_FILES,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;

#if CONFIG_USE_SPI_MODE    
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
#else
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
#endif

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    /* print card info if mount successfully */
    sdmmc_card_print_info(stdout, card);

    const char *test_file = MOUNT_POINT"/index.html";
    FILE* f = fopen(test_file, "r");
    if(f == NULL){
        ESP_LOGE(TAG, "Failed to opnen file for writing");
        return ESP_OK;
    }
    ESP_LOGI(TAG, "Succes open file: %s", test_file);
    fclose(f);
    return ESP_OK;
}
#endif