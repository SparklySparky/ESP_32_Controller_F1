#ifndef CAR_CONTROL_TYPES_H
#define CAR_CONTROL_TYPES_H
          
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"


typedef struct {
    adc_oneshot_chan_cfg_t chan_cfg;
} car_control_init_t;


typedef struct {
    adc_channel_t chan;
    car_control_init_t *init;
    int val;
    int val_pr;
} car_control_t;


typedef struct {
    adc_oneshot_unit_init_cfg_t adc_oneshot_unit_init_cfg;
} car_interface_init_t;


typedef struct {
    adc_oneshot_unit_handle_t adc_oneshot_unit_handle;
    car_interface_init_t *init;

    car_control_t throttle;
    car_control_t steering;
    int reading_state;
    int limiter_state;
    int st_offset_state;
    int th_coeff_state;
} car_interface_t;


typedef struct __packed {
    uint16_t th_value;
    uint16_t st_value;
} send_data_t;

#endif
