
#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/gptimer.h"

#include "us_timer.h"
#include "esp_log.h"
static const char *TAG = "US TIMER";

#define TIMER_DIVIDER 16                                    //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER / 1000) // convert counter value to ms seconds
#define TIMER_INTERVAL0_SEC (10)                            // sample test interval for the first timer
#define TEST_WITH_RELOAD 1                                  // testing will be done with auto reload

typedef struct
{
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
} timer_event_t;

/* 创建一个队列对象 */
QueueHandle_t queue;

timer_event_t g_timer_event;

/**
 * @brief 定时器的回调函数
 * 
 * @param timer 定时器
 * @param edata 定时器计数值
 * @param user_data 用户传递的数据，在注册回调函数时配置
 * @return true 正常
 * @return false 错误
 */
static bool IRAM_ATTR example_timer_on_alarm_cb_v1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    /* 存放函数结果 */
    BaseType_t high_task_awoken = pdFALSE;

    return (high_task_awoken == pdTRUE);
}

void us_timer_init(void)
{
    /* 创建了一个队列， */
    g_timer_event.timer_minute_count = 0;
    g_timer_event.timer_second_count = 0;
    queue = xQueueCreate(10, sizeof(g_timer_event));

    /* 创建一个定时器 */
    gptimer_handle_t gptimer = NULL; // 一个定时器句柄
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    /* 注册定时器回调函数 */
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb_v1,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));

    /* 使能定时器 */
    ESP_LOGI(TAG, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    /* 配置定时器报警值 */
    ESP_LOGI(TAG, "Start timer, stop it at alarm event");
    gptimer_alarm_config_t alarm_config1 = {
        .alarm_count = 1000000, // period = 1s
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    
    /* 开启定时器 */
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}