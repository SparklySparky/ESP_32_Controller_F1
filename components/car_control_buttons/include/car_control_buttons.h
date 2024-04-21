#ifndef CAR_CONTROL_BUTTONS_H
#define CAR_CONTROL_BUTTONS_H

#include "car_control_types.h"
#include "iot_button.h"

//----- External declaration of task handle -----//
extern TaskHandle_t read_data_task_handle;

//----- Declare callback functions -----//
void read_data_long_click(void *arg, void *usr_data);
void limiter_send_cb(void *arg, void *usr_data);

//----- External declaration of button configurations -----//
extern button_config_t toggle_reading_btn;
extern button_event_config_t toggle_reading_cfg;
extern button_config_t limiter_btn;
extern button_event_config_t limiter_btn_cfg;

#endif