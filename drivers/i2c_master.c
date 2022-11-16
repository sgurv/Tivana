/*
 * i2c_master.c
 *
 *  Created on: Nov 10, 2022
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
#include "driverlib/i2c.h"


#include "drivers/rtos_hw_drivers.h"

#include "drivers/i2c_master.h"


void I2C_Master_WriteByte(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress, uint8_t data)
{
    //  Set I2C address of MPU, writing mode
    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, false);
    //  Push register address into a sending buffer
    MAP_I2CMasterDataPut(ui32Base, regAddress);
    //  Send start sequence and address, followed by register address
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);
    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));

    //  Send register data to write and stop sequence
    MAP_I2CMasterDataPut(ui32Base, data);
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_FINISH);
    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));
}


uint8_t I2C_Master_WriteBytes(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress,
                           uint16_t length, uint8_t *data)
{
    uint16_t i;
    //  Set I2C address of MPU, writing mode
    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, false);
    //  Push register address into a sending buffer
    MAP_I2CMasterDataPut(ui32Base, regAddress);
    //  Send start sequence and address, followed by register address. Use burst
    //  mode as we're sending more than 1 byte
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);
    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));

    //  Loop through all data that needs to be sent. Send everything but last
    //  byte
    for (i = 0; i < (length-1); i++)
    {
        MAP_I2CMasterDataPut(ui32Base, data[i]);
        MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_CONT);
        vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
        while(MAP_I2CMasterBusy(ui32Base));
    }

    //  Send last byte together with stop sequence
    MAP_I2CMasterDataPut(ui32Base, data[length-1]);
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_FINISH);
    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));

    return 0;
}

uint8_t I2C_Master_ReadByte(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress)
{
    uint32_t data;

    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, false); //write register address
    //  Push register address into a sending buffer
    MAP_I2CMasterDataPut(ui32Base, regAddress);
    //  Send start sequence and address, followed by register address
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);
    //vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));

    //  Perform s single receive from I2C bus
    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, true);
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_RECEIVE);
    //vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));
    //  Read a byte from receiving buffer
    data = MAP_I2CMasterDataGet(ui32Base);

    //  We're dealing with 8-bit data so return only lower 8 bits
    return (data & 0xFF);
}

uint8_t I2C_Master_ReadBytes(uint32_t ui32Base, uint8_t I2Caddress, uint8_t regAddress,
                          uint16_t length, uint8_t* data)
{
    uint16_t i;


    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, false);
    //  Push register address into a sending buffer
    MAP_I2CMasterDataPut(ui32Base, regAddress);
    //  Send start sequence and address, followed by register address. Use burst
    //  mode as we're reading more than 1 byte
    MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);
    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));

    //  Change address to reading mode
    MAP_I2CMasterSlaveAddrSet(ui32Base, I2Caddress, true);

    //  Check how many bytes we need to receive
    if (length == 1)
        MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_RECEIVE);
    else
    {
        MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_START);
        vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
        while(MAP_I2CMasterBusy(ui32Base));
        data[0] = (uint8_t)(MAP_I2CMasterDataGet(ui32Base) & 0xFF);

        for (i = 1; i < (length-1); i++)
        {
            MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
            while(MAP_I2CMasterBusy(ui32Base));
            data[i] = (uint8_t)(MAP_I2CMasterDataGet(ui32Base) & 0xFF);
        }
        MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    }

    vTaskDelay(pdMS_TO_TICKS( 4 )); //HAL_DelayUS(4);
    while(MAP_I2CMasterBusy(ui32Base));
    data[length-1] = (uint8_t)(MAP_I2CMasterDataGet(ui32Base) & 0xFF);

    return 0;
}
