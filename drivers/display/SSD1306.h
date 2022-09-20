/*
 * SSD1306.h
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#ifdef __cplusplus
extern "C" {
#endif

#if USE_SSD1306

#include "middlewares/lvgl/lvgl.h"

//Fundamental Commands
#define SSD1306_CMD_SET_CONTRAST_CONTROL    0x81
#define SSD1306_CMD_SET_DISABLE_ENTIRE_DISPLAY_ON   0xA4
#define SSD1306_CMD_SET_ENABLE_ENTIRE_DISPLAY_ON    0xA5
#define SSD1306_CMD_SET_NORMAL_DISPLAY      0xA6
#define SSD1306_CMD_SET_INVERSE_DISPLAY     0xA7
#define SSD1306_CMD_SET_DISPLAY_OFF         0xAE
#define SSD1306_CMD_SET_DISPLAY_ON          0xAF

//Scrolling Commands
#define SSD1306_CMD_RIGHT_HORIZONTAL_SCROLL             0x26
#define SSD1306_CMD_LEFT_HORIZONTAL_SCROLL              0x27
#define SSD1306_CMD_VERTICAL_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_CMD_VERTICAL_LEFT_HORIZONTAL_SCROLL     0x2A
#define SSD1306_CMD_DEACTIVATE_SCROLL                   0x2E
#define SSD1306_CMD_ACTIVATE_SCROLL                     0x2F
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA            0xA3

//Addressing Setting Commands
#define SSD1306_CMD_SET_LOWER_COLUMN_START_ADDRESS_FOR_PAGE_ADDRESSING  0x00
#define SSD1306_CMD_SET_HIGHER_COLUMN_START_ADDRESS_FOR_PAGE_ADDRESSING 0x10
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE          0x20
#define SSD1306_CMD_PARAM_HORIZONTAL_ADDRESSING_MODE    0x00
#define SSD1306_CMD_PARAM_VERTICAL_ADDRESSING_MODE      0x01
#define SSD1306_CMD_PARAM_PAGE_ADDRESSING_MODE          0x02
#define SSD1306_CMD_SET_COLUMN_ADDRESS                  0x21
#define SSD1306_CMD_SET_PAGE_ADDRESS                    0x22
#define SSD1306_CMD_SET_PAGE_START_ADDRESS_FOR_PAGE_ADDRESSING_MODE     0xB0
#define SSD1306_CMD_PARAM_PAGE_0                        0x00
#define SSD1306_CMD_PARAM_PAGE_1                        0x01
#define SSD1306_CMD_PARAM_PAGE_2                        0x02
#define SSD1306_CMD_PARAM_PAGE_3                        0x03
#define SSD1306_CMD_PARAM_PAGE_4                        0x04
#define SSD1306_CMD_PARAM_PAGE_5                        0x05
#define SSD1306_CMD_PARAM_PAGE_6                        0x06
#define SSD1306_CMD_PARAM_PAGE_7                        0x07

//Hardware Configuration Commands
#define SSD1306_CMD_SET_DISPLAY_START_LINE              0x40
#define SSD1306_CMD_SET_SEGMENT_REMAP_SEG0_COL_ADR_0    0xA0
#define SSD1306_CMD_SET_SEGMENT_REMAP_SEG0_COL_ADR_127  0xA1
#define SSD1306_CMD_SET_MULTIPLEX_RATIO                 0xA8
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL    0xC0
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPED   0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET                  0xD3
#define SSD1306_CMD_SET_COM_PINS_HARDWARE_CONFIG        0xDA

//Timing & Driving Scheme Setting Commands
#define SSD1306_CMD_SET_DISPLAY_CLK_DIV_OSC_FREQ        0xD5
#define SSD1306_CMD_SET_PRE_CHARGE_PERIOD               0xD9
#define SSD1306_CMD_SET_VCOM_DESELECT_LEVEL             0xDB
#define SSD1306_CMD_NOP                                 0xE3

//Charge pump settings command
#define SSD1306_CMD_CHARGE_PUMP_SETTING                 0x8D
#define SSD1306_CMD_PARAM_CHARGE_PUMP_DISABLE           0x10
#define SSD1306_CMD_PARAM_CHARGE_PUMP_ENABLE            0x14

//void SSD1306_Init(SPI_HandleTypeDef * hspi);
//void SSD1306_WriteFrameBuffer(uint8_t *fb);
//void SSD1306_RefreshFrameBuffer(void);
//void SSD1306_putchar(uint8_t c);
//void SSD1306_puts(const char *str);
//void SSD1306_puts_centerAlign(const char *str);
//void SSD1306_SetFont(uint8_t font_num);
//void SSD1306_GotoXY(uint8_t x, uint8_t y);
//void SSD1306_Clear(void);

/*
 * for lvgl
 *
 * */

void SSD1306_init(void);
void SSD1306_flush(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

#endif /* USE_SSD1306 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SSD1306_H_ */
