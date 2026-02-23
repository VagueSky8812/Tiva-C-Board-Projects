
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
#include "driverlib/debug.h"
#include <time.h>
#include <math.h>

int main(void)
{
    uint32_t ui32ADC0Value;
    //volatile uint32_t ui32VoltAvg;
    float VoltValueV = 0.0;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    //GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3);

    // Disable SS3 before configuration
    ADCSequenceDisable(ADC0_BASE, 3);

    // Configure SS3 for processor trigger, highest priority
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure SS3 step 0 to read AIN0
    // ADC_CTL_END marks the end of the sequence
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);



    while(1)
    {
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);

        ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value);
        VoltValueV = (ui32ADC0Value*3.3f) / 4095.0f;
    }
}



