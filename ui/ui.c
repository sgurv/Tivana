// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: tiva

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
void splash_Animation(lv_obj_t * TargetObject, int delay);
void ui_event_Screen1(lv_event_t * e);
lv_obj_t * ui_Screen1;
void ui_event_Screen2(lv_event_t * e);
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Screen2_Arc2;
lv_obj_t * ui_Screen2_Switch1;
void ui_event_Screen3(lv_event_t * e);
lv_obj_t * ui_Screen3;
void ui_event_Screen3_TextArea1(lv_event_t * e);
lv_obj_t * ui_Screen3_TextArea1;
void ui_event_Screen4(lv_event_t * e);
lv_obj_t * ui_Screen4;
lv_obj_t * ui_Screen4_Colorwheel1;
void ui_event_Screen5(lv_event_t * e);
lv_obj_t * ui_Screen5;
lv_obj_t * ui_Screen5_Spinner1;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void splash_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

}

///////////////////// FUNCTIONS ////////////////////
void ui_event_Screen1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen3_TextArea1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen5, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_img_src(ui_Screen1, &ui_img_782468719, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Screen1, ui_event_Screen1, LV_EVENT_ALL, NULL);

}
void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen2_Arc2 = lv_arc_create(ui_Screen2);
    lv_obj_set_width(ui_Screen2_Arc2, 56);
    lv_obj_set_height(ui_Screen2_Arc2, 59);
    lv_obj_set_x(ui_Screen2_Arc2, -39);
    lv_obj_set_y(ui_Screen2_Arc2, 1);
    lv_obj_set_align(ui_Screen2_Arc2, LV_ALIGN_CENTER);

    ui_Screen2_Switch1 = lv_switch_create(ui_Screen2);
    lv_obj_set_width(ui_Screen2_Switch1, 50);
    lv_obj_set_height(ui_Screen2_Switch1, 25);
    lv_obj_set_x(ui_Screen2_Switch1, 34);
    lv_obj_set_y(ui_Screen2_Switch1, 18);
    lv_obj_set_align(ui_Screen2_Switch1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Screen2, ui_event_Screen2, LV_EVENT_ALL, NULL);

}
void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen3_TextArea1 = lv_textarea_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_TextArea1, 150);
    lv_obj_set_height(ui_Screen3_TextArea1, 70);
    lv_obj_set_align(ui_Screen3_TextArea1, LV_ALIGN_CENTER);
    lv_textarea_set_text(ui_Screen3_TextArea1, "Hello World!\nLorem Ipsum dolor sit amet.\nalpha tango charlie");
    lv_obj_set_style_text_color(ui_Screen3_TextArea1, lv_color_hex(0xE1E1E1), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui_Screen3_TextArea1, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui_Screen3_TextArea1, lv_color_hex(0x44162E), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_Screen3_TextArea1, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(ui_Screen3_TextArea1, lv_color_hex(0xE48D1A), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui_Screen3_TextArea1, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui_Screen3_TextArea1, 1, LV_PART_MAIN | LV_STATE_FOCUSED);

    lv_obj_add_event_cb(ui_Screen3_TextArea1, ui_event_Screen3_TextArea1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen3, ui_event_Screen3, LV_EVENT_ALL, NULL);

}
void ui_Screen4_screen_init(void)
{
    ui_Screen4 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen4_Colorwheel1 = lv_colorwheel_create(ui_Screen4, true);
    lv_obj_set_width(ui_Screen4_Colorwheel1, 63);
    lv_obj_set_height(ui_Screen4_Colorwheel1, 63);
    lv_obj_set_x(ui_Screen4_Colorwheel1, -32);
    lv_obj_set_y(ui_Screen4_Colorwheel1, 0);
    lv_obj_set_align(ui_Screen4_Colorwheel1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Screen4, ui_event_Screen4, LV_EVENT_ALL, NULL);

}
void ui_Screen5_screen_init(void)
{
    ui_Screen5 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen5, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen5_Spinner1 = lv_spinner_create(ui_Screen5, 1000, 90);
    lv_obj_set_width(ui_Screen5_Spinner1, 71);
    lv_obj_set_height(ui_Screen5_Spinner1, 72);
    lv_obj_set_align(ui_Screen5_Spinner1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Screen5_Spinner1, LV_OBJ_FLAG_CLICKABLE);      /// Flags

    lv_obj_add_event_cb(ui_Screen5, ui_event_Screen5, LV_EVENT_ALL, NULL);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui_Screen4_screen_init();
    ui_Screen5_screen_init();
    lv_disp_load_scr(ui_Screen1);
}
