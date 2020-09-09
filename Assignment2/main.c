/*
 * Initialize and display "hello world" and "Assignment 3" on the LCD using
 * 4-bit data mode.
 * Data and control pins share Port 4.
 */
#include "msp.h"
#include "LCD.h"


int main(void) {
   LCD_init();
   for(;;)
   {
       LCD_command(0x01);    //* clear display
       Write_char_LCD('H');  //* write the word "Hello"
       Write_char_LCD('E');
       Write_char_LCD('L');
       Write_char_LCD('L');
       Write_char_LCD('O');

       Write_char_LCD(' ');
       Write_char_LCD('W');
       Write_char_LCD('O');
       Write_char_LCD('R');
       Write_char_LCD('L');  //* write the word "World"
       Write_char_LCD('D');
       delay_us(1000000);    //1 second viewing time to see the word displayed
       //LCD_command(0x01); testing that the clr cmd has long enough delay
       LCD_Line2();
       //Home_LCD(); Using 3 cmds to test delay and acry of the words in lcd
       Write_string_LCD("Assignment 3");   //write string to lcd
       delay_us(1261000);

  }
}
