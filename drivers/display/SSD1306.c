/*
 * SSD1306.c
 *
 *  Created on: Sep 17, 2022
 *      Author: sandeep
 */

#include "SSD1306.h"

#if USE_SSD1306

void SSD1306_init(void){

}

void SSD1306_flush(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p){

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

#endif


