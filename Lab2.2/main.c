
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
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include <time.h>
#include <math.h>

int main(void)
{
    uint32_t ui32ADC0Value;
    //volatile uint32_t ui32VoltAvg;
    double VoltValueV = 0.0;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);//ADC
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

    //DAC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//DAC
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    // Wait until ready
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0); //DAC
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0));

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    //GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5);

    SSIConfigSetExpClk(SSI0_BASE,
                       SysCtlClockGet(),
                       SSI_FRF_MOTO_MODE_0, // SPI Mode 3
                       SSI_MODE_MASTER,
                       10000,             // 4 MHz SPI clock
                       16);                  // 8-bit data
    // Enable the SSI module
    //
    SSIEnable(SSI0_BASE);


    while(1)
    {
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);

        ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value);
        VoltValueV = (ui32ADC0Value*3.3f) / 4096.0f;

        SSIDataPut(SSI0_BASE, (0x00003000 | ui32ADC0Value));
        while(SSIBusy(SSI0_BASE))
        {
        }
    }
}



