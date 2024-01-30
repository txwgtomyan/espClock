#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "sdkconfig.h"

#include "esp_log.h"
static const char *TAG = "US API";

/* ---------------------声明其他函数--------------------- */
extern void us_wifi_init(void);
/* ----------------------------------------------------- */

#define WEB_SERVER "quan.suning.com"
#define WEB_PORT "80"
#define WEB_PATH "/getSysTime.do"

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
                             "Host: " WEB_SERVER ":" WEB_PORT "\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    struct addrinfo *res; // 存放地址信息
    //struct in_addr *addr;
    int s, r;
    char recv_buf[64];//缓存

    while (1)
    {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res); // 获取地址信息

        if (err != 0 || res == NULL)
            {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        s = socket(res->ai_family, res->ai_socktype, 0); // 套接字API 错误
        if (s < 0)
        {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res); // 释放地址信息
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if (connect(s, res->ai_addr, res->ai_addrlen) != 0)
        {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... connected"); // 连接成功

        freeaddrinfo(res); // 释放地址信息

        if (write(s, REQUEST, strlen(REQUEST)) < 0)
        {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0)
        {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* 读取HTTP响应 */
        do
        {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf) - 1);
            for (int i = 0; i < r; i++)
            {
                putchar(recv_buf[i]);
            }
        } while (r > 0);
        printf("\r\n");
        
        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        vTaskDelay(10000);
    }
}

void us_api_init(void)
{
    us_wifi_init();

    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
}
