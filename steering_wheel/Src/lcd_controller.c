/*********************
 *      INCLUDES
 *********************/
#include "lcd_controller.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES 320
#define MY_DISP_VER_RES 240
//#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
///*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv;
extern lv_meter_indicator_t *indicator1;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void) {
  /*-------------------------
   * Initialize your display
   * -----------------------*/
  disp_init();

  /*-----------------------------
   * Create a buffer for drawing
   *----------------------------*/

  /* Example for 2) */
  static lv_disp_draw_buf_t draw_buf_dsc_2;
  static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10]; /*A buffer for 10 rows*/
  static lv_color_t
      buf_2_2[MY_DISP_HOR_RES * 10]; /*An other buffer for 10 rows*/
  lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2,
                        MY_DISP_HOR_RES * 10); /*Initialize the display buffer*/

  /*-----------------------------------
   * Register the display in LVGL
   *----------------------------------*/

  lv_disp_drv_init(&disp_drv); /*Basic initialization*/

  /*Set up the functions to access to your display*/

  /*Set the resolution of the display*/
  disp_drv.hor_res = MY_DISP_HOR_RES;
  disp_drv.ver_res = MY_DISP_VER_RES;

  /*Used to copy the buffer's content to the display*/
  disp_drv.flush_cb = disp_flush;

  /*Set a display buffer*/
  disp_drv.draw_buf = &draw_buf_dsc_2;

  /*Required for Example 3)*/
  // disp_drv.full_refresh = 1;

  /* Fill a memory array with a color if you have GPU.
   * Note that, in lv_conf.h you can enable GPUs that has built-in support in
   * LVGL. But if you have a different GPU you can use with this callback.*/
  // disp_drv.gpu_fill_cb = gpu_fill;

  /*Finally register the driver*/
  lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void) { ILI9341_Init(); }

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called
 * by LVGL
 */
void disp_enable_update(void) { disp_flush_enabled = true; }

/* Disable updating the screen (the flushing process) when disp_flush() is
 * called by LVGL
 */
void disp_disable_update(void) { disp_flush_enabled = false; }

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the
 *background but 'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p) {
  if (disp_flush_enabled) {
    // Set the drawing region
    ILI9341_SetWindow(area->x1, area->y1, area->x2, area->y2);

    int height = area->y2 - area->y1 + 1;
    int width = area->x2 - area->x1 + 1;

    ILI9341_DrawBitmapDMA(width, height, (uint8_t *)color_p);
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  lv_disp_flush_ready(&disp_drv);
  //	ILI9341_EndOfDrawBitmap();
}

void disp_set_time(uint16_t min_counter, uint16_t sec_counter, uint16_t min_sum,
                   uint16_t sec_sum, uint8_t time_send_flag) {
  static uint8_t old_sec_counter;
  char buffer_time[50];
  char buffer_total_time[50];

  if (time_send_flag) {
    time_send_flag = 0;

    uint16_t total_s = sec_sum + sec_counter;
    uint16_t total_m = min_sum + min_counter;

    if (total_s >= 60) {
      total_m += total_s / 60;
      total_s = total_s % 60;
    }
    if (sec_counter != old_sec_counter) {
      old_sec_counter = sec_counter;
      sprintf(buffer_time, "%lu:%02lu", min_counter, sec_counter);
      sprintf(buffer_total_time, "%lu:%02lu", total_m, total_s);
      lv_textarea_set_text(objects.lap_time_area, buffer_time);
      lv_textarea_set_text(objects.total_time_area, buffer_total_time);
    }
  }
}

void disp_set_lap_number(uint8_t lap_number, uint8_t lap_send_flag) {
  char buffer_lap_num[20];

  if (lap_send_flag) {
    lap_send_flag = 0;
    sprintf(buffer_lap_num, "%u", lap_number);
    lv_textarea_set_text(objects.lap_number_area, buffer_lap_num);
  }
}

void disp_set_vehicle_speed(uint8_t rx_data, uint8_t send_vehicle_speed_flag) {
  char buffer_speed[20];
  if (send_vehicle_speed_flag) {
    send_vehicle_speed_flag = 0;
    sprintf(buffer_speed, "%u", rx_data);
    lv_meter_set_indicator_value(objects.speed_meter, indicator1, rx_data);
    lv_textarea_set_text(objects.speed_area, buffer_speed);
  }
}
