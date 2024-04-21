#include "car_control_types.h"


// ----- This file includes the constants required for car control initialization. -----//


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

