#ifndef CAR_CONTROL_GPIOS_H
#define CAR_CONTROL_GPIOS_H


// ----- This file includes all the gpios & adc channel used in the controller -----//


// ----- GPIOs ----- //
#define TOGGLE_BUTTON_GPIO      GPIO_NUM_25 // ESP Pin Num: 25
#define LIMITER_BUTTON_GPIO     GPIO_NUM_27 // ESP Pin Num: 26
#define LED_GPIO                GPIO_NUM_26 // ESP Pin Num: 27


// ----- ADC throttle & steering channels -----//
#define THROTTLE_ADC_CHANNEL    ADC_CHANNEL_4 // ESP Pin Num: 32
#define STEERING_ADC_CHANNEL    ADC_CHANNEL_5 // ESP Pin Num: 33


// ----- ADC controls tolerance -----//
#define CONTROLS_TOLERANCE      50 

#endif
