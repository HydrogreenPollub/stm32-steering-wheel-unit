#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include "can.h"
#include "can_ids.h"
#include "lcd_controller.h"
#include "tim.h"
#include "steering_wheel.h"

#define STANDARD_CAN_ID_BIT_SHIFT   0x05

extern CAN_TxHeaderTypeDef tx_header;
extern CAN_RxHeaderTypeDef rx_header;

extern uint8_t rx_data;
extern uint8_t tx_data[8];

extern uint32_t tx_mailbox;

void CAN_Init(CAN_HandleTypeDef *hcan);
void CAN_FilterConfig(CAN_HandleTypeDef *hcan);
void CAN_SendMessage(uint16_t std_id, uint8_t *data, uint8_t len);
void CAN_ReceiveMessage(uint8_t *data);


#endif