#ifndef STEERING_WHEEL_H
#define STEERING_WHEEL_H

#include "gpio.h"
#include "stm32l4xx.h"
#include "can.h"
#include "can_controller.h"

typedef struct {
  uint8_t send_vehicle_speed_flag;
  uint8_t send_emergency_msg_flag;
  uint8_t time_send_flag;
  uint8_t lap_send_flag;
  uint8_t horn_flag;
} flags_t;

typedef struct {
  uint16_t ms_counter;
  uint16_t sec_counter;
  uint16_t min_counter;
  uint16_t sec_sum;  
  uint16_t min_sum;
} time_t;

extern volatile flags_t flags;
extern volatile time_t time;
extern volatile uint8_t lap_number;

void steering_wheel_init();
void steering_wheel_loop();


#endif