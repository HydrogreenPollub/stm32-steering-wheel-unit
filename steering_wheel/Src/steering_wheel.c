#include "steering_wheel.h"


volatile flags_t flags;
volatile time_t time;
volatile uint8_t lap_number = 0;
volatile uint8_t time_reset_button_press_counter = 0;

void steering_wheel_init()
{
  lv_init();
  lv_port_disp_init();
  ui_init();
  CAN_Init(&hcan1);
  CAN_FilterConfig(&hcan1);
}

void steering_wheel_loop()
{
    lv_timer_handler();
    ui_tick();
    disp_set_time(time.min_counter, time.sec_counter, time.min_sum, time.sec_sum, flags.time_send_flag);
    disp_set_lap_number(lap_number, flags.lap_send_flag);
    CAN_ReceiveMessage(&rx_data);
}
