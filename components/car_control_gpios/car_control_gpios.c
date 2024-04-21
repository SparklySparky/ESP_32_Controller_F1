#include <stdio.h>
#include "driver/gpio.h"
#include "car_control_gpios.h"

// Define GPIOs
#define TOGGLE_BUTTON_GPIO      GPIO_NUM_25
#define LIMITER_BUTTON_GPIO   GPIO_NUM_27
#define LED_GPIO                GPIO_NUM_26

// Define ADC Channels
#define THROTTLE_ADC_CHANNEL    ADC_CHANNEL_4
#define STEERING_ADC_CHANNEL    ADC_CHANNEL_5