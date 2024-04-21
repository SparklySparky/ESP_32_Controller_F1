#include <stdio.h>
#include "esp_log.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"

#include "car_control_read.h"
#include "app_remote_control.h"
#include "car_control_gpios.h"
#include "car_control_types.h"
#include "car_control_buttons.h"
#include "car_control_init.h"


//----- This file includes the main functions of the controller -----//


static const char* TAG = "car_control_read";

//----- Initialize car_interface & data msg -----//
car_interface_t car_interface = {
    .init = &car_interface_init,
    .throttle = {
        .chan = THROTTLE_ADC_CHANNEL,
        .init = &car_control_throttle_init,
        .val = 0,
        .val_pr = 0,
    },
    .steering = {
        .chan = STEERING_ADC_CHANNEL,
        .init = &car_control_steering_init,
        .val = 0,
        .val_pr = 0,
    },
    .reading_state = 0,
    .limiter_state = 0,
};

send_data_t data_msg = {
    .th_value = 0,
    .st_value = 0,
};

//----- Initialize ADC unit handle -----//
adc_oneshot_unit_handle_t adc_handle;


void car_control_read_init(void)
{
    // Initialize ADC unit config
    adc_oneshot_unit_init_cfg_t init_cfg = car_interface.init->adc_oneshot_unit_init_cfg;

    esp_err_t err = adc_oneshot_new_unit(&init_cfg, &adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC initialization failed");
        return;
    }

    // Initialize channels config
    adc_oneshot_chan_cfg_t throttle_chan_cfg = car_interface.throttle.init->chan_cfg;
    adc_oneshot_chan_cfg_t steering_chan_cfg = car_interface.steering.init->chan_cfg;

    // Initialize singular channels config & read
    err = adc_oneshot_config_channel(adc_handle, car_interface.throttle.chan, &throttle_chan_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Throttle ADC channel configuration failed");
        return;
    }
    
    err = adc_oneshot_config_channel(adc_handle, car_interface.steering.chan, &steering_chan_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Steering ADC channel configuration failed");
        return;
    }

    // Create and register the buttons & led
    gpio_set_direction(LED_GPIO, GPIO_MODE_INPUT_OUTPUT);

    button_handle_t toggle_btn_handle = iot_button_create(&toggle_reading_btn);
    if (toggle_btn_handle == NULL) {
        ESP_LOGE(TAG, "Button create failed");
        return;
    }

    err = iot_button_register_event_cb(toggle_btn_handle, toggle_reading_cfg, read_data_long_click, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Button event registration failed");
        return;
    }

    button_handle_t limiter_btn_handle = iot_button_create(&limiter_btn);
    if (limiter_btn_handle == NULL) {
        ESP_LOGE(TAG, "Button create failed");
        return;
    }

    err = iot_button_register_event_cb(limiter_btn_handle, limiter_btn_cfg, limiter_send_cb, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Button event registration failed");
        return;
    }

    ESP_LOGI(TAG, "Initialization completed");
}

void car_control_read(void)
{
    while(1) {
        // Read the throttle values
        esp_err_t err = adc_oneshot_read(adc_handle, car_interface.throttle.chan, &car_interface.throttle.val);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "ADC read failed");
            continue;
        }

        // Check the throttle values
        if (abs(car_interface.throttle.val - car_interface.throttle.val_pr) > CONTROLS_TOLERANCE)
        {
            data_msg.th_value = (uint16_t) car_interface.throttle.val; // Update throttle value in data message
        }

        car_interface.throttle.val_pr = car_interface.throttle.val; // Update previous throttle value
        
        // Read the steering values
        err = adc_oneshot_read(adc_handle, car_interface.steering.chan, &car_interface.steering.val);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "ADC read failed");
            continue;
        }

        // Check the steering values
        if (abs(car_interface.steering.val - car_interface.steering.val_pr) > CONTROLS_TOLERANCE)
        {
            data_msg.st_value = (uint16_t) car_interface.steering.val; // Update steering value in data message
        }

        car_interface.steering.val_pr = car_interface.steering.val; // Update previous steering value

        ESP_LOGI(TAG, "TH -> %u, ST -> %u, VALUE -> %lu", data_msg.th_value, data_msg.st_value, *(uint32_t*) &data_msg);

        remote_control_send_data(ESPNOW_ATTRIBUTE_F1_CONTROL, *(uint32_t*) &data_msg); // Send data to the car

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
