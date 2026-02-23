//---------------------------------------------------------------------------
// Project: Blink TM4C - CCS Lab - STARTER
//
//
//
//
//
//---------------------------------------------------------------------------


//------------------------------------------
// TivaWare Header Files
//------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <time.h>
#include <math.h>


//----------------------------------------
// Prototypes
//----------------------------------------
void hardware_init(void);
void ledToggle(int pin, int val);
void delay(long int delay_val);


//---------------------------------------
// Globals
//---------------------------------------
volatile int16_t i16ToggleCount = 0;


//---------------------------------------------------------------------------
#define LOCK_F (*((volatile unsigned long*)0x40025520))
#define CR_F (*((volatile unsigned long*)0x40025524))
// main()
long int* opPinSel = GPIO_PIN_1;
int opPinVal = 2;
int SW2_state_num = 0;
int SW2_state = 0;
int SW2_state_prev = 0;

int SW1_state_num = 0;
int SW1_state = 0;
int SW1_state_prev = 0;
long int baseDelayVal = 6700000;
long int delayVal = 6700000;
//---------------------------------------------------------------------------
void main(void)
{

   hardware_init();                         // init hardware via Xware

   while(1)                                 // forever loop
   {
       SW2_state = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
       if (!SW2_state){			//active on relaxed switch
           if (SW2_state_prev){
               SW2_state_num = (SW2_state_num + 1)%3;
               opPinSel = GPIO_PIN_1 * (long int)pow(2, SW2_state_num);
               opPinVal = 2 * pow(2, SW2_state_num);
           }
       }

       SW1_state = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
       if (!SW1_state){
           if (SW1_state_prev){
               SW1_state_num = (SW1_state_num + 1)%3;
               delayVal = baseDelayVal * pow(2, SW1_state_num);
           }
       }

       ledToggle(opPinSel, opPinVal);
       delay(delayVal);                             // create a delay of ~1/2sec

       i16ToggleCount += 1;                 // keep track of #toggles
       SW1_state_prev = SW1_state;
       SW2_state_prev = SW2_state;
   }

}


//---------------------------------------------------------------------------
// hardware_init()
//
// inits GPIO pins for toggling the LED
//---------------------------------------------------------------------------
void hardware_init(void)
{
    //Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // ADD Tiva-C GPIO setup - enables port, sets pins 1-3 (RGB) pins for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    LOCK_F = 0x4C4F434BU;
    CR_F = GPIO_PIN_0 | GPIO_PIN_4;


    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Turn on the LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);

}


//---------------------------------------------------------------------------
// ledToggle()
//
// toggles LED on Tiva-C LaunchPad
//---------------------------------------------------------------------------
void ledToggle(int pin, int val)
{
    // LED values - 2=RED, 4=BLUE, 8=GREEN
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, pin, val);
        /*delay();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        delay();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);*/

    }
}



//---------------------------------------------------------------------------
// delay()
//
// Creates a 500ms delay via TivaWare fxn
//---------------------------------------------------------------------------
void delay(long int delay_val)
{
     //SysCtlDelay(6700000);      // creates ~500ms delay - TivaWare fxn
     SysCtlDelay(delay_val);

}




