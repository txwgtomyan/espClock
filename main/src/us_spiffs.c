#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "us_spiffs.h"
#include "esp_log.h"
static const char *TAG = "US_SPIFFS";

/**
 * @brief 测试程序
 *
 */
void us_spiffs_test(void)
{
    /* 写文件--->重命名文件--->读文件 */

    /* 按照可写模式打开文件 */
    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    /* 写文件 */
    fprintf(f, "Hello World!");

    /* 关闭文件 */
    fclose(f);
    ESP_LOGI(TAG, "File written");

    /* 检查文件是否存在 */
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0)
    {
        /* 如果存在，删除文件 */
        unlink("/spiffs/foo.txt");
    }

    /* 重命名文件 */
        ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    /* 打开重命名的文件 */
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    /* 从文件中读取 */
        char line[64];
    fgets(line, sizeof(line), f);//安全读取一行（到指定长度，或者到换行符。）
        fclose(f);

    /* 去掉换行符，将换行符替换为空格 */
    char* pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

}

/**
 * @brief 初始化文件系统
 *
 */
void us_spiffs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    /* 初始化文件的结构体 */
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,                // 同时打开文件的最大数量
        .format_if_mount_failed = true // 没有文件系统就初始化
    };

    /* 挂载文件系统 */
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    /* 获取文件系统的信息 */
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    /* 检查文件系统的一致性 */
    if (used > total)
    {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);
        // Could be also used to mend broken files, to clean unreferenced pages, etc.
        // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return;
        }
        else
        {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }
    
    us_spiffs_test();
    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}