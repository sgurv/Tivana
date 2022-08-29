/*
 * cli_commands.c
 *
 *  Created on: Aug 29, 2022
 *      Author: sandeep
 */


/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* Hardware includes. */
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
#include "driverlib/watchdog.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/comp.h" // Comparator
#include "driverlib/adc.h"
#include "driverlib/aes.h"
#include "driverlib/can.h"
#include "driverlib/crc.h"
#include "driverlib/des.h" //Data encryption and decryption
#include "driverlib/eeprom.h"
#include "driverlib/emac.h" //Ethernet
#include "driverlib/epi.h" // external peripheral interface
#include "driverlib/flash.h"
#include "driverlib/fpu.h" //Floating Point Unit
//#include "driverlib/hibernate.h" //RTC
#include "driverlib/i2c.h"
#include "driverlib/lcd.h"
#include "driverlib/mpu.h" // Memory protection unit
#include "driverlib/onewire.h" // 1-Wire master
#include "driverlib/pwm.h"
#include "driverlib/qei.h" // Quadrature Encoder
#include "driverlib/shamd5.h" // SHA MD5
#include "driverlib/ssi.h" // QSPI Microwire etc
//#include "driverlib/sw_crc.h" //Soft CRC

#include "drivers/rtos_hw_drivers.h"

#include "utils/uartstdio.h"

/* The system clock frequency. */
extern uint32_t g_ui32SysClock;


//--------------------------------------------CLI
/*
 * Defines a command for some cli -LED On
 */
static BaseType_t prvLedOnCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -LED Off
 */
static BaseType_t prvLedOffCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Three
 */
static BaseType_t prvThreeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Four
 */
static BaseType_t prvFourCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Five
 */
static BaseType_t prvFiveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Six
 */
static BaseType_t prvSixCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Seven
 */
static BaseType_t prvSevenCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command for some cli -Eight
 */
static BaseType_t prvEightCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );


/* Structure that defines the "One" command line command. */
static const CLI_Command_Definition_t xLedOnCommand =
{
    "led-on", /* The command string to type. */
    "\r\nled-on:\r\n Turns On LED \r\n",
    prvLedOnCommand, /* The function to run. */
    1 /* 1 parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Two" command line command. */
static const CLI_Command_Definition_t xLedOffCommand =
{
    "led-off", /* The command string to type. */
    "\r\nled-off:\r\n Turns Off LED \r\n",
    prvLedOffCommand, /* The function to run. */
    1 /* 1 parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Three" command line command. */
static const CLI_Command_Definition_t xThreeCommand =
{
    "three", /* The command string to type. */
    "\r\nThree:\r\n three command echos something \r\n",
    prvThreeCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Four" command line command. */
static const CLI_Command_Definition_t xFourCommand =
{
    "four", /* The command string to type. */
    "\r\nFour:\r\n Four command echos something \r\n",
    prvFourCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Five" command line command. */
static const CLI_Command_Definition_t xFiveCommand =
{
    "five", /* The command string to type. */
    "\r\nFive:\r\n five command echos something \r\n",
    prvFiveCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Six" command line command. */
static const CLI_Command_Definition_t xSixCommand =
{
    "six", /* The command string to type. */
    "\r\nSix:\r\n six command echos something \r\n",
    prvSixCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Seven" command line command. */
static const CLI_Command_Definition_t xSevenCommand =
{
    "seven", /* The command string to type. */
    "\r\nSeven:\r\n seven command echos something \r\n",
    prvSevenCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};

/* Structure that defines the "Eight" command line command. */
static const CLI_Command_Definition_t xEightCommand =
{
    "eight", /* The command string to type. */
    "\r\nEight:\r\n eight command echos something \r\n",
    prvEightCommand, /* The function to run. */
    0 /* No parameters are expected. */ /* -1 = any number of parameter*/
};


void vRegisterCLICommands( void )
{
    /* Register all the command line commands defined immediately above. */
    FreeRTOS_CLIRegisterCommand( &xLedOnCommand );
    FreeRTOS_CLIRegisterCommand( &xLedOffCommand );
    FreeRTOS_CLIRegisterCommand( &xThreeCommand );
    FreeRTOS_CLIRegisterCommand( &xFourCommand );
    FreeRTOS_CLIRegisterCommand( &xFiveCommand );
    FreeRTOS_CLIRegisterCommand( &xSixCommand );
    FreeRTOS_CLIRegisterCommand( &xSevenCommand );
    FreeRTOS_CLIRegisterCommand( &xEightCommand );
}


static BaseType_t prvLedOnCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter
                    (
                        pcCommandString,        /* The command string itself. */
                        1,                      /* Return the first parameter. */
                        &lParameterStringLength /* Store the parameter string length. */
                    );

    /* Sanity check something was returned. */
    configASSERT( pcParameter );

    if( strncmp( pcParameter, "1", strlen( "1" ) ) == 0 )
    {
        /* Turn On LED*/
        LEDWrite(LED_D1,1);
        sprintf( pcWriteBuffer, "Turning ON LED 1\r\n" );
    } else if( strncmp( pcParameter, "2", strlen( "2" ) ) == 0 )
    {
        /* Turn On LED*/
        LEDWrite(LED_D2,1);
        sprintf( pcWriteBuffer, "Turning ON LED 2\r\n" );
    } else if( strncmp( pcParameter, "3", strlen( "3" ) ) == 0 )
    {
        /* Turn On LED*/
        LEDWrite(LED_D3,1);
        sprintf( pcWriteBuffer, "Turning ON LED 3\r\n" );
    } else if( strncmp( pcParameter, "4", strlen( "4" ) ) == 0 )
    {
        /* Turn On LED*/
        LEDWrite(LED_D4,1);
        sprintf( pcWriteBuffer, "Turning ON LED 4\r\n" );
    }

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvLedOffCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter
                    (
                        pcCommandString,        /* The command string itself. */
                        1,                      /* Return the first parameter. */
                        &lParameterStringLength /* Store the parameter string length. */
                    );

    /* Sanity check something was returned. */
    configASSERT( pcParameter );

    if( strncmp( pcParameter, "1", strlen( "1" ) ) == 0 )
    {
        /* Turn Off LED*/
        LEDWrite(LED_D1,0);
        sprintf( pcWriteBuffer, "Turning OFF LED 1\r\n" );
    } else if( strncmp( pcParameter, "2", strlen( "2" ) ) == 0 )
    {
        /* Turn Off LED*/
        LEDWrite(LED_D2,0);
        sprintf( pcWriteBuffer, "Turning OFF LED 2\r\n" );
    } else if( strncmp( pcParameter, "3", strlen( "3" ) ) == 0 )
    {
        /* Turn Off LED*/
        LEDWrite(LED_D3,0);
        sprintf( pcWriteBuffer, "Turning OFF LED 3\r\n" );
    } else if( strncmp( pcParameter, "4", strlen( "4" ) ) == 0 )
    {
        /* Turn Off LED*/
        LEDWrite(LED_D4,0);
        sprintf( pcWriteBuffer, "Turning OFF LED 4\r\n" );
    }

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvThreeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvFourCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvFiveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvSixCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvSevenCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

static BaseType_t prvEightCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ){

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

