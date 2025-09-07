#include "button.h"

extern TIM_HandleTypeDef htim6;

buttons_states_t button_states;

button_t buttons[NUM_BUTTONS] = {

    {
        BUTTON_TIME_RESET_GPIO_Port, BUTTON_TIME_RESET_Pin, BUTTON_STATE_RELEASED, 0,
        // CAN_ID_BUTTONS_STEERING_MASK,
    },
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        if (buttons[i].pin == GPIO_Pin && buttons[i].debouncing_flag == 0)
        {
            buttons[i].debouncing_flag = 1;
            __HAL_TIM_SET_COUNTER(&htim6, 0);
            HAL_TIM_Base_Start_IT(&htim6);
            break;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim == &htim6)
    {
        // HAL_TIM_Base_Stop_IT(htim);

        for (int i = 0; i < NUM_BUTTONS; i++)
        {
            if (buttons[i].debouncing_flag)
            {
                GPIO_PinState state = HAL_GPIO_ReadPin(buttons[i].port, buttons[i].pin);
                buttons[i].debouncing_flag = 0;

                if (state == GPIO_PIN_RESET && buttons[i].button_state == BUTTON_STATE_RELEASED)
                {
                    buttons[i].button_state = BUTTON_STATE_PRESSED;
                    // button_states.all_button_states |= (1 << i);
                    // tx_data[0] = (uint8_t) button_states.all_button_states & 0xFF;
                    // tx_data[1] = (uint8_t)(button_states.all_button_states >> 8) & 0xFF;

                    switch (buttons[i].pin)
                    {
                        case BUTTON_TIME_RESET_Pin:
                            if (time_reset_button_press_counter > 0)
                            {
                                tx_data[0] = params.lap_number;
                                // uint64_t dupa = 0x1122334455667788;
                                CAN_SendMessage(CAN_ID_LAP_NUMBER, tx_data, 1);

                                time.nanosec_to_master = time.min_counter * SECONDS_IN_MINUTE * NANOSECONDS_IN_SECOND
                                                         + time.sec_counter * NANOSECONDS_IN_SECOND;

                                for (int i = 0; i < 8; i++)
                                {
                                    tx_data[i] = (time.nanosec_to_master >> (8 * i)) & 0xFF;
                                    // tx_data[i] = (dupa >> (8 * i)) & 0xFF;
                                }
                                CAN_SendMessage(CAN_ID_LAP_TIME, tx_data, 8);
                            }

                            time_reset_button_press_counter++;

                            time.sec_sum += time.sec_counter;
                            time.min_sum += time.min_counter;

                            if (time.sec_sum >= 60)
                            {
                                time.min_sum += time.sec_sum / 60;
                                time.sec_sum = time.sec_sum % 60;
                            }

                            time.sec_counter = 0;
                            time.min_counter = 0;

                            params.lap_number++;
                            flags.lap_send_flag = 1;

                            break;

                        default:
                            break;
                    }
                }
                else if (state == GPIO_PIN_SET && buttons[i].button_state == BUTTON_STATE_PRESSED)
                {
                    buttons[i].button_state = BUTTON_STATE_RELEASED;

                    // if (__HAL_TIM_GET_COUNTER(&htim6))
                    //     ;

                    // button_states.all_button_states &= ~(1 << i);
                    // tx_data[0] = (uint8_t) button_states.all_button_states & 0xFF;
                    // tx_data[1] = (uint8_t)(button_states.all_button_states >> 8) & 0xFF;
                }
            }
        }
    }
    if (htim == &htim7)
    {
        flags.can_rx_tick_flag = 1;
    }
}
