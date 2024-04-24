#ifndef CAR_CONTROL_GPIOS_H
#define CAR_CONTROL_GPIOS_H

// ----- GPIOs ----- //
#define TOGGLE_BUTTON_GPIO                  GPIO_NUM_25
#define LIMITER_BUTTON_GPIO                 GPIO_NUM_26
#define LED_GPIO                            GPIO_NUM_27

#define STEERING_OFFSET_INCR_BTN_GPIO       GPIO_NUM_22
#define STEERING_OFFSET_DECR_BTN_GPIO       GPIO_NUM_21

#define THROTTLE_COEFF_INCR_BTN_GPIO        GPIO_NUM_19
#define THROTTLE_COEFF_DECR_BTN_GPIO        GPIO_NUM_18



// ----- ADC throttle & steering channels -----//
#define THROTTLE_ADC_CHANNEL    ADC_CHANNEL_4
#define STEERING_ADC_CHANNEL    ADC_CHANNEL_5


// ----- ADC controls tolerance -----//
#define CONTROLS_TOLERANCE      50 

#endif
