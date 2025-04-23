#include "button.h"

extern TIM_HandleTypeDef htim6;
extern uint8_t tx_data;
extern volatile uint16_t sec_counter;
extern volatile uint16_t min_counter;
volatile uint16_t sec_sum = 0;  
volatile uint16_t min_sum = 0;
volatile uint8_t lap_send_flag = 0;
volatile uint8_t lap_number = 0;

extern volatile uint8_t send_vehicle_speed_flag;
//extern volatile uint8_t rx_data;



Button_t buttons[NUM_BUTTONS] = {
    {BUTTON_FULL_GAS_GPIO_Port, BUTTON_FULL_GAS_Pin, BUTTON_STATE_RELEASED, 0,
     CAN_ID_IS_GAS_BUTTON_PRESSED},
    {BUTTON_HALF_GAS_GPIO_Port, BUTTON_HALF_GAS_Pin, BUTTON_STATE_RELEASED, 0,
     CAN_ID_IS_HALF_SPEED_BUTTON_PRESSED},
    {BUTTON_MODE1_GPIO_Port, BUTTON_MODE1_Pin, BUTTON_STATE_RELEASED, 0, 0},
    {BUTTON_MODE2_GPIO_Port, BUTTON_MODE2_Pin, BUTTON_STATE_RELEASED, 0, 0},
    {BUTTON_HORN_GPIO_Port, BUTTON_HORN_Pin, BUTTON_STATE_RELEASED, 0, 0},
    {BUTTON_TIME_GPIO_Port, BUTTON_TIME_Pin, BUTTON_STATE_RELEASED, 0,
     CAN_ID_IS_TIME_RESET_BUTTON_PRESSED},
    {BUTTON_SC_CLOSE_GPIO_Port, BUTTON_SC_CLOSE_Pin, BUTTON_STATE_RELEASED, 0,
     CAN_ID_IS_SC_RELAY_CLOSED},
    {BUTTON_EMERGENCY_GPIO_Port, BUTTON_EMERGENCY_Pin, BUTTON_STATE_RELEASED, 0,
     CAN_ID_IS_EMERGENCY_BUTTON_PRESSED},
    {BUTTON_FUELCELL_RACE_MODE_GPIO_Port, BUTTON_FUELCELL_RACE_MODE_Pin,
     BUTTON_STATE_RELEASED, 0, 0},
    {BUTTON_FUELCELL_PREPARE_TO_RACE_MODE_GPIO_Port,
     BUTTON_FUELCELL_PREPARE_TO_RACE_MODE_Pin, BUTTON_STATE_RELEASED, 0, 0},
    {BUTTON_FUELCELL_OFF_MODE_GPIO_Port, BUTTON_FUELCELL_OFF_MODE_Pin,
     BUTTON_STATE_RELEASED, 0, 0},
};


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (buttons[i].pin == GPIO_Pin && buttons[i].debouncing_flag == 0) {
      buttons[i].debouncing_flag = 1;
      __HAL_TIM_SET_COUNTER(&htim6, 0);
      HAL_TIM_Base_Start_IT(&htim6);
      break;
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim6) {
    HAL_TIM_Base_Stop_IT(htim);

    for (int i = 0; i < NUM_BUTTONS; i++) {
      if (buttons[i].debouncing_flag) {

        GPIO_PinState state = HAL_GPIO_ReadPin(buttons[i].port, buttons[i].pin);
        buttons[i].debouncing_flag = 0;

        if (state == GPIO_PIN_RESET &&
            buttons[i].button_state == BUTTON_STATE_RELEASED) {

          buttons[i].button_state = BUTTON_STATE_PRESSED;
          tx_data = BUTTON_STATE_PRESSED;

          switch (buttons[i].pin) {
          case BUTTON_FULL_GAS_Pin:
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          case BUTTON_HALF_GAS_Pin:
            HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          case BUTTON_TIME_Pin:
            HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);

             // dodajemy aktualny czas do sumy
            sec_sum += sec_counter;
            min_sum += min_counter;

            // normalizacja czasu łącznego
            if (sec_sum >= 60) {
              min_sum += sec_sum / 60;
              sec_sum = sec_sum % 60;
            }

            sec_counter = 0;
            min_counter = 0;

            lap_number++;
            lap_send_flag = 1;

            break;
          case BUTTON_SC_CLOSE_Pin:
            HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          default:
            break;
          }

        } else if (state == GPIO_PIN_SET &&
                   buttons[i].button_state == BUTTON_STATE_PRESSED) {

          buttons[i].button_state = BUTTON_STATE_RELEASED;
          tx_data = BUTTON_STATE_RELEASED;

          switch (buttons[i].pin) {
          case BUTTON_FULL_GAS_Pin:
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          case BUTTON_HALF_GAS_Pin:
            HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          case BUTTON_TIME_Pin:
            HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
            break;
          case BUTTON_SC_CLOSE_Pin:
            HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
            CAN_SendMessage(buttons[i].can_id, &tx_data, 1);
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
