
#ifndef LCDCONTROLLER_H_
#define LCDCONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include <stdbool.h>
#include "main.h"
#include "ili9341.h"
#include "ui.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void lv_port_disp_init(void);

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void);

void disp_set_time(uint16_t min_counter, uint16_t sec_counter, uint16_t min_sum, uint16_t sec_sum, uint8_t time_send_flag);

void disp_set_lap_number(uint8_t lap_number, uint8_t lap_send_flag);

void disp_set_vehicle_speed(uint8_t rx_data, uint8_t send_vehicle_speed_flag);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

