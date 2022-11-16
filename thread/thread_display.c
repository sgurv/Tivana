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

/* Hardware Includes */
#include "driverlib/gpio.h"

/* lvgl */
#include "middlewares/lvgl/lvgl.h"

/*display includes*/

#include "drivers/rtos_hw_drivers.h"

//#include "drivers/display/SSD1306.h"
#include "drivers/display/ST7735.h"

#include "thread_display.h"

/* include UI */
#include "ui/ui.h"

/* The system clock frequency. */
extern uint32_t g_ui32SysClock;

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * 20];
//static lv_color_t buf_2[MY_DISP_HOR_RES * 10];

static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/

//LED global
//lv_obj_t * led1;
//lv_obj_t * led2;
//
//static void btn_event_cb(lv_event_t * e);
//static void btn_event_cb2(lv_event_t * e);

static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);

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

    //register input device
    /*Register a button input device*/
    lv_indev_t * indev_button;

    lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    indev_button = lv_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[2] = {
        {40, 20},   /*Button 0 -> x:10; y:10*/
        {120, 20},  /*Button 1 -> x:40; y:100*/
    };

    lv_indev_set_button_points(indev_button, btn_points);

    ui_init();

////    /**
////     * Create a button with a label and react on click event.
////     */
//    //Button 1
//    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
//    lv_obj_set_pos(btn, 5, 5);                            /*Set its position*/
//    lv_obj_set_size(btn, 70, 30);                          /*Set its size*/
//    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/
//
//    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
//    lv_label_set_text(label, "Button A");                     /*Set the labels text*/
//    lv_obj_center(label);
//
//    //Button2
//    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
//    lv_obj_set_pos(btn2, 85, 5);                            /*Set its position*/
//    lv_obj_set_size(btn2, 70, 30);                          /*Set its size*/
//    lv_obj_add_event_cb(btn2, btn_event_cb2, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/
//
//    lv_obj_t * label2 = lv_label_create(btn2);          /*Add a label to the button*/
//    lv_label_set_text(label2, "Button B");                     /*Set the labels text*/
//    lv_obj_center(label2);
//
//    /*Create a LED and switch it OFF*/
//    led1  = lv_led_create(lv_scr_act());
//    lv_obj_set_pos(led1, 35, 50);
//    //lv_obj_align(led1, LV_ALIGN_CENTER, -80, 0);
//    lv_led_set_color(led1, lv_palette_main(LV_PALETTE_LIME));
//    lv_led_off(led1);
//
//    led2  = lv_led_create(lv_scr_act());
//    lv_obj_set_pos(led2, 115, 50);
//    //lv_obj_align(led2, LV_ALIGN_CENTER, -80, 0);
//    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_RED));
//    lv_led_off(led2);


    for(;;){
//        //lv_tick_inc(10); // called in vApplicationTickHook
//        //TODO: use mutex
        lv_timer_handler();
//        //lv_timer_handler_run_in_period(10); //supper loop
//        //TODO: release mutex
        vTaskDelay( pdMS_TO_TICKS( 10 ));

    }
}

//static void btn_event_cb(lv_event_t * e)
//{
//    lv_event_code_t code = lv_event_get_code(e);
//    lv_obj_t * btn = lv_event_get_target(e);
//    if(code == LV_EVENT_CLICKED) {
////        static uint8_t cnt = 0;
////        cnt++;
////
////        /*Get the first child of the button which is the label and change its text*/
////        lv_obj_t * label = lv_obj_get_child(btn, 0);
////        lv_label_set_text_fmt(label, "Button: %d", cnt);
//        lv_led_toggle(led1);
//    }
//}
//
//static void btn_event_cb2(lv_event_t * e)
//{
//    lv_event_code_t code = lv_event_get_code(e);
//    lv_obj_t * btn = lv_event_get_target(e);
//    if(code == LV_EVENT_CLICKED) {
//
//        lv_led_toggle(led2);
//    }
//}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(ButtonsPoll(NULL,NULL) & USR_SW1) {
            return 0;
        }
        if(ButtonsPoll(NULL,NULL) & USR_SW2) {
            return 1;
        }
    }

    /*No button pressed*/
    return -1;
}


