#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "lvgl.h"

#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_touch.h"
#include "esp_log.h"
static const char *TAG = "US Touch";

/* 触摸的引脚定义 */
#define EXAMPLE_PIN_NUM_SCLK           18
#define EXAMPLE_PIN_NUM_MOSI           19
#define EXAMPLE_PIN_NUM_MISO           21

void us_lcd_touch(void)
{
    
    esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = 800,
        .y_max = 480,
        .rst_gpio_num = -1,
        .int_gpio_num = -1,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_touch_handle_t tp = NULL;
    esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp);
}