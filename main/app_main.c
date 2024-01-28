/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

/* 我写的文件 */
#include "us_timer.h"
#include "us_spiffs.h"
#include "us_nvs.h"
#include "us_rgb_lcd.h"
#include "net/us_ap.h"
/* 日志打印头文件和代码 */
#include "esp_log.h"
static const char *TAG = "MAIN APP";


static void test_task_example(void * arg)
{
    ESP_LOGI(TAG,"example task start");

    for(;;)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"test run");
    }
}

void app_main(void)
{
    printf("Hello world!\n");

#if 1
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
#endif

    ESP_LOGI(TAG, "system is start");

    // xTaskCreatePinnedToCore(test_task_example,"test_task_example",2048,NULL,10,NULL,0);
    
    // us_timer_init();
    //us_spiffs_init();
    //us_nvs_init();
    //us_rgb_lcd_init();
    us_ap_start();
    for (;;)
    {
        /* 死循环 */
        vTaskDelay(1);
    }
}
