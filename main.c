

/**
 * main.c
 */
#define TARGET_IS_TM4C129_RA2 /*TM4C129 devices, silicon revision A2. Part revision 3(marking)*/

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

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
#include "driverlib/ssi.h" // SPI Microwire etc
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

    for (;;)
    {

        /* Create a 1 second delay */
        //vTaskDelay( pdMS_TO_TICKS( 1000 ));
        //LEDWrite(LED_D1,1);
        //vTaskDelay( pdMS_TO_TICKS( 1000 ));
        //LEDWrite(LED_D1,0);
        if(ButtonsPoll(NULL,NULL) & USR_SW1){
            LEDWrite(LED_D1,1);
        } else {
            LEDWrite(LED_D1,0);
        }

        /* Print the Hello world! message. */
        UARTprintf("Hello world!\n");
        vTaskDelay( pdMS_TO_TICKS( 1000 ));

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
    PinoutSet(true, false); // use EMAC

    /*Buttons*/
    ButtonsInit();

    /* Configure UART0 to send messages to terminal. (ICD) */
    prvConfigureUART();

    /* UART 2*/
    /* Enable and configure the peripherals used by the uart. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    MAP_GPIOPinConfigure(GPIO_PD4_U2RX);
    MAP_GPIOPinConfigure(GPIO_PD5_U2TX);
    MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Wait for the UART6 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART2))
    {
    }

    MAP_UARTConfigSetExpClk(UART2_BASE, MAP_SysCtlClockGet(), 9600,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));

    /* UART 6*/
    /* Enable and configure the peripherals used by the uart. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    MAP_GPIOPinConfigure(GPIO_PP0_U6RX);
    MAP_GPIOPinConfigure(GPIO_PP1_U6TX);
    MAP_GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Wait for the UART6 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART6))
    {
    }

    MAP_UARTConfigSetExpClk(UART6_BASE, MAP_SysCtlClockGet(), 9600,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));

    /* UART 7*/
    /* Enable and configure the peripherals used by the uart. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    MAP_GPIOPinConfigure(GPIO_PC4_U7RX);
    MAP_GPIOPinConfigure(GPIO_PC5_U7TX);
    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Wait for the UART7 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART7))
    {
    }

    MAP_UARTConfigSetExpClk(UART7_BASE, MAP_SysCtlClockGet(), 9600,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));

    /* CAN 0 */
    // Enable the CAN0 module.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    //
    // Wait for the CAN0 module to be ready.
    //
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0))
    {
    }

    // Reset the state of all the message objects and the state of the CAN
    // module to a known state.
    //
    MAP_CANInit(CAN0_BASE);

    /* CAN 1 */

    /* ADC 0 */
    // Enable the ADC0 module.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Wait for the ADC0 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {
    }

    // Enable the first sample sequencer to capture the value of channel 0 when
    // the processor trigger occurs.
    MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
    MAP_ADCSequenceEnable(ADC0_BASE, 0);

    /* ADC 1 */
    // Enable the ADC1 module.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // Wait for the ADC0 module to be ready.
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
    {
    }

    // Enable the first sample sequencer to capture the value of channel 0 when
    // the processor trigger occurs.
    MAP_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    MAP_ADCSequenceStepConfigure(ADC1_BASE, 0, 0,
    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
    MAP_ADCSequenceEnable(ADC1_BASE, 0);


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

    // Initialize Master and Slave
    MAP_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), true);

    // Specify slave address
    MAP_I2CMasterSlaveAddrSet(I2C0_BASE, 0x3B, false);

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

    // Initialize Master and Slave
    MAP_I2CMasterInitExpClk(I2C2_BASE, MAP_SysCtlClockGet(), true);

    // Specify slave address
    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, 0x3B, false);

    /* SSI2 SPI*/
    /* Enable the peripheral */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    /* SSI3 SPI*/
    /* Enable the peripheral */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

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

