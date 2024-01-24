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

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
static bool example_on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
// #if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
//     if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE) {
//         xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
//     }
// #endif
    return high_task_awoken == pdTRUE;
}

/**
 * @brief 画图函数
 * 
 * @param drv 
 * @param area 表示屏幕的一个区域。
 * @param color_map 
 */
static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    /* 将绘制缓冲区传递给驱动程序 */
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);//告诉LVGL画完了
}

/**
 * @brief LVGL定时器的回调函数
 * 
 * @param arg 指向不透明的用户特定数据的指针
 */
static void example_increase_lvgl_tick(void *arg)
{
    /* 告诉LVGL过去了多长时间 */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

extern void example_lvgl_demo_ui(lv_disp_t *disp);
/**
 * @brief 初始化屏幕
 *
 */
void us_rgb_lcd_init(void)
{
    /* 创建缓存 */
    static lv_disp_draw_buf_t disp_buf; // 包含称为绘制缓冲区的内部图形缓冲区。
    static lv_disp_drv_t disp_drv;      // 显示驱动的描述符,表示一个屏幕

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
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    
    /* 设置注册RGB面板事件回调 */
    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = example_on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    /* 初始化LVGL */
    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    /* 为lvgl缓存申请空间 */
    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);

    /* 初始化 LVGL 绘制缓冲区 */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * 100);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);//基本初始化
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;

    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);//注册驱动

    /* 给LVGL的心跳函数（lv_tick_inc）创建一个定时器 */
    ESP_LOGI(TAG, "Install LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    /* 初始化UI函数 */
    ESP_LOGI(TAG, "Display LVGL Scatter Chart");
    example_lvgl_demo_ui(disp);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_timer_handler();//运行lv_timer_handler的任务的优先级应该低于运行lv_tick_inc的任务。
    } 
}   