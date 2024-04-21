idf_component_register(SRCS "car_control_buttons.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                        esp_adc
                        app_remote_control
                        car_control_gpios
                        car_control_types
                        car_control_buttons
                        car_control_read
                    )
