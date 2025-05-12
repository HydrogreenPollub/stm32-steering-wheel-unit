#include "button.h"

extern TIM_HandleTypeDef htim6;

buttons_states_t button_states;

button_t buttons[NUM_BUTTONS] = {
    {
        BUTTON_FULL_GAS_GPIO_Port,
        BUTTON_FULL_GAS_Pin,
        BUTTON_STATE_RELEASED,
        0,
        CAN_ID_IS_GAS_BUTTON_PRESSED,
    },
    {
        BUTTON_HALF_GAS_GPIO_Port,
        BUTTON_HALF_GAS_Pin,
        BUTTON_STATE_RELEASED,
        0,
        CAN_ID_IS_HALF_SPEED_BUTTON_PRESSED,
    },
    {
        BUTTON_MODE1_GPIO_Port,
        BUTTON_MODE1_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
    },
    {
        BUTTON_MODE2_GPIO_Port,
        BUTTON_MODE2_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
    },
    {
        BUTTON_HORN_GPIO_Port,
        BUTTON_HORN_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
    },
    {
        BUTTON_TIME_RESET_GPIO_Port,
        BUTTON_TIME_RESET_Pin,
        BUTTON_STATE_RELEASED,
        0,
        CAN_ID_IS_TIME_RESET_BUTTON_PRESSED,
    },
    {
        BUTTON_SC_CLOSE_GPIO_Port,
        BUTTON_SC_CLOSE_Pin,
        BUTTON_STATE_RELEASED,
        0,
        CAN_ID_IS_SC_RELAY_CLOSED,
    },
    {
        BUTTON_EMERGENCY_GPIO_Port,
        BUTTON_EMERGENCY_Pin,
        BUTTON_STATE_RELEASED,
        0,
        CAN_ID_IS_EMERGENCY_BUTTON_PRESSED,
    },
    {
        BUTTON_FC_RACE_MODE_GPIO_Port,
        BUTTON_FC_RACE_MODE_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
    },
    {
        BUTTON_FC_PREPARE_TO_RACE_MODE_GPIO_Port,
        BUTTON_FC_PREPARE_TO_RACE_MODE_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
    },
    {
        BUTTON_FC_OFF_MODE_GPIO_Port,
        BUTTON_FC_OFF_MODE_Pin,
        BUTTON_STATE_RELEASED,
        0,
        0,
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
        HAL_TIM_Base_Stop_IT(htim);

        for (int i = 0; i < NUM_BUTTONS; i++)
        {
            if (buttons[i].debouncing_flag)
            {
                GPIO_PinState state = HAL_GPIO_ReadPin(buttons[i].port, buttons[i].pin);
                buttons[i].debouncing_flag = 0;

                if (state == GPIO_PIN_RESET && buttons[i].button_state == BUTTON_STATE_RELEASED)
                {
                    buttons[i].button_state = BUTTON_STATE_PRESSED;
                    // tx_data = BUTTON_STATE_PRESSED;
                    button_states.all_button_states |= (1 << i);
                    tx_data[0] = (uint8_t) button_states.all_button_states & 0xFF;
                    tx_data[1] = (uint8_t)(button_states.all_button_states >> 8) & 0xFF;

                    switch (buttons[i].pin)
                    {
                        case BUTTON_FULL_GAS_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        case BUTTON_HALF_GAS_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        case BUTTON_TIME_RESET_Pin:
                            if (time_reset_button_press_counter > 0)
                            {
                                tx_data[2] = params.lap_number;
                                tx_data[3] = time.min_counter;
                                tx_data[4] = time.sec_counter;
                                CAN_SendMessage(buttons[i].can_id, tx_data, 5);
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
                        case BUTTON_SC_CLOSE_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        default:
                            break;
                    }
                }
                else if (state == GPIO_PIN_SET && buttons[i].button_state == BUTTON_STATE_PRESSED)
                {
                    buttons[i].button_state = BUTTON_STATE_RELEASED;
                    button_states.all_button_states &= ~(1 << i);
                    tx_data[0] = (uint8_t) button_states.all_button_states & 0xFF;
                    tx_data[1] = (uint8_t)(button_states.all_button_states >> 8) & 0xFF;

                    switch (buttons[i].pin)
                    {
                        case BUTTON_FULL_GAS_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        case BUTTON_HALF_GAS_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        case BUTTON_TIME_RESET_Pin:
                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        case BUTTON_SC_CLOSE_Pin:

                            CAN_SendMessage(buttons[i].can_id, tx_data, 2);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    // else if (htim == &htim7)
    // {
    //   HAL_TIM_Base_Stop_IT(&htim7);
    //   send_vehicle_speed_flag = 1;

    // }
}
