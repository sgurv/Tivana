/*
 * thread_display.c
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* lvgl */
#include "middlewares/lvgl/lvgl.h"

/*display includes*/

#include "drivers/rtos_hw_drivers.h"

//#include "drivers/display/SSD1306.h"
#include "drivers/display/ST7735.h"

#include "thread_display.h"

/* The system clock frequency. */
extern uint32_t g_ui32SysClock;

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
//static lv_color_t buf_2[MY_DISP_HOR_RES * 10];

static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/

static void btn_event_cb(lv_event_t * e);

void prvDisplayTask( void *pvParameters ){

    LEDWrite(LED_D1,LED_D1);
    LEDWrite(LED_D2,LED_D2);
    LEDWrite(LED_D3,LED_D3);
    LEDWrite(LED_D4,LED_D4);


    //vTaskDelay( pdMS_TO_TICKS( 200 ));
    ST7735_init();

    lv_init();


    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX*10);

    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
    disp_drv.flush_cb = ST7735_flush;        /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;                 /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = LV_VER_RES_MAX;                 /*Set the vertical resolution in pixels*/

    lv_disp_t * disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
//
//    //TODO: register input device
//
//    /**
//     * Create a button with a label and react on click event.
//     */

    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 5, 5);                            /*Set its position*/
    lv_obj_set_size(btn, 70, 30);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);

    for(;;){
//        //lv_tick_inc(10); // called in vApplicationTickHook
//        //TODO: use mutex
        lv_timer_handler();
//        //lv_timer_handler_run_in_period(10); //supper loop
//        //TODO: release mutex
        vTaskDelay( pdMS_TO_TICKS( 10 ));


//        vTaskDelay( pdMS_TO_TICKS( 200 )); //TEST
//        ST7735_FillRectangle(0,0,ST7735_WIDTH, ST7735_HEIGHT,ST7735_RED);//TEST
//        vTaskDelay( pdMS_TO_TICKS( 200 )); //TEST
//        ST7735_FillRectangle(0,0,ST7735_WIDTH, ST7735_HEIGHT,ST7735_BLUE);//TEST
//        vTaskDelay( pdMS_TO_TICKS( 200 )); //TEST
//        ST7735_FillRectangle(0,0,ST7735_WIDTH, ST7735_HEIGHT,ST7735_GREEN);//TEST
    }
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}


