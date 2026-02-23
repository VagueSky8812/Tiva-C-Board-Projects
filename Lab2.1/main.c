/*

//------------------------------------------
// TivaWare Header Files
//------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h" //Macros defining memory map of the device
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h" //Prototypes for the system control driver
#include "driverlib/gpio.h" //GPIO API
#include "driverlib/adc.h"
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
//-------------------------------------------
void main(void)
{

   hardware_init();                         // init hardware via Xware

   while(1)                                 // forever loop
   {
       //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 1);
       //GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 1);
       //delay(6700000);

       //GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 16);
       ADCIntClear(ADC0_BASE, 1);
       ADCProcessorTrigger(ADC0_BASE, 1);
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
    //ADC Clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // ADD Tiva-C GPIO setup - enables port, sets pins 1-3 (RGB) pins for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //Port E pin 3 as GPIO Input and ADC Input
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    //Some Pins are output pins

    //
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0 );
    ADCSequenceEnable(ADC0_BASE, 1);


    //GPIO_O_LOCK
    //GPIO_O_CR



    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    //GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);


    //GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);

    //GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

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

*/
