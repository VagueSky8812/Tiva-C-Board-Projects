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
#include <string.h>

// TASK 1 : Create a separate header file, named "myLib.h" and put all the required headers in the that file
// No built-in library file should be in this code
#include "myLib.h"

// TASK 2 : Create a header file named "lcd.h" and
// write the appropriate function declaration for the user defined function used over here
#include "lcd.h"

uint8_t position = 0x80;

#define lcd_delay    SysCtlDelay(67000 *2) // changed

//#define lcd_put_data(x)  {GPIOPinWrite(GPIO_PORTB_BASE,(0xFF), x); GPIOPinWrite(GPIO_PORTB_BASE,(0xFF), x);}
//0xFF is the mask here; means all pins
#define lcd_put_data(x)  {GPIOPinWrite(GPIO_PORTB_BASE,(0xFF), x);}             //put 8-bit data on data lines

#define lcd_set_RS() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_4),0x01<<4))           //tells that the 8-bit signal line has a data
#define lcd_set_RW() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_5),0x01<<5))           //tells that the 8-bit signal has to be read from the lcd
#define lcd_set_EN() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_6),0x01<<6))           //tells that info exchange is on

//#define lcd_reset_RS() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_4),~(0x01<<4)))    //tells that the 8-bit signal line has a command
//#define lcd_reset_RW() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_5),~(0x01<<5)))    //tells that the 8-bit signal has to be written to the lcd
//#define lcd_reset_EN() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_6),~(0x01<<6)))    //tells that info exchange is off

#define lcd_reset_RS() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_4),0))               //tells that the 8-bit signal line has a command
#define lcd_reset_RW() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_5),0))               //tells that the 8-bit signal has to be written to the lcd
#define lcd_reset_EN() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_6),0))               //tells that info exchange is off
//lcd works on the data only at the negative edge of EN signal

//------------------------------------------------------------------------------------------------------------------------------------------
//Command Set for LCD
//The position of the first non-zero bit (from MSB) determines the kind of the command.
//D7 = 1 -> display data RAM address set command,
//(DB6 - DB0) -> the address in the display RAM from where the next read/write will be carried out.
// 0b000 0000(0x80) for 1st line leftmost position; 0b000 1111(0x8f) for 1st line rightmost position;
// 0b100 0000(0xc0) for 2nd line leftmost position; 0b100 1111(0xcf) for 2nd line rightmost position;

//D7 D6 = 01 -> character generator address set command.

//D7 D6 D5 = 001 -> function set command, where
//D4 -> no. of data bits used by the interface(1 -> 8 bit interface, 0 -> 4 bit interface),
//D3 -> no. of lines in the display(1 -> 2 line display, 0 -> 1 line display),
//D2 -> font(1 -> 5x10 font, 0 -> 5x7 font).

//D7 D6 D5 D4 = 0001 -> shift command.
//D3 -> whether the display or the cursor will be shifted,
//D2 -> whether a right or left shift is desired.

//D7 D6 D5 D4 D3 = 00001 -> display switch command.
//D2 -> if the whole display will be turned on(1)/off(0),
//D1 -> turning the cursor on(1)/off(0)
//D0 -> turns blinking of the cursor on(1)/off(0).

//D7 D6 D5 D4 D3 D2 = 000001 -> input set command.
//D1 -> auto increment(1)/decrement(0) cursor for entering next data
//D0 -> whether the display should shift after entering the new character by one position.(1 -> whole display shift, 0 -> not so)

//D7 D6 D5 D4 D3 D2 D1 = 0000001 -> returns the cursor to the first position.

//D7 D6 D5 D4 D3 D2 D1 D0 = 00000001 -> clears screen
//------------------------------------------------------------------------------------------------------------------------------------------

/*
 * lcd_portconfig();
 * Description: Sets LCD port pin as output
 * Datapins: PORTB pin 0 to pin7 (LCD is interfaced in 8 bit mode)
 * ControlPins: PORTC pin4,pin5 and pin6 is connected to RS, RW and EN respectively.
 * These connections are for TIVA booster board designed by WEL lab. Change Port connection accroding to interface
 */

void lcd_portconfig(void)
{
    //Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
    //SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_USE_OSC|SYSCTL_OSC_MAIN);

    //* * * * * * Setting Ouput for LCD * * * * * * * *
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

    // * * * * * * LCD_Control Pin* * * * * * *
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, 0x70);

    lcd_delay;
    lcd_delay;
}

/*
 * lcd_init();
 * Description: Set ports pins as outputs. Initialize LCD in 8-bit mode.
 *
 * This should be the first function that must be called in main when using LCD
 */
void lcd_init()
{
    lcd_portconfig();

    lcd_reset_RS(); //tells that the 8-bit signal line has a command
    lcd_reset_RW(); //tells that the 8-bit signal has to be written to the lcd
    lcd_reset_EN(); //tells that info exchange is on

    lcd_command(0x30); // function set command; 8bit data
    lcd_command(0x38); // function set command; 8bit data/commands; 2 line display; 5x7 font
    lcd_command(0x0f); // display switch command; display turned on; cursor turned on; cursor blinking
    lcd_command(0x06); // input set command; auto increment cursor for entering next data; whole display doesn,t shift
    lcd_command(0x01); // clear screen command
    lcd_command(0x80); // Set cursor to first line starting
    position = 0x80;
}

void lcd_cls_csr_strt(){
    lcd_command(0x01); // clear screen command
    lcd_command(0x80); // Set cursor to first line starting
    position = 0x80;
}

/*
 * lcd_command()
 * Description: Sends command on databus
 * usage: lcd_command(0x01) - clears the display
 */
void lcd_command(uint32_t cmd)
{
    lcd_reset_RS();     //tells that the 8-bit signal line has a command
    lcd_reset_RW();     //tells that the 8-bit signal has to be written to the lcd
    lcd_delay;          //wait for signals to settle
    lcd_set_EN();       //tells that info exchange is on
    lcd_delay;          //wait for signals to settle
    lcd_put_data(cmd);  //put 8 bit command on the D lines
    lcd_delay;          //wait for signals to settle
    lcd_reset_EN();     //tells that info exchange is off
    //lcd works on the data only at the negative edge of EN signal
    lcd_delay;          //wait for signals to settle
}

void lcd_busy() {
    lcd_set_RS();       //tells that the 8-bit signal line has a data
    lcd_set_RW();       //tells that the 8-bit signal has to be read from the lcd
    lcd_delay;          //wait for signals to settle
    lcd_set_EN();       //tells that info exchange is on
    lcd_delay;          //wait for signals to settle
    lcd_reset_EN();     //tells that info exchange is off
    //lcd works on the data only at the negative edge of EN signal
    //now read the byte from port b
    while(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_7)){
        //wait; do nothing
    }
}

//lcd_wrap_around()
void lcd_wrap_around() {
    if ((position > 0x8f) && (position < 0xc0)){
        position = 0xc0;
        lcd_command(position);
    }
    else if (position > 0xcf){
        position = 0x80;
        lcd_command(position);
    }
}

/*
 *lcd_char()
 *Description: Print single character
 *Example lcd_char('A'); prints letter A
 */
void lcd_char(char data)
{
    lcd_set_RS();       //tells that the 8-bit signal line has a data
    lcd_reset_RW();     //tells that the 8-bit signal has to be written to the lcd
    lcd_delay;          //wait for signals to settle
    lcd_set_EN();       //tells that info exchange is on
    lcd_delay;          //wait for signals to settle
    lcd_put_data(data);  //put 8 bit command on the D lines
    lcd_delay;          //wait for signals to settle
    lcd_reset_EN();     //tells that info exchange is off
    //lcd works on the data only at the negative edge of EN signal
    lcd_delay;          //wait for signals to settle

    position++;
    lcd_wrap_around();
}

/*
 * lcd_string()
 * Description: Print string
 * Example: lcd_string("Hello World");
 */
void lcd_string(char *g){
        while(*g) {lcd_char(*g++);}
    }

/*
 * Name: lcd_cursor (row, column). For setting cursor position in 16 by 2 lcd
 * Description: Position the LCD cursor at "row", "column"
 * row: 1,2
 * column: 1 to 16
 * Example: lcd_cursor(2,14) - Places cursor at 2nd line 14th column
*/

void lcd_cursor (int row, int column)
{
    /* TASK 3 : Write the code to set the cursor position*/
    uint8_t position_row_col = 0x80 + (row - 1)*(0x40) + (column - 1);
    position = position_row_col;
    lcd_command(position);
    lcd_wrap_around();

}

/*
 * Name: lcd_print (value, digit). Print value (a numeric number).
 * Description: Print number
 * value: Numeric number
 * digit: number of digits in number. Maximum allowed digit is 7
 * Example: lcd_print(456,3) - Print 456 on LCD
*/

void lcd_print (int row, int column, uint32_t value, int digits)
{
    /* row    : Input Cursor position for the row position
       column : Input Cursor position for the column position
       value  : Input integer value to be displayed on LCD
       digits : Input number to depict the number of decimal places u want to show*/
    lcd_cursor(row, column);

    int divider = pow(10, digits-1);
    while (divider){
        lcd_char('0' + (char)(value/divider));
        value %= divider;
        divider /= 10;
    }

    /* TASK 4 : Write the code to convert the integer number into ASCII format so
       that can passed to the LCD for display */
    /* You can declare your own variables also*/

}
