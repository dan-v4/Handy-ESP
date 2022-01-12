/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "sys_constants.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

static const char *TAG = "example";

sdmmc_card_t *card;

void sd_init(){
    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = HOST_SLOT;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 20000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CS;
    slot_config.host_id = host.slot;

    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
}

bool find_file(char *pt){
    if(access(pt, F_OK) != -1){
        return true;
    }
    return false;
}

bool find_dir(char *pt){
    struct stat buffer;
    if(stat(pt, &buffer) != -1){
        return true;
    }
    return false;
}

void check_new_init(){
  if(!find_dir("/sdcard/sys")){
    mkdir("/sdcard/sys" ,S_IRWXU);
    mkdir("/sdcard/sys/apps" ,S_IRWXU);
    mkdir("/sdcard/sys/files" ,S_IRWXU);
    mkdir("/sdcard/sys/settings" ,S_IRWXU);
    mkdir("/sdcard/sys/apps/simpletxt" ,S_IRWXU);
    mkdir("/sdcard/sys/apps/paintpad" ,S_IRWXU);
    mkdir("/sdcard/sys/apps/calculator" ,S_IRWXU);
    FILE* system_file = fopen(menul_path, "wb"); 
    fprintf(system_file, base_menu);
    fclose(system_file);
    system_file = fopen(appl_path, "wb"); 
    fprintf(system_file, base_apps);
    fclose(system_file);
    system_file = fopen(filel_path, "wb"); 
    fprintf(system_file, base_files);
    fclose(system_file);
    system_file = fopen(settingsl_path, "wb"); 
    fprintf(system_file, base_settings);
    fclose(system_file);
    system_file = fopen(bgs_path, "wb"); 
    fprintf(system_file, base_set_bg);
    fclose(system_file);
    system_file = fopen(fts_path, "wb"); 
    fprintf(system_file, base_set_ft);
    fclose(system_file);
  }else{
      FILE* system_file = fopen(bgs_path, "rb"); 
      char bgr_char[4] = "";
      char bgg_char[4] = "";
      char bgb_char[4] = "";
      char nouse0[20] = "";
      fgets(nouse0, sizeof(nouse0), system_file);
      fgets(bgr_char, sizeof(bgr_char), system_file);
      fgets(bgg_char, sizeof(bgg_char), system_file);
      fgets(bgb_char, sizeof(bgb_char), system_file);
      uint8_t bg_red = strtol(bgr_char, NULL,10 );
      uint8_t bg_green = strtol(bgg_char, NULL,10 );
      uint8_t bg_blue = strtol(bgb_char, NULL,10 );
      background =  (uint32_t)bg_red << 10 | (uint32_t)bg_green <<4| (uint32_t)bg_blue << 2;
      system_file = fopen(fts_path, "rb"); 
      char nouse1[20] = "";
      char ftr_char[4] = "";
      char ftg_char[4] = "";
      char ftb_char[4] = "";
      fgets(nouse1, sizeof(nouse1), system_file);
      fgets(ftr_char, sizeof(ftr_char), system_file);
      fgets(ftg_char, sizeof(ftg_char), system_file);
      fgets(ftb_char, sizeof(ftb_char), system_file);
      uint8_t ft_red = strtol(ftr_char, NULL,10 );
      uint8_t ft_green = strtol(ftg_char, NULL,10 );
      uint8_t ft_blue = strtol(ftb_char, NULL,10 );
      font_colour =  (uint32_t)ft_red << 10 | (uint32_t)ft_green <<4 | (uint32_t)ft_blue << 2;
  }
}

FILE* get_file(char * path, char * perms){
    FILE* file = fopen(path, perms);
    return file;
}