//header files




//------------------------------------------
// TivaWare Header Files
//------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h" //Macros defining memory map of the device
#include "driverlib/sysctl.h" //Prototypes for the system control driver
#include "driverlib/gpio.h" //GPIO API
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
//long int* opPinSel = GPIO_PIN_1;
int opPinVal = 2;
int SW2_state_num = 0;
int SW2_state = 0;
int SW2_state_prev = 0;

int SW1_state_num = 0;
int SW1_state = 0;
int SW1_state_prev = 0;
long int baseDelayVal = 6700000;
long int delayVal = 6700000;
bool input_array[8];
int output_array[8];
int i;
int opcodeval;
int opr1;
int opr2;
int opr_output;
//---------------------------------------------------------------------------
void main(void)
{

   hardware_init();                         // init hardware via Xware

   while(1)                                 // forever loop
   {
       //Input Stage
       input_array[0] = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
       input_array[1] = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3);
       input_array[2] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2);
       input_array[3] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3);
       input_array[4] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_4);
       input_array[5] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5);
       input_array[6] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6);
       input_array[7] = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);

       //processing stage
       //for (i = 0; i<8; i++){
       //    output_array[i] = input_array[i];
       //}
       //OpCode Transfer
       output_array[7] = input_array[7];
       output_array[6] = input_array[6];
       //operation selector
       opcodeval = 2*input_array[7] + input_array[6];
       opr1 = 4*input_array[5] + 2*input_array[4] + input_array[3];
       opr2 = 4*input_array[2] + 2*input_array[1] + input_array[0];
       switch (opcodeval) {
       case 0:
           //pass as obtained
           opr_output = 8*opr1 + opr2;
           break;
       case 1:
           //sum
           opr_output = opr1 + opr2;
           break;
       case 2:
           opr_output = opr1 - opr2;
           break;
       case 3:
           opr_output = opr1 * opr2;
       }

       //output binary computation stage
       for (i=0; i<6; i++){
           output_array[i] = opr_output%2;
           opr_output /= 2;
       }

       //Output Stage
       GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 1*output_array[0]);
       GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 2*output_array[1]);
       GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 4*output_array[2]);
       GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1*output_array[3]);
       GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2*output_array[4]);
       GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4*output_array[5]);
       GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 8*output_array[6]);
       GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 16*output_array[7]);

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

}

void delay(long int delay_val)
{
     //SysCtlDelay(6700000);      // creates ~500ms delay - TivaWare fxn
     SysCtlDelay(delay_val);

}
