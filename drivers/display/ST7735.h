/*
 * ST7735.h
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

#ifndef ST7735_H_
#define ST7735_H_

#ifdef __cplusplus
extern "C" {
#endif

//#if USE_ST7735

#include "middlewares/lvgl/lvgl.h"

//System Function Commands
#define ST7735_CMD_NOP          0x00 /*No Operation*/
#define ST7735_CMD_SWRESET      0x01 /*Software reset*/
#define ST7735_CMD_RDDID        0x04 /*Read Display ID, read-byte: Dummy read,ID1,ID2,ID3*/
#define ST7735_CMD_RDDST        0x09 /*Read Display Status*/
#define ST7735_CMD_RDDPM        0x0A /*Read Display Power*/
#define ST7735_CMD_RDDMADCTL    0x0B /*Read Display*/
#define ST7735_CMD_RDDCOLMOD    0x0C /*Read Display Pixel*/
#define ST7735_CMD_RDDIM        0x0D /*Read Display Image*/
#define ST7735_CMD_RDDSM        0x0E /*Read Display Signal*/
#define ST7735_CMD_SLPIN        0x10 /*Sleep in & booster off*/
#define ST7735_CMD_SLPOUT       0x11 /*Sleep out & booster on*/
#define ST7735_CMD_PTLON        0x12 /*Partial mode on*/
#define ST7735_CMD_NORON        0x13 /*Partial off(Normal)*/
#define ST7735_CMD_INVOFF       0x20 /*Display inversion off*/
#define ST7735_CMD_INVON        0x21 /*Display inversion on*/
#define ST7735_CMD_GAMSET       0x26 /*Gamma curve select*/
#define ST7735_CMD_DISPOFF      0x28 /*Display Off*/
#define ST7735_CMD_DISPON       0x29 /*Display On*/
#define ST7735_CMD_CASET        0x2A /*Column address set*/
#define ST7735_CMD_RASET        0x2B /*Row address set*/
#define ST7735_CMD_RAMWR        0x2C /*Memory write*/
#define ST7735_CMD_RAMRD        0x2E /*Memory read*/
#define ST7735_CMD_PTLAR        0x30 /*Partial start/end address*/
#define ST7735_CMD_TEOFF        0x34 /*Tearing effect line off*/
#define ST7735_CMD_TEON         0x35 /*Tearing effect mode set & on*/
#define ST7735_CMD_MADCTL       0x36 /*Memory data access control*/
#define ST7735_CMD_IDMOFF       0x38 /*Idle mode off*/
#define ST7735_CMD_IDMON        0x39 /*Idle mode on*/
#define ST7735_CMD_COLMOD       0x3A /*Interface pixel format*/
#define ST7735_CMD_RDID1        0xDA /*Read ID1*/
#define ST7735_CMD_RDID2        0xDB /*Read ID2*/
#define ST7735_CMD_RDID3        0xDC /*Read ID3*/

#define ST7735_PARAM_MADCTL_MY      0x80 /*Row address order*/
#define ST7735_PARAM_MADCTL_MX      0x40 /*Column Address Order*/
#define ST7735_PARAM_MADCTL_MV      0x20 /*Row/Column exchange*/
#define ST7735_PARAM_MADCTL_ML      0x10 /*0=V refresh Top to Bottom, 1=V refresh Bottom to Top*/
#define ST7735_PARAM_MADCTL_RGB     0x00 /*0=RGB, 1=BGR*/
#define ST7735_PARAM_MADCTL_BGR     0x08 /*0=RGB, 1=BGR*/
#define ST7735_PARAM_MADCTL_MH      0x04 /*0= H refresh left to right, 1= H refresh right to left*/

#define ST7735_PARAM_COLMOD_IFPF_12B_PER_PIXEL   0x03
#define ST7735_PARAM_COLMOD_IFPF_16B_PER_PIXEL   0x05
#define ST7735_PARAM_COLMOD_IFPF_18B_PER_PIXEL   0x06

#define ST7735_PARAM_GAMSET_GC0     0x01    /*GS=1 => G2.2, GS=0 G1.0*/
#define ST7735_PARAM_GAMSET_GC1     0x02    /*GS=1 => G1.8, GS=0 G2.5*/
#define ST7735_PARAM_GAMSET_GC2     0x04    /*GS=1 => G2.5, GS=0 G2.2*/
#define ST7735_PARAM_GAMSET_GC3     0x08    /*GS=1 => G1.0, GS=0 G1.8*/

//Panel Function Commands
#define ST7735_CMD_FRMCTR1          0xB1
#define ST7735_CMD_FRMCTR2          0xB2
#define ST7735_CMD_FRMCTR3          0xB3
#define ST7735_CMD_INVCTR           0xB4
#define ST7735_CMD_DISSET5          0xB6
#define ST7735_CMD_PWCTR1           0xC0
#define ST7735_CMD_PWCTR2           0xC1
#define ST7735_CMD_PWCTR3           0xC2
#define ST7735_CMD_PWCTR4           0xC3
#define ST7735_CMD_PWCTR5           0xC4
#define ST7735_CMD_VMCTR1           0xC5
#define ST7735_CMD_VMOFCTR          0xC7
#define ST7735_CMD_WRID2            0xD1
#define ST7735_CMD_WRID3            0xD2
#define ST7735_CMD_PWCTR6           0xFC
#define ST7735_CMD_NVCTR1           0xD9
#define ST7735_CMD_NVCTR2           0xDE
#define ST7735_CMD_NVCTR3           0xDF
#define ST7735_CMD_GAMCTRP1         0xE0
#define ST7735_CMD_GAMCTRN1         0xE1
#define ST7735_CMD_EXTCTRL          0xF0
#define ST7735_CMD_VCOM4L           0xFF

//////////////////////////////////
#define ST7735_RED      0xF800
#define ST7735_GREEN    0x07E0
#define ST7735_BLUE     0x001F

//////////////////////////////
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
///////////////////////////////
//#define ST7735_XSTART 26
//#define ST7735_YSTART 1
//#define ST7735_WIDTH  80
//#define ST7735_HEIGHT 160
//////////////////////////////

#define ST7735_HOR_RES  ST7735_WIDTH
#define ST7735_VER_RES  ST7735_HEIGHT

//#define ST7735_FRAMEBUFFER_SIZE (ST7735_WIDTH * ST7735_HEIGHT)
//
//void ST7735_Init(SPI_HandleTypeDef * hspi);
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
//void ST7735_Unselect();
//void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
//void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
//void ST7735_FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
//void ST7735_FillScreen(uint16_t color);
//void ST7735_FillScreenFast(uint16_t color);
//void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

/*
 * for lvgl
 * */

void ST7735_init(void);
void ST7735_flush(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

//
void ST7735_DrawPixel(uint32_t x, uint32_t y, uint16_t color);

//#endif /* USE_ST7735 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ST7735_H_ */
