idf_component_register(SRCS 
                        "app_main.c"
                        "./src/us_timer.c"
                        "./src/us_spiffs.c"
                        "./src/us_nvs.c"
                        "./src/us_gpio.c"
                        "./src/us_rgb_lcd.c"
                        "./src/us_ui.c"
                    INCLUDE_DIRS "."
                                "./Inc")
