# 笔记

## CMakeLists.txt修改案例

```cmake
idf_component_register(SRCS 
                        "app_main.c"
                        "./src/us_timer.c"
                        "./src/us_spiffs.c"
                    INCLUDE_DIRS "."
                                "./Inc")
```

## 定时器
定时器初始化逻辑
1. 创建定时器
2. 注册回调函数
3. 使能定时器
4. 配置报警值
5. 开启定时器

## spiffs文件系统

1. 复制partitios_example.csv文件到工程根目录并且改名为partitions.csv
2. 修改 menuconfig中的CONFIG_PARTITION_TABLE_CUSTOM参数为y
3. 清空工程的便衣文件，然后重新编译
