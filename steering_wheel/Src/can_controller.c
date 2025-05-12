#include "can_controller.h"

FDCAN_TxHeaderTypeDef tx_header;
FDCAN_RxHeaderTypeDef rx_header;

uint8_t rx_data[8];
uint8_t tx_data[8];

void CAN_Init(FDCAN_HandleTypeDef* hfdcan)
{
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;
    HAL_FDCAN_Start(hfdcan);
    // HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CAN_FilterConfig(FDCAN_HandleTypeDef* hfdcan)
{
    FDCAN_FilterTypeDef filterConfig;

    filterConfig.IdType = FDCAN_STANDARD_ID;
    filterConfig.FilterIndex = 0;
    filterConfig.FilterType = FDCAN_FILTER_MASK;
    filterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filterConfig.FilterID1 = CAN_ID_IS_EMERGENCY;
    filterConfig.FilterID2 = STANDARD_ID_MASK;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);

    filterConfig.FilterIndex = 1;
    filterConfig.FilterID1 = CAN_ID_VEHICLE_SPEED;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);

    filterConfig.FilterIndex = 2;
    filterConfig.FilterID1 = CAN_ID_FC_VOLTAGE;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);
}

// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {

//     if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, &rx_data) ==
//         HAL_OK) {
//       if (rx_header.IDE == CAN_ID_STD) {
//         if (rx_header.StdId == CAN_ID_IS_EMERGENCY) {
//           send_emergency_msg_flag = 1;
//         } else if (rx_header.StdId == CAN_ID_VEHICLE_SPEED) {
//           speed = rx_data;
//           send_vehicle_speed_flag = 1;
//           // disp_set_vehicle_speed(speed, send_vehicle_speed_flag);
//           // printf("data = %u\n", speed);

//           // __HAL_TIM_SET_COUNTER(&htim7, 0);
//           // HAL_TIM_Base_Start_IT(&htim7);
//         }
//       }
//     }
// }

void CAN_SendMessage(uint16_t std_id, uint8_t* data, uint8_t len)
{
    tx_header.Identifier = std_id;
    tx_header.DataLength = len;

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}

void CAN_ReceiveMessage(uint8_t* data)
{
    if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_header, data) == HAL_OK)
    {
        if (tx_header.IdType == FDCAN_STANDARD_ID)
        {
            if (rx_header.Identifier == CAN_ID_IS_EMERGENCY)
            {
                // send_emergency_msg_flag = 1;
            }
            else if (rx_header.Identifier == CAN_ID_VEHICLE_SPEED)
            {
                flags.send_vehicle_speed_flag = 1;
                params.vehicle_speed = data[0];
                disp_set_vehicle_speed(params.vehicle_speed, flags.send_vehicle_speed_flag);
            }
            else if (rx_header.Identifier == CAN_ID_FC_VOLTAGE)
            {
                flags.sc_voltage_send_flag = 1;
                params.sc_voltage = data[0];
                disp_set_sc_voltage(params.sc_voltage, flags.sc_voltage_send_flag);
            }
        }
    }
}
