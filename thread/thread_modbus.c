/*
 * thread_modbus.c
 *
 *  Created on: Sep 2, 2022
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

/* ----------------------- Platform includes --------------------------------*/
//#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
//#include "mb.h"
//#include "mbport.h"

/* The system clock frequency. */
extern uint32_t g_ui32SysClock;

#include "thread_modbus.h"

void prvModbusTask( void *pvParameters ){

    for(;;){
        vTaskDelay(1000);
    }
}
