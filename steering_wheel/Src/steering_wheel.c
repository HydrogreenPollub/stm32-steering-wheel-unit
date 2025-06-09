#include "steering_wheel.h"

volatile flags_t flags;
volatile time_t time;
volatile params_t params;
// volatile uint8_t lap_number = 0;
volatile uint8_t time_reset_button_press_counter = 0;

void steering_wheel_init()
{
    lv_init();
    lv_port_disp_init();
    ui_init();
    CAN_Init(&hfdcan2);
    CAN_FilterConfig(&hfdcan2);
}

void steering_wheel_loop()
{
    lv_timer_handler();
    ui_tick();
    disp_set_time(time.min_counter, time.sec_counter, time.min_sum, time.sec_sum, flags.time_send_flag);
    disp_set_lap_number(params.lap_number, flags.lap_send_flag);
    disp_set_icon(flags.horn_flag);
    CAN_ReceiveMessage(rx_data);
    // disp_set_sc_voltage(rx_data[0], flags.sc_voltage_send_flag);
}
