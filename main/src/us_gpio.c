#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "us_gpio.h"
#include "esp_log.h"
//static const char *TAG = "US_GPIO";

#define GPIO_OUTPUT_IO_0    5
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0))

#define GPIO_INPUT_IO_0    6
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))

#define ESP_INTR_FLAG_DEFAULT 0

void us_gpio_init(void)
{
    /* 初始化结构体 */
    gpio_config_t io_conf = {};

    /* 配置IO结构体 */
    io_conf.intr_type = GPIO_INTR_DISABLE;//禁用中断
    io_conf.mode = GPIO_MODE_OUTPUT;//设置为输出模式
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;//引脚的位置掩码
    io_conf.pull_down_en = 0;//禁用上拉
    io_conf.pull_up_en = 0;//禁用下拉

    /* 使用配置初始化IO */
    gpio_config(&io_conf);//配置输出引脚0

    io_conf.intr_type = GPIO_INTR_POSEDGE;//上升沿中断
    io_conf.mode = GPIO_MODE_INPUT;//设置为输入模式
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;//引脚的位置掩码
    gpio_config(&io_conf);//配置输入引脚0

    /* 改变中断类型 */
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    /* 安装gpio isr服务 */
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
}