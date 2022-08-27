

/**
 * main.c
 */
#define TARGET_IS_TM4C129_RA2 /*TM4C129 devices, silicon revision A2. Part revision 3(marking)*/
#define RTOS_FREERTOS   1

//For CLI
/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE   60

/* Dimensions the buffer into which string outputs can be placed. */
#define cmdMAX_OUTPUT_SIZE  1024
//END for CLI

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Kernel includes. */
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
uint32_t g_ui32SysClock;

/* Set up the hardware ready to run this demo. */
static void prvSetupHardware( void );

/* This function sets up UART0 to be used for a console to display information
 * as the example is running. */
static void prvConfigureUART(void);

//Task
static void prvHelloTask( void *pvParameters );

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

void vRegisterCLICommands( void );

//-----------------------------------------END_CLI

int main(void)
{
    /* Prepare the hardware to run this demo. */
    prvSetupHardware();


    /* Create tasks
     *
     * The xTaskCreate parameters in order are:
     *  - The function that implements the task.
     *  - The text name Hello task - for debug only as it is
     *    not used by the kernel.
     *  - The size of the stack to allocate to the task.
     *  - No parameter passed to the task
     *  - The priority assigned to the task.
     *  - The task handle is NULL */
    xTaskCreate( prvHelloTask,
                 "app_main",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 1,
                 NULL );


    /* Register commands with the FreeRTOS+CLI command interpreter. */
    vRegisterCLICommands();

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
    for( ;; );
}

/*-----------------------------------------------------------*/
static void prvHelloTask( void *pvParameters )
{
    BaseType_t xMoreDataToFollow; //For LCI
    static signed char cInputString[ cmdMAX_INPUT_SIZE ], cOutputString[ cmdMAX_OUTPUT_SIZE ]; //, cLocalBuffer[ cmdSOCKET_INPUT_BUFFER_SIZE ]; // for CLI

    LEDWrite(LED_D1,1);
    LEDWrite(LED_D2,1);
    LEDWrite(LED_D3,1);
    LEDWrite(LED_D4,1);

    for (;;)
    {

        ///////-- Blinky
        /* Create a 1 second delay */
//        vTaskDelay( pdMS_TO_TICKS( 1000 ));
//        LEDWrite(LED_D2,1);
//        vTaskDelay( pdMS_TO_TICKS( 1000 ));
//        LEDWrite(LED_D2,0);
        //////--- END Blinky

        ////-- Button
//        if(ButtonsPoll(NULL,NULL) & USR_SW1){
//            LEDWrite(LED_D1,1);
//        } else {
//            LEDWrite(LED_D1,0);
//        }
        ////-- END Button

        //// -- UART Test
        /* Print the Hello world! message. */
        //UARTprintf("Hello world!\n");
        //vTaskDelay( pdMS_TO_TICKS( 1000 ));
        /////--- END UART Test

        ///-- FreeRTOS-PLUS-CLI test

        UARTgets((char *)cInputString,cmdMAX_INPUT_SIZE); // This is blocking, use peak for non blocking

        do
        {
            /* Pass the string to FreeRTOS+CLI. */
            xMoreDataToFollow = FreeRTOS_CLIProcessCommand( (char *)cInputString,(char *) cOutputString, cmdMAX_OUTPUT_SIZE );

            /* Send the output generated by the command's implementation. */
            //sendto( xSocket, cOutputString,  strlen( cOutputString ), 0, ( SOCKADDR * ) &xClient, xClientAddressLength );
            UARTprintf((const char *)cOutputString);

        } while( xMoreDataToFollow != pdFALSE ); /* Until the command does not generate any more output. */

        ///-- END FreeRTOS-PLUS-CLI test


    }
}

/*-------------------*/
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

/*---------------------*/
/*-------------------------------------------------------------------*/

static void prvConfigureUART(void)
{
    /* Enable GPIO port A which is used for UART0 pins.
     * TODO: change this to whichever GPIO port you are using. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Configure the pin muxing for UART0 functions on port A0 and A1.
     * This step is not necessary if your part does not support pin muxing.
     * TODO: change this to select the port/pin you are using. */
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);

    /* Enable UART0 so that we can configure the clock. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Use the internal 16MHz oscillator as the UART clock source. */
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    /* Select the alternate (UART) function for these pins.
     * TODO: change this to select the port/pin you are using. */
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART for console I/O. */
    UARTStdioConfig(0, 115200, 16000000);
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Run from the PLL at configCPU_CLOCK_HZ MHz. */
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
            SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
            SYSCTL_CFG_VCO_240), configCPU_CLOCK_HZ);

    /* Configure device pins. */
    PinoutSet(false, false); // use EMAC

    /*Buttons*/
    ButtonsInit();

    /* Configure UART0 to send messages to terminal. (ICD) */
    prvConfigureUART();

    /* UART 2*/
    /* Enable and configure the peripherals used by the uart. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
//    MAP_GPIOPinConfigure(GPIO_PD4_U2RX);
//    MAP_GPIOPinConfigure(GPIO_PD5_U2TX);
//    MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Wait for the UART6 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART2))
//    {
//    }

//    MAP_UARTConfigSetExpClk(UART2_BASE, MAP_SysCtlClockGet(), 9600,
//    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//    UART_CONFIG_PAR_NONE));

    /* UART 6*/
    /* Enable and configure the peripherals used by the uart. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
//    MAP_GPIOPinConfigure(GPIO_PP0_U6RX);
//    MAP_GPIOPinConfigure(GPIO_PP1_U6TX);
//    MAP_GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Wait for the UART6 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART6))
//    {
//    }

//    MAP_UARTConfigSetExpClk(UART6_BASE, MAP_SysCtlClockGet(), 9600,
//    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//    UART_CONFIG_PAR_NONE));

    /* UART 7*/
    /* Enable and configure the peripherals used by the uart. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
//    MAP_GPIOPinConfigure(GPIO_PC4_U7RX);
//    MAP_GPIOPinConfigure(GPIO_PC5_U7TX);
//    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Wait for the UART7 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART7))
//    {
//    }

//    MAP_UARTConfigSetExpClk(UART7_BASE, MAP_SysCtlClockGet(), 9600,
//    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//    UART_CONFIG_PAR_NONE));

    /* CAN 0 */
    // Enable the CAN0 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    // Wait for the CAN0 module to be ready.

//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0))
//    {
//    }

    // Reset the state of all the message objects and the state of the CAN
    // module to a known state.
//    MAP_CANInit(CAN0_BASE);

    /* CAN 1 */
    // Enable the CAN1 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);

    // Wait for the CAN0 module to be ready.

//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CAN1))
//    {
//    }

    // Reset the state of all the message objects and the state of the CAN
    // module to a known state.
//    MAP_CANInit(CAN1_BASE);

    /* ADC 0 */
    // Enable the ADC0 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Wait for the ADC0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
//    {
//    }

    // Enable the first sample sequencer to capture the value of channel 0 when
    // the processor trigger occurs.
//    MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
//    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
//    MAP_ADCSequenceEnable(ADC0_BASE, 0);

    /* ADC 1 */
    // Enable the ADC1 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // Wait for the ADC0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
//    {
//    }

    // Enable the first sample sequencer to capture the value of channel 0 when
    // the processor trigger occurs.
//    MAP_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//    MAP_ADCSequenceStepConfigure(ADC1_BASE, 0, 0,
//    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
//    MAP_ADCSequenceEnable(ADC1_BASE, 0);


    /* I2C 0 */
    /* Enable the peripheral */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    /* Configure the appropriate pins to be I2C instead of GPIO. */
//    MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
//    MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
//    MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
//    MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Wait for the I2C0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0))
//    {
//    }

    // Initialize Master and Slave
//    MAP_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), true);

    // Specify slave address
//    MAP_I2CMasterSlaveAddrSet(I2C0_BASE, 0x3B, false);

    /* I2C 2*/
    /* Enable the peripheral */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    /* Configure the appropriate pins to be I2C instead of GPIO. */
//    MAP_GPIOPinConfigure(GPIO_PN5_I2C2SCL);
//    MAP_GPIOPinConfigure(GPIO_PN4_I2C2SDA);
//    MAP_GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
//    MAP_GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    // Wait for the I2C2 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2))
//    {
//    }

    // Initialize Master and Slave
//    MAP_I2CMasterInitExpClk(I2C2_BASE, MAP_SysCtlClockGet(), true);

    // Specify slave address
//    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, 0x3B, false);

    /* SSI2 SPI*/
    /* Enable the peripheral */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    // Wait for the SSI2 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2))
//    {
//    }

    // Configure the SSI.
//    MAP_SSIConfigSetExpClk(SSI2_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//    SSI_MODE_MASTER, 2000000, 8);

//    MAP_SSIAdvModeSet(SSI2_BASE,SSI_ADV_MODE_LEGACY); //SSI_ADV_MODE_LEGACY default

    // Enable the SSI module.
//    MAP_SSIEnable(SSI2_BASE);

    /* SSI3 SPI*/
    /* Enable the peripheral */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

    // Wait for the SSI3 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI3))
//    {
//    }

    // Configure the SSI.
//    MAP_SSIConfigSetExpClk(SSI3_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//    SSI_MODE_MASTER, 2000000, 8);

//    MAP_SSIAdvModeSet(SSI3_BASE,SSI_ADV_MODE_LEGACY); //SSI_ADV_MODE_LEGACY default

    // Enable the SSI module.
//    MAP_SSIEnable(SSI3_BASE);

    /* QEI 0 */
    // Enable the QEI0 peripheral
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);

    // Wait for the QEI0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_QEI0))
//    {
//    }

    // Configure the quadrature encoder to capture edges on both signals and
    // maintain an absolute position by resetting on index pulses. Using a
    // 1000 line encoder at four edges per line, there are 4000 pulses per
    // revolution; therefore set the maximum position to 3999 as the count
    // is zero based.
//    MAP_QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_RESET_IDX |
//    QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 3999);

    // Enable the quadrature encoder.
//    MAP_QEIEnable(QEI0_BASE);

    /* EEPROM */
    // Enable the EEPROM module.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    // Wait for the EEPROM module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
    {
    }

//    // Wait for the EEPROM Initialization to complete
//    ui32EEPROMInit = MAP_EEPROMInit();
//
//    // Check if the EEPROM Initialization returned an error
//    // and inform the application
//
//    if(ui32EEPROMInit != EEPROM_INIT_OK)
//    {
//        while(1)
//        {
//        }
//    }


}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    IntMasterDisable();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    IntMasterDisable();
    for( ;; );
}
/*-----------------------------------------------------------*/

void *malloc( size_t xSize )
{
    /* There should not be a heap defined, so trap any attempts to call
    malloc. */
    IntMasterDisable();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
        configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
        added here, but the tick hook is called from an interrupt context, so
        code must not attempt to block, and only the interrupt safe FreeRTOS API
        functions can be used (those that end in FromISR()). */

    /* Only the full demo uses the tick hook so there is no code is
        executed here. */
}

