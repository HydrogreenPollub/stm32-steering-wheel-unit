#include "can_controller.h"

FDCAN_TxHeaderTypeDef tx_header;
FDCAN_RxHeaderTypeDef rx_header;

uint8_t rx_data[8];
uint8_t tx_data[8];

ring_buffer_t speed_ring_buffer;
ring_buffer_t sc_voltage_ring_buffer;
uint8_t can_speed_ring_buffer[CAN_FRAMES_POOL_SIZE];
uint8_t can_sc_voltage_ring_buffer[CAN_FRAMES_POOL_SIZE];

void CAN_Init(FDCAN_HandleTypeDef* hfdcan)
{
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_Start(hfdcan);
    ring_buffer_init(&speed_ring_buffer, can_speed_ring_buffer, CAN_FRAMES_POOL_SIZE);
    ring_buffer_init(&sc_voltage_ring_buffer, can_sc_voltage_ring_buffer, CAN_FRAMES_POOL_SIZE);
}

void CAN_FilterConfig(FDCAN_HandleTypeDef* hfdcan)
{
    FDCAN_FilterTypeDef filterConfig;

    filterConfig.IdType = FDCAN_STANDARD_ID;
    filterConfig.FilterIndex = 0;
    filterConfig.FilterType = FDCAN_FILTER_MASK;
    filterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filterConfig.FilterID1 = CAN_ID_SENSOR_SPEED;
    filterConfig.FilterID2 = STANDARD_ID_MASK;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);

    filterConfig.FilterIndex = 1;
    filterConfig.FilterID1 = CAN_ID_MASTER_STATE;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);

    filterConfig.FilterIndex = 2;
    filterConfig.FilterID1 = CAN_ID_SC_VOLTAGE;
    HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);
}


void CAN_SendMessage(uint16_t std_id, uint8_t* data, uint8_t len)
{
    tx_header.Identifier = std_id;
    tx_header.DataLength = len;

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}

// void CAN_ReceiveMessage()
// {
//     if (flags.can_rx_tick_flag)
//     {
//         if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
//         {
//             if (rx_header.IdType == FDCAN_STANDARD_ID && rx_header.DataLength == FDCAN_DLC_BYTES_4)
//             {
//                 switch (rx_header.Identifier)
//                 {
//                     case CAN_ID_SENSOR_SPEED:
//                         flags.send_vehicle_speed_flag = 1;
//                         memcpy(&params.vehicle_speed, rx_data, sizeof(float));
//                         uint8_t speed = (uint8_t)params.vehicle_speed;
//                         disp_set_vehicle_speed(speed, flags.send_vehicle_speed_flag);
//                         break;
//
//                     case CAN_ID_SC_VOLTAGE:
//                         flags.sc_voltage_send_flag = 1;
//                         memcpy(&params.sc_voltage, rx_data, sizeof(float));
//                         uint8_t sc_voltage = (uint8_t)params.sc_voltage;
//                         disp_set_sc_voltage(sc_voltage, flags.sc_voltage_send_flag);
//                         break;
//
//                   default:
//                         break;
//                 }
//             }
//         }
//         flags.can_rx_tick_flag = 0;
//     }
// }

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
        {
            if (rx_header.IdType == FDCAN_STANDARD_ID && rx_header.DataLength == FDCAN_DLC_BYTES_4)
            {
                switch (rx_header.Identifier)
                {
                    case CAN_ID_SENSOR_SPEED:
                        flags.send_vehicle_speed_flag = 1;
                        memcpy(&params.vehicle_speed, rx_data, sizeof(float));
                        uint8_t speed = (uint8_t)params.vehicle_speed;
                        ring_buffer_enqueue(&speed_ring_buffer, speed);
                        break;

                    case CAN_ID_SC_VOLTAGE:
                        flags.sc_voltage_send_flag = 1;
                        memcpy(&params.sc_voltage, rx_data, sizeof(float));
                        uint8_t sc_voltage = (uint8_t)params.sc_voltage;
                        ring_buffer_enqueue(&sc_voltage_ring_buffer, sc_voltage);
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

void CAN_ProcessData(void)
{
    uint8_t speed, sc_voltage;
    if (flags.update_can_rx_params_flag)
    {
        if (ring_buffer_dequeue(&speed_ring_buffer, &speed))
        {
            disp_set_vehicle_speed(speed);
        }
        if (ring_buffer_dequeue(&sc_voltage_ring_buffer, &sc_voltage))
        {
            disp_set_sc_voltage(sc_voltage);
        }
        flags.update_can_rx_params_flag = 0;
    }
}

