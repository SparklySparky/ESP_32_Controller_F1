idf_component_register(SRCS "car_control_read.c"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                        esp_adc
                        app_remote_control
                        car_control_gpios
                        car_control_types
                        car_control_buttons
                        car_control_init
                    )
