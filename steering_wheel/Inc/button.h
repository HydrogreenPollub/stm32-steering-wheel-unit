#ifndef BUTTON_H
#define BUTTON_H

#include "can_controller.h"
#include "can_ids.h"
#include "gpio.h"
#include "steering_wheel.h"
#include "stm32g4xx.h"
#include "tim.h"
#include <stdio.h>
#include <stdint.h>

#define NUM_BUTTONS 11

typedef enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASED,
} button_state_t;

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
    button_state_t button_state;
    bool debouncing_flag;
    uint32_t can_id;
} button_t;

typedef struct __attribute__((packed))
{
    uint16_t button_full_gas_bit : 1;
    uint16_t button_half_gas_bit : 1;
    uint16_t button_mode1_bit : 1;
    uint16_t button_mode2_bit : 1;
    uint16_t button_horn_bit : 1;
    uint16_t button_time_reset_bit : 1;
    uint16_t button_sc_close_bit : 1;
    uint16_t button_emergency_bit : 1;
    uint16_t button_fc_race_mode : 1;
    uint16_t button_fc_prepare_to_race_mode_bit : 1;
    uint16_t button_fc_off_mode_bit : 1;
    uint16_t reserved : 5;
} buttons_bits_t;

typedef union __attribute__((packed))
{
    buttons_bits_t buttons_bits;
    uint16_t all_button_states;
} buttons_states_t;

#endif