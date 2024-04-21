#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "car_control_buttons.h"
#include "car_control_types.h"
#include "car_control_gpios.h"
#include "car_control_read.h"
#include "app_remote_control.h"
#include "iot_button.h"
#include "esp_log.h"


// ----- This file includes all functions, configs & handle necessary for the buttons management -----//


static const char* TAG = "car_control_buttons";

//----- Task Handle initialize -----//
TaskHandle_t read_data_task_handle = NULL;

//----- Read data task callback function -----//
void read_data_long_click(void *arg, void *usr_data)
{   
    // Implementation of read data task
    switch(car_interface.reading_state){
        case 0:
            car_interface.reading_state = 1;
            gpio_set_level(LED_GPIO, 1);
            xTaskCreate(
                (TaskFunction_t) &car_control_read,
                "Read Data",
                2048,
                NULL,
                1,
                &read_data_task_handle
            );

            ESP_LOGI(TAG, "READING STARTED");
            break;
        case 1:
            car_interface.reading_state = 0;
            gpio_set_level(LED_GPIO, 0);
            vTaskDelete(read_data_task_handle);
            ESP_LOGI(TAG, "READING STOPPED");
            break;
    }
}

//----- Limiter button callback function -----//
void limiter_send_cb(void *arg, void *usr_data)
{
    // Implementation of limiter button callback
    switch(car_interface.limiter_state){
        case 0:
            car_interface.limiter_state = 1;
            
            remote_control_send_data(ESPNOW_ATTRIBUTE_F1_LIMITER, 1);
            ESP_LOGI(TAG, "LIMITER SET");
            break;
        case 1:
            car_interface.limiter_state = 0;
            
            remote_control_send_data(ESPNOW_ATTRIBUTE_F1_LIMITER, 0);
            ESP_LOGI(TAG, "LIMITER REMOVED");
            break;
    }
}

//----- Define button configurations -----//
button_config_t toggle_reading_btn = {
    .type = BUTTON_TYPE_GPIO,
    .short_press_time = BUTTON_SHORT_PRESS_TIME_MS,
    .long_press_time = BUTTON_LONG_PRESS_TIME_MS,
    .gpio_button_config = {
        .gpio_num = TOGGLE_BUTTON_GPIO,
        .active_level = 0,
    },
};

button_event_config_t toggle_reading_cfg = {
    .event = BUTTON_LONG_PRESS_START,
    .event_data.long_press = {
        .press_time = 2000,
    },
};

button_config_t limiter_btn = {
    .type = BUTTON_TYPE_GPIO,
    .short_press_time = BUTTON_SHORT_PRESS_TIME_MS,
    .long_press_time = BUTTON_LONG_PRESS_TIME_MS,
    .gpio_button_config = {
        .gpio_num = LIMITER_BUTTON_GPIO,
        .active_level = 0,
    },
};

button_event_config_t limiter_btn_cfg = {
    .event = BUTTON_SINGLE_CLICK,
};
