/*
 * lcd.h
 *
 *  Created on: 26-Jan-2026
 *      Author: Anupam
 */

#ifndef LCD_H_
#define LCD_H_
////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
extern void lcd_portconfig(void);
extern void lcd_init();
extern void lcd_command(uint32_t cmd);
extern void lcd_busy();
extern void lcd_char(char data);
extern void lcd_string(char *g);
extern void lcd_cursor (int row, int column);
extern void lcd_print (int row, int column, uint32_t value, int digits);
extern void lcd_wrap_around();
extern uint8_t position;

#endif /*LCD_H_*/
