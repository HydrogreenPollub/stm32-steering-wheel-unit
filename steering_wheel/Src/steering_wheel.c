#include "steering_wheel.h"

volatile flags_t flags;
volatile time_t time;
params_t params;
volatile uint8_t time_reset_button_press_counter = 0;

void steering_wheel_init()
{
    flags.disp_tick_flag = 1;
    lv_init();
    lv_port_disp_init();
    ui_init();
    CAN_Init(&hfdcan2);
    CAN_FilterConfig(&hfdcan2);
    __HAL_TIM_SET_COUNTER(&htim7, 0);
    HAL_TIM_Base_Start_IT(&htim7);
}

void steering_wheel_loop()
{
    if (flags.disp_tick_flag)
    {
        lv_timer_handler();
        ui_tick();
        flags.disp_tick_flag = 0;
    }
    disp_set_time(time.min_counter, time.sec_counter, time.min_sum, time.sec_sum, flags.time_send_flag);
    disp_set_lap_number(params.lap_number, flags.lap_send_flag);
    CAN_ReceiveMessage(rx_data);
}
