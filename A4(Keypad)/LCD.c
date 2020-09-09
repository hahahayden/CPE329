
#include "LCD.h"
#include "msp.h"
#include<stdio.h>
#include<string.h>

void Write_string_LCD(char* str)
{
   int i = 0;
   for(i = 0; i < strlen(str); i++)
   {
       Write_char_LCD(str[i]);
   }
}

void LCD_init(void) {
   P4->DIR = 0xFF;                 /* make P4 pins output for data and controls */
   delay_us(30000);
   LCD_4bit_write(0x30, 0);        /* initialization sequence */
   delay_us(40);                //delay can be replicated from writecom subroutine on page 11
   LCD_4bit_write(0x30, 0);
   delay_us(40);
   LCD_4bit_write(0x30, 0);
   delay_us(40);
   LCD_4bit_write(0x20, 0);        /* use 4-bit data mode */
   delay_us(1000);
   LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
   LCD_command(0x06);      /* move cursor right after each char */
   LCD_command(0x01);      /* clear screen, move cursor to home */
   LCD_command(0x0F);      /* turn on display, cursor blinking */
}

/* With 4-bit mode, each command or data is sent twice with upper
* nibble first then lower nibble.
*/

void LCD_4bit_write(unsigned char data, unsigned char control) {
   data &= 0xF0;                    /* clear lower nibble for control */
   control &= 0x0F;                /* clear upper nibble for data*/
   P4->OUT = data | control;   /* RS = 0, R/W = 0;  allows the above to fit in 4bit data 3 bit control*/
   P4->OUT = data | control | EN;  /* pulse E */
   delay_us(5);                   // can change lower since enable pulses in ns
   P4->OUT = data;                 /* clear E */
   P4->OUT = 0;
}

void LCD_command(unsigned char command) {
   LCD_4bit_write(command & 0xF0, 0);    /* upper nibble first */
   LCD_4bit_write(command << 4, 0);      /* then lower nibble */
   if (command < 4)
   {
       delay_us(4000);         /* commands 1 and 2 need up to 1.64ms */
   }
   else
       delay_us(70);         /* all others 40 us*/
}

void Write_char_LCD(unsigned char data) {
   LCD_4bit_write(data & 0xF0, RS);    /* upper nibble first */
   LCD_4bit_write(data << 4, RS);      /* then lower nibble  */
   delay_us(1000000);                   // viewable to see the cursor moving
}

void delay_us(int time)
{
   // default is 30000000 for clock
   int freq= 3000000;
   int clock;
   int i;
   clock=time/8;
   for (i=clock; i>0; i--);                // create a for loop with the appropriate number of iterations to create a delay approximate to the input
}

void LCD_Line2(){
   LCD_command(0xC0);
}

void clear_LCD(){
   LCD_command(0x01);
}
void Home_LCD(){
   LCD_command(0x80);
}
