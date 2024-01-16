#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "us_nvs.h"
#include "esp_log.h"
static const char *TAG = "US_NVS";

/**
 * @brief 读写已有的KEY值
 * 
 */
void us_nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* 重新擦除再初始化 */
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* 打开分区 */
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else // 打开成功
    {

        ESP_LOGI(TAG, "Reading restart counter from NVS ... ");
        int32_t restart_counter = 0;

        /* 从NVS中查询KEY值进行读取 */
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter); // 依据k值读取数据
        switch (err)
        {
        case ESP_OK:
            ESP_LOGI(TAG, "Done");
            ESP_LOGI(TAG, "Restart counter = %" PRIu32, restart_counter);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG, "The value is not initialized yet!");
            break;
        default:
            ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
        }

        /* 写 */
        ESP_LOGI(TAG,"Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        if(err!=ESP_OK)
        {
            ESP_LOGE(TAG,"write Failed!");
        }
        else
        {
            ESP_LOGI(TAG,"write Done");
        }

        /* 提交，确保写入 */
        ESP_LOGI(TAG,"Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        if(err!=ESP_OK)
        {
            ESP_LOGE(TAG,"commit Failed!");
        }
        else
        {
            ESP_LOGI(TAG,"commit Done");
        }

        /* 关闭 */
        nvs_close(my_handle);


        /* 打印nvs分区，storage命名空间下所有的KEY值 */
        printf("------------------------------------\n");
        nvs_iterator_t it = NULL;
        esp_err_t res = nvs_entry_find("nvs", "storage", NVS_TYPE_ANY, &it);
        while (res == ESP_OK)
        {
            nvs_entry_info_t info;
            nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
            printf("key '%s', type '%d' \n", info.key, info.type);
            res = nvs_entry_next(&it);
        }
        nvs_release_iterator(it);
        printf("------------------------------------\n");
    }
}
