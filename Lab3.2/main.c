
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h" //Macros defining memory map of the device
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h" //Prototypes for the system control driver
#include "driverlib/systick.h"  //needed for the SysTick timer interrupt
#include "driverlib/gpio.h" //GPIO API
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include <time.h>
#include <math.h>

#include "myLib.h"
#include "lcd.h"

//setup
#define LOCK_F (*((volatile unsigned long*)0x40025520))
#define CR_F (*((volatile unsigned long*)0x40025524))
void hardware_init(void);

//timer Stuff------------------------------------------------------------------------------
volatile uint32_t msTicks = 0;  //simple counter variable to track time
//This function is supposed to execute automatically whenever the SysTick timer generates an interrupt.
void SysTick_Handler(void)
{
    msTicks++;   // increment every millisecond
}
//Initialize SysTick to generate an interrupt every 1 millisecond.
void SysTick_Init(void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1000);  //set the reload value to 1ms period
    SysTickIntRegister(SysTick_Handler);        // Register ISR; links the interrupt handler function.
    SysTickIntEnable();                         // Enable SysTick interrupt
    SysTickEnable();                            // Start SysTick timer
}
//delay function
void delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < ms) {}
}
//
uint32_t t_start = 0;
uint32_t t_stop = 0;
uint32_t t_diff = 0;
int AttemptNum = 0;
int TotalTime = 0;
int AvgTime = 0;
uint32_t t_resp = 0;

//----------------------------------------------------------------------------------------------------

void main() {
    hardware_init();

    lcd_init();

    //Initialize timer interrupt system
    IntMasterEnable();
    SysTick_Init();

    while(1)
    {
        lcd_cls_csr_strt();
        char string1[] = "Press switch SW1";
        lcd_string(string1);
        lcd_cursor(2, 1);
        char string2[] = "LED glows";
        lcd_string(string2);
        //turn on LED
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x01<<1);
        //start timer
        t_start = msTicks;
        while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))    //active when relaxed switch
        {
            //wait; a better way is to use gpio interrupt
        }
        AttemptNum++;
        //turn off LED
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        //stop timer
        t_stop = msTicks;
        t_diff = t_stop - t_start;//abs(t_stop - t_start);
        t_resp = t_diff - 10;   //10ms debounce of switch



        lcd_cls_csr_strt();
        char string3[] = "Attempt # ";
        lcd_string(string3);
        lcd_print(1, 11, AttemptNum, 2);
        lcd_cursor(2, 1);
        char string4[] = "Time = ";
        lcd_string(string4);
        lcd_print(2, 8, t_resp, 6);

        delay_ms(5000); //5s delay


        TotalTime += t_resp;

        if(AttemptNum == 5){
            lcd_cls_csr_strt();
            AvgTime = TotalTime/5;
            char string5[] = "Now press SW2";
            lcd_string(string5);
            lcd_cursor(2, 1);
            char string6[] = "Avg = ";
            lcd_string(string6);
            lcd_print(2, 7, AvgTime, 6);
            AvgTime = 0;
            TotalTime = 0;
            AttemptNum = 0;
            //delay_ms(5000); //5s delay

            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))    //active when relaxed switch
            {
                //wait for SW2 press; a better way is to use gpio interrupt
            }
            //light up green LED
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x01<<3);

            lcd_cls_csr_strt();
            char string7[] = "Press SW2 to Restart";
            lcd_string(string7);
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))    //active when relaxed switch
            {
               //wait for SW2 press; a better way is to use gpio interrupt
            }
            //turn off green LED
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
            //delay_ms(5000); //5s delay
        }

    }

}

void hardware_init(void){
    //additional configuration for LED and Switch
    //Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
    //SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_USE_OSC|SYSCTL_OSC_MAIN);

    // ADD Tiva-C GPIO setup - enables port, sets pins 1-3 (RGB) pins for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    LOCK_F = 0x4C4F434BU;
    CR_F = GPIO_PIN_0 | GPIO_PIN_4;


    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

}

