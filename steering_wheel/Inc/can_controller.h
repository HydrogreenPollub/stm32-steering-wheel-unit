#ifndef CAN_CONTROLLER_H
#define CAN_CONTROLLER_H

#include "fdcan.h"
#include "can_ids.h"
#include "lcd_controller.h"
#include "tim.h"
#include "steering_wheel.h"

#define STANDARD_CAN_ID_BIT_SHIFT 0x05
#define STANDARD_ID_MASK          0x7FF

extern FDCAN_TxHeaderTypeDef tx_header;
extern FDCAN_RxHeaderTypeDef rx_header;

extern uint8_t rx_data[8];
extern uint8_t tx_data[8];

void CAN_Init(FDCAN_HandleTypeDef* hfdcan);
void CAN_FilterConfig(FDCAN_HandleTypeDef* hfdcan);
void CAN_SendMessage(uint16_t std_id, uint8_t* data, uint8_t len);
void CAN_ReceiveMessage();

#endif