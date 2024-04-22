#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "car_control.h"
#include "app_remote_control.h"

void app_main(void)
{
    ESP_ERROR_CHECK(initialize_remote_control());
    car_control_read_init();
} 