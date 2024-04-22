#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <car_control.h>
#include <car_control_conf.h>
#include <car_control_types.h>
#include <app_remote_control.h>
#include <iot_button.h>
#include <esp_log.h>

void car_control_read_init(void);
void car_control_read(void);

void read_data_long_click(void *arg, void *usr_data);
void limiter_send_cb(void *arg, void *usr_data);

static const char *TAG = "car control";

/*----- Button configurations -----*/
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


const car_interface_init_t car_interface_init = {
    .adc_oneshot_unit_init_cfg = {
        .unit_id = ADC_UNIT_1,            // Initialize ADC unit ID
        .ulp_mode = ADC_ULP_MODE_DISABLE  // Disable ULP mode
    }
};

const car_control_init_t car_control_throttle_init = {
    .chan_cfg = {
        .atten = ADC_ATTEN_DB_12,   // Set attenuation to 12dB
        .bitwidth = ADC_BITWIDTH_12 // Set ADC bit width to 12 bits
    }
};

const car_control_init_t car_control_steering_init = {
    .chan_cfg = {
        .atten = ADC_ATTEN_DB_12,   // Set attenuation to 12dB
        .bitwidth = ADC_BITWIDTH_12 // Set ADC bit width to 12 bits
    }
};

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
    send_data_t data_msg;
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
            data_msg.th_value *= 0x10;
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
            data_msg.st_value *= 0x10;
        }

        car_interface.steering.val_pr = car_interface.steering.val; // Update previous steering value

        ESP_LOGI(TAG, "TH -> %u, ST -> %u, VALUE -> %lu", data_msg.th_value, data_msg.st_value, *(uint32_t*) &data_msg);

        remote_control_send_data(ESPNOW_ATTRIBUTE_F1_CONTROL, *(uint32_t*) &data_msg); // Send data to the car

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

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
