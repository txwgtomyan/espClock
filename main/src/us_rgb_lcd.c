#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "lvgl.h"

#include "us_rgb_lcd.h"
#include "esp_log.h"
static const char *TAG = "US RGB";

/* 屏幕引脚定义 */
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT -1//原理图没有背光
#define EXAMPLE_PIN_NUM_HSYNC 46
#define EXAMPLE_PIN_NUM_VSYNC 3
#define EXAMPLE_PIN_NUM_DE 5
#define EXAMPLE_PIN_NUM_PCLK 7
#define EXAMPLE_PIN_NUM_DATA0 14  // B3
#define EXAMPLE_PIN_NUM_DATA1 38  // B4
#define EXAMPLE_PIN_NUM_DATA2 18  // B5
#define EXAMPLE_PIN_NUM_DATA3 17  // B6
#define EXAMPLE_PIN_NUM_DATA4 10  // B7
#define EXAMPLE_PIN_NUM_DATA5 39  // G2
#define EXAMPLE_PIN_NUM_DATA6 0   // G3
#define EXAMPLE_PIN_NUM_DATA7 45  // G4
#define EXAMPLE_PIN_NUM_DATA8 48  // G5
#define EXAMPLE_PIN_NUM_DATA9 47  // G6
#define EXAMPLE_PIN_NUM_DATA10 21 // G7
#define EXAMPLE_PIN_NUM_DATA11 1  // R3
#define EXAMPLE_PIN_NUM_DATA12 2  // R4
#define EXAMPLE_PIN_NUM_DATA13 42 // R5
#define EXAMPLE_PIN_NUM_DATA14 41 // R6
#define EXAMPLE_PIN_NUM_DATA15 40 // R7
#define EXAMPLE_PIN_NUM_DISP_EN -1

/* 屏幕参数 */
#define EXAMPLE_LCD_H_RES              800
#define EXAMPLE_LCD_V_RES              480

/**
 * @brief 初始化屏幕
 *
 */
void us_rgb_lcd_init(void)
{
    /* 创建缓存 */
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    ESP_LOGI(TAG, "Create semaphores");
    sem_vsync_end = xSemaphoreCreateBinary();
    assert(sem_vsync_end);
    sem_gui_ready = xSemaphoreCreateBinary();
    assert(sem_gui_ready);
#endif

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
        .num_fbs = 1,//缓冲区的数量
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
            EXAMPLE_PIN_NUM_DATA8,
            EXAMPLE_PIN_NUM_DATA9,
            EXAMPLE_PIN_NUM_DATA10,
            EXAMPLE_PIN_NUM_DATA11,
            EXAMPLE_PIN_NUM_DATA12,
            EXAMPLE_PIN_NUM_DATA13,
            EXAMPLE_PIN_NUM_DATA14,
            EXAMPLE_PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            // The following parameters should refer to LCD spec
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
            .flags.pclk_active_neg = true,
        },
        .flags.fb_in_psram = true, // 在 PSRAM 中分配帧缓冲区
    };
    //ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
}