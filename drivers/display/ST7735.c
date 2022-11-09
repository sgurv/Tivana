/*
 * ST7735.c
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware Includes */
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysexc.h" //System Exception
#include "driverlib/systick.h" //SysTick
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h" //NIVC
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h" // QSPI Microwire etc

//-----------------
#include "drivers/rtos_hw_drivers.h"

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
    while(true == MAP_SSIBusy(SSI2_BASE));// test
    //MAP_SSIAdvFrameHoldEnable(SSI2_BASE);
    MAP_GPIOPinWrite(DISPLAY_CS_PORT, DISPLAY_CS_PIN, 0);
}

void ST7735_Unselect() {
    //MAP_SSIAdvFrameHoldDisable(SSI2_BASE);
    while(true == MAP_SSIBusy(SSI2_BASE)); // wait for data send to complete
    MAP_GPIOPinWrite(DISPLAY_CS_PORT, DISPLAY_CS_PIN, DISPLAY_CS_PIN);
}

static void ST7735_Reset() {
    //HAL_GPIO_WritePin(RES2_GPIO_Port, RES2_Pin, GPIO_PIN_RESET);
    //HAL_Delay(5);
    //HAL_GPIO_WritePin(RES2_GPIO_Port, RES2_Pin, GPIO_PIN_SET);
    MAP_GPIOPinWrite(DISPLAY_RST_PORT, DISPLAY_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS( 50 )); //5
    MAP_GPIOPinWrite(DISPLAY_RST_PORT, DISPLAY_RST_PIN, DISPLAY_RST_PIN);
}

static void ST7735_WriteCommand(uint8_t cmd) {
    //HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET);
    //HAL_SPI_Transmit(&st7735_spi, &cmd, sizeof(cmd), HAL_MAX_DELAY);
    while(true == MAP_SSIBusy(SSI2_BASE));// test
    MAP_GPIOPinWrite(DISPLAY_RS_PORT, DISPLAY_RS_PIN, 0);
    //MAP_SSIAdvDataPutFrameEnd(SSI2_BASE,cmd);
    //while(true == MAP_SSIBusy(SSI2_BASE));
    MAP_SSIDataPut(SSI2_BASE,cmd);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {
    //uint8_t i;

    //HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
    //HAL_SPI_Transmit(&st7735_spi, buff, buff_size, HAL_MAX_DELAY);
    while(true == MAP_SSIBusy(SSI2_BASE)); //test
    MAP_GPIOPinWrite(DISPLAY_RS_PORT, DISPLAY_RS_PIN, DISPLAY_RS_PIN);
    //i = 0;
    while(buff_size--){
        //if(buff_size == 0){
            //MAP_SSIAdvDataPutFrameEnd(SSI2_BASE,*buff);
            //LEDWrite(LED_D1,0); //TEST
        //} else {
            //while(true == MAP_SSIBusy(SSI2_BASE));
            MAP_SSIDataPut(SSI2_BASE,*buff++);
        //}

    }
    //vTaskDelay( pdMS_TO_TICKS( 1 )); // test
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
            //LEDWrite(LED_D2,0); // test
        }
        //LEDWrite(LED_D1,0); // test
        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            vTaskDelay( pdMS_TO_TICKS( ms ));
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
#if 0
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //put_px(x, y, *color_p);
            ST7735_DrawPixel(x,y,color_p->full); // TODO:
            color_p++;
        }
    }
#else

    ST7735_Select();

    // column address set
    ST7735_WriteCommand(ST7735_CMD_CASET);
    uint8_t data[] = { 0x00, area->x1 + ST7735_XSTART, 0x00, area->x2 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_CMD_RASET);
    data[1] = area->y1 + ST7735_YSTART;
    data[3] = area->y2 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_CMD_RAMWR);

    while(true == MAP_SSIBusy(SSI2_BASE));
    MAP_GPIOPinWrite(DISPLAY_RS_PORT, DISPLAY_RS_PIN, DISPLAY_RS_PIN);

    uint8_t data2[2];

    uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);

    while(size--){

        data2[0] = color_p->full >> 8;
        data2[1] = color_p->full & 0xFF;

        MAP_SSIDataPut(SSI2_BASE,data2[0]);
        MAP_SSIDataPut(SSI2_BASE,data2[1]);
        color_p++;
    }

    ST7735_Unselect();

#endif

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    //HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
    while(true == MAP_SSIBusy(SSI2_BASE));
    MAP_GPIOPinWrite(DISPLAY_RS_PORT, DISPLAY_RS_PIN, DISPLAY_RS_PIN);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            //HAL_SPI_Transmit(&st7735_spi, data, sizeof(data), HAL_MAX_DELAY);
            //HAL_SPI_Transmit_DMA(&st7735_spi, data, sizeof(data));
            //while(true == MAP_SSIBusy(SSI2_BASE));
            MAP_SSIDataPut(SSI2_BASE,data[0]);
            MAP_SSIDataPut(SSI2_BASE,data[1]);

        }
    }

    ST7735_Unselect();
}


void ST7735_DrawPixel(uint32_t x, uint32_t y, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow((uint8_t)x, (uint8_t)y, (uint8_t)x+1, (uint8_t)y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    // column address set
    ST7735_WriteCommand(ST7735_CMD_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_CMD_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_CMD_RAMWR);
}
//#endif


