/*
 * ST7735.c
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

#include "ST7735.h"

//#if USE_ST7535

static void ST7735_Select();
static void ST7735_Unselect();
static void ST7735_Reset();
static void ST7735_WriteCommand(uint8_t cmd);
static void ST7735_WriteData(uint8_t* buff, size_t buff_size);
static void ST7735_ExecuteCommandList(const uint8_t *addr);

#define DELAY 0x80

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_CMD_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_CMD_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_CMD_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_CMD_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_CMD_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_CMD_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_CMD_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_CMD_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_CMD_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_CMD_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_CMD_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_CMD_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_CMD_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_CMD_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
    (ST7735_PARAM_MADCTL_MX | ST7735_PARAM_MADCTL_RGB),        //     row addr/col addr, bottom to top refresh
    ST7735_CMD_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
    ST7735_PARAM_COLMOD_IFPF_16B_PER_PIXEL },                 //     16-bit color


  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CMD_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_CMD_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_CMD_INVON, 0 },        //  3: Invert colors


  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_CMD_GAMCTRP1, 16      , //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_CMD_GAMCTRN1, 16      , //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_CMD_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_CMD_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

static void ST7735_Select() {
    //HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
}

void ST7735_Unselect() {
    //HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
}

static void ST7735_Reset() {
    //HAL_GPIO_WritePin(RES2_GPIO_Port, RES2_Pin, GPIO_PIN_RESET);
    //HAL_Delay(5);
    //HAL_GPIO_WritePin(RES2_GPIO_Port, RES2_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteCommand(uint8_t cmd) {
    //HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET);
    //HAL_SPI_Transmit(&st7735_spi, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {
    //HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
    //HAL_SPI_Transmit(&st7735_spi, buff, buff_size, HAL_MAX_DELAY);
}

static void ST7735_ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            ///osDelay(ms); //TODO:
        }
    }
}


void ST7735_init(void){
    ST7735_Select();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}

void ST7735_flush(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p){

    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to be implemented by you.*/
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //put_px(x, y, *color_p);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

//#endif


