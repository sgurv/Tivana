// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: tiva

#ifndef _TIVA_UI_H
#define _TIVA_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "middlewares/lvgl/lvgl.h"

void splash_Animation(lv_obj_t * TargetObject, int delay);
void ui_event_Screen1(lv_event_t * e);
extern lv_obj_t * ui_Screen1;
void ui_event_Screen2(lv_event_t * e);
extern lv_obj_t * ui_Screen2;
extern lv_obj_t * ui_Screen2_Arc2;
extern lv_obj_t * ui_Screen2_Switch1;
void ui_event_Screen3(lv_event_t * e);
extern lv_obj_t * ui_Screen3;
void ui_event_Screen3_TextArea1(lv_event_t * e);
extern lv_obj_t * ui_Screen3_TextArea1;
void ui_event_Screen4(lv_event_t * e);
extern lv_obj_t * ui_Screen4;
extern lv_obj_t * ui_Screen4_Colorwheel1;
void ui_event_Screen5(lv_event_t * e);
extern lv_obj_t * ui_Screen5;
extern lv_obj_t * ui_Screen5_Spinner1;


LV_IMG_DECLARE(ui_img_782468719);    // assets\car-yellow.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
