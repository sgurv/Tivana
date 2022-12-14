

/**
 * main.c
 */
#define TARGET_IS_TM4C129_RA2 /*TM4C129 devices, silicon revision A2. Part revision 3(marking)*/
#define NO_SYS 0
#define RTOS_FREERTOS   1
#define USE_ST7735     (1)

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "FreeRTOS_IP.h"

/* */
#include "middlewares/lvgl/lvgl.h"

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
#include "drivers/i2c_master.h"

#include "drivers/sensors/APDS9960.h"

//Utilities
#include "utils/uartstdio.h"
#include "utils/cli_commands.h"

//Threads
#include "thread/thread_cli.h"
#include "thread/thread_modbus.h"
#include "thread/thread_display.h"

/* The system clock frequency. */
uint32_t g_ui32SysClock;

/* The MAC address array is not declared const as the MAC address will
normally be read from an EEPROM and not hard coded (in real deployed
applications).*/
//static uint8_t ucMACAddress[ 6 ] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */
//static const uint8_t ucIPAddress[ 4 ] = { 10, 10, 10, 200 };
//static const uint8_t ucNetMask[ 4 ] = { 255, 0, 0, 0 };
//static const uint8_t ucGatewayAddress[ 4 ] = { 10, 10, 10, 1 };

/* The following is the address of an OpenDNS server. */
//static const uint8_t ucDNSServerAddress[ 4 ] = { 208, 67, 222, 222 };

/* Set up the hardware. */
static void prvSetupHardware( void );

/* This function sets up UART0 to be used for a console to display information */
static void prvConfigureUART(void);

//Task
static void prvHelloTask( void *pvParameters );

int main(void)
{
//    BaseType_t res = pdFALSE;
    /* Prepare the hardware to run this demo. */
    prvSetupHardware();

    /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
        are created in the vApplicationIPNetworkEventHook() hook function
        below.  The hook function is called when the network connects. */
//    res = FreeRTOS_IPInit( ucIPAddress,
//                     ucNetMask,
//                     ucGatewayAddress,
//                     ucDNSServerAddress,
//                     ucMACAddress );
//
//    if(res == pdFALSE){ // Test
//        LEDWrite(LED_D1,1);
//    }

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
                 "thread_test",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 1,
                 NULL );

    xTaskCreate( prvCommandInterpreterTask,
                 "thread_cli",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 1,
                 NULL );

    xTaskCreate( prvModbusTask,
                 "thread_mb",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 1,
                 NULL );

    xTaskCreate( prvDisplayTask,
                 "thread_dis",
                 2048,
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

    //BaseType_t result;

    bool res;

    res = APDS9960Init(I2C0_BASE);


    for (;;)
    {

        if(res == true){
            LEDWrite(LED_D1,LED_D1);
        } else {
            LEDWrite(LED_D1,0);
        }


        ////-- Button
//        if(ButtonsPoll(NULL,NULL) & USR_SW1){
//            LEDWrite(LED_D1,LED_D1);
//        } else {
//            LEDWrite(LED_D1,0);
//        }
        ////-- END Button

        //// -- UART Test
        /* Print the Hello world! message. */
//        if(result == pdPASS){
//            UARTprintf("Yesss!\n");
//        } else {
//            UARTprintf("Noooh!\n");
//        }


        vTaskDelay( pdMS_TO_TICKS( 10 ));
        /////--- END UART Test

    }
}

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
    PinoutSet(false, false); //

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

    // Wait for the UART2 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART2))
//    {
//    }
//
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
//
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
//
//    MAP_UARTConfigSetExpClk(UART7_BASE, MAP_SysCtlClockGet(), 9600,
//    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//    UART_CONFIG_PAR_NONE));

    /* CAN 0 */
    // Enable the CAN0 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
//
//    // Wait for the CAN0 module to be ready.
//
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0))
//    {
//    }
//
//    // Reset the state of all the message objects and the state of the CAN
//    // module to a known state.
//    MAP_CANInit(CAN0_BASE);

    /* CAN 1 */
    // Enable the CAN1 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);
//
//    // Wait for the CAN0 module to be ready.
//
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CAN1))
//    {
//    }
//
//    // Reset the state of all the message objects and the state of the CAN
//    // module to a known state.
//    MAP_CANInit(CAN1_BASE);

    /* ADC 0 */
    // Enable the ADC0 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//
//    // Wait for the ADC0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
//    {
//    }
//
//    // Enable the first sample sequencer to capture the value of channel 0 when
//    // the processor trigger occurs.
//    MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
//    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
//    MAP_ADCSequenceEnable(ADC0_BASE, 0);

    /* ADC 1 */
    // Enable the ADC1 module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
//
//    // Wait for the ADC0 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
//    {
//    }
//
//    // Enable the first sample sequencer to capture the value of channel 0 when
//    // the processor trigger occurs.
//    MAP_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//    MAP_ADCSequenceStepConfigure(ADC1_BASE, 0, 0,
//    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
//    MAP_ADCSequenceEnable(ADC1_BASE, 0);


    /* I2C 0 */
    /* Enable the peripheral */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    /* Configure the appropriate pins to be I2C instead of GPIO. */
    MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Wait for the I2C0 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0))
    {
    }

    //Enable
    MAP_I2CMasterEnable(I2C0_BASE);

    // Initialize Master and Slave
    MAP_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), false); //slow mode

    // Specify slave address
    //MAP_I2CMasterSlaveAddrSet(I2C0_BASE, APDS9960_I2C_ADDR, false);

    /* I2C 2*/
    /* Enable the peripheral */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    /* Configure the appropriate pins to be I2C instead of GPIO. */
    MAP_GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    MAP_GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    MAP_GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
    MAP_GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    // Wait for the I2C2 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2))
    {
    }

    //Enable
    MAP_I2CMasterEnable(I2C2_BASE);

    // Initialize Master and Slave
    MAP_I2CMasterInitExpClk(I2C2_BASE, MAP_SysCtlClockGet(), true);

    /* SSI2 SPI*/
    /* Enable the peripheral */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    // Wait for the SSI2 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2))
    {
    }

    // Configure the pin muxing for SSI2 functions
    MAP_GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    MAP_GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    MAP_GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    MAP_GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);

    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.

    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 |
                      GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    // Configure the SSI.
    MAP_SSIConfigSetExpClk(SSI2_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
    SSI_MODE_MASTER, 2400000, 8);//200000

    MAP_SSIAdvModeSet(SSI2_BASE,SSI_ADV_MODE_WRITE); //SSI_ADV_MODE_LEGACY default

    // Enable the SSI module.
    MAP_SSIEnable(SSI2_BASE);

    /* SSI3 SPI*/
    /* Enable the peripheral */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
//
//    // Wait for the SSI3 module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI3))
//    {
//    }
//
//    // Configure the SSI.
//    MAP_SSIConfigSetExpClk(SSI3_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//    SSI_MODE_MASTER, 2000000, 8);
//
//    MAP_SSIAdvModeSet(SSI3_BASE,SSI_ADV_MODE_LEGACY); //SSI_ADV_MODE_LEGACY default
//
//    // Enable the SSI module.
//    MAP_SSIEnable(SSI3_BASE);

    /* QEI 0 */
    // Enable the QEI0 peripheral
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
//
//    // Wait for the QEI0 module to be ready.
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
//
//    // Enable the quadrature encoder.
//    MAP_QEIEnable(QEI0_BASE);

    /* EMAC */

    /* EEPROM */
    // Enable the EEPROM module.
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
//
//    // Wait for the EEPROM module to be ready.
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
//    {
//    }

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

    ///Other Pins
    //for ST7735
    //PN2 -- RS
    //PN3 -- RES
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);
    MAP_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3,
                        GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

    MAP_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);
    MAP_GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_0,
                        GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

    //apds9960 int input
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);


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

//void *malloc( size_t xSize )
//{
//    /* There should not be a heap defined, so trap any attempts to call
//    malloc. */
//    IntMasterDisable();
//    for( ;; );
//}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
        configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
        added here, but the tick hook is called from an interrupt context, so
        code must not attempt to block, and only the interrupt safe FreeRTOS API
        functions can be used (those that end in FromISR()). */

    lv_tick_inc(1); // for lvgl
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
        //LEDWrite(LED_D1,0); // test

        /* Create the tasks that use the TCP/IP stack if they have not already
        been created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS-Plus-TCP can be created here
             * to ensure they are not created before the network is usable.
             */

            xTasksAlreadyCreated = pdTRUE;
        }
    }
}

BaseType_t xApplicationGetRandomNumber( uint32_t *pulValue ){
    *( pulValue ) = rand();
    return pdTRUE;
}

/*
 * Callback that provides the inputs necessary to generate a randomized TCP
 * Initial Sequence Number per RFC 6528.  THIS IS ONLY A DUMMY IMPLEMENTATION
 * THAT RETURNS A PSEUDO RANDOM NUMBER SO IS NOT INTENDED FOR USE IN PRODUCTION
 * SYSTEMS.
 */
extern uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                                    uint16_t usSourcePort,
                                                    uint32_t ulDestinationAddress,
                                                    uint16_t usDestinationPort )
{
    ( void ) ulSourceAddress;
    ( void ) usSourcePort;
    ( void ) ulDestinationAddress;
    ( void ) usDestinationPort;

    return rand();
}
