#ifndef CAR_CONTROL_READ_H
#define CAR_CONTROL_READ_H

#include "car_control_types.h"

//----- Function declarations -----//
void car_control_read_init(void);
void car_control_read(void);

//----- External declaration of car interface -----//
extern car_interface_t car_interface;

#endif
