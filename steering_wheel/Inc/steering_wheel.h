#ifndef STEERING_WHEEL_H
#define STEERING_WHEEL_H

#include "gpio.h"
#include "stm32g4xx.h"
#include "fdcan.h"
#include "can_controller.h"
#include "ui.h"

typedef struct
{
    uint8_t send_vehicle_speed_flag;
    uint8_t send_emergency_msg_flag;
    uint8_t time_send_flag;
    uint8_t lap_send_flag;
    uint8_t horn_flag;
    uint8_t steering_wheel_tick_flag;
    uint8_t sc_voltage_send_flag;
    uint8_t disp_tick_flag;
    uint8_t can_rx_tick_flag;
} flags_t;

typedef struct
{
    uint16_t ms_counter;
    uint16_t disp_ms_counter;
    uint16_t sec_counter;
    uint16_t min_counter;
    uint64_t nanosec_to_master;
    uint16_t sec_sum;
    uint16_t min_sum;
} time_t;

typedef struct
{
    uint16_t sc_voltage_milivolts;
    float sc_voltage;
    float vehicle_speed;
    uint16_t vehicle_speed_times10;
    uint8_t lap_number;
} params_t;

typedef enum {
    Idle,
    Running,
    Shutdown,
    Failure,
} master_state_t;

typedef enum {
    Disconnected,
    SystemOff,
    FirmwareVersion,
    CommandNotFound,
    EnteringToStartingPhase,
    EnteringToRunningPhase,
    AnodeSupplyPressureCheck,
    TemperatureCheck,
    FCGasSystemCheck,
    FCSealingCheck,
    FCVoltageCheck,
    LowH2Supply,
    ShutdownInitiated,
    AbnormalShutdownInitiated,
    RunningProtium,
} protium_states_t;

extern volatile flags_t flags;
extern volatile time_t time;
extern params_t params;
// extern volatile uint8_t lap_number;
extern volatile uint8_t time_reset_button_press_counter;

void steering_wheel_init();
void steering_wheel_loop();

#endif