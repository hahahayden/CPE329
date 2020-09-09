/*
* LCD.h*
*  Created on: Apr 14, 2020
*      Author: Hayden
*/

#ifndef LCD_C_
#define LCD_C_
#define RS 1     /* P4.0 mask */
#define RW 2     /* P4.1 mask */
#define EN 4     /* P4.2 mask */
#define FREQ_1_5_Mhz 1500000
#define FREQ_3_Mhz 3000000
#define FREQ_6_Mhz 6000000
#define FREQ_12_Mhz 12000000
#define FREQ_24_Mhz 24000000
#define FREQ_48_Mhz 48000000

/*void delayMs(int n);*/
void LCD_4bit_write(unsigned char data, unsigned char control);
void Char_LCD_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void Write_char_LCD(unsigned char data);
void LCD_init(void);
void delay_us(int n);
void Write_string_LCD(char* str);
void LCD_Line2();
//void Home_LCD();
//void clear_LCD();

#endif
/* LCD_C_ */
