/*
 * p3_3.c: Initialize and display "hello" on the LCD using 4-bit data mode.
 * Data and control pins share Port 4.
 */

#include "msp.h"
#include "LCD.h"


int main(void) {
    LCD_init();

    for(;;)
    {
        LCD_command(0x01);     //* clear display

        Write_char_LCD('H');      //* write the word "Hello"
        Write_char_LCD('E');
        Write_char_LCD('L');
        Write_char_LCD('L');
        Write_char_LCD('O');

        Write_char_LCD(' ');
        Write_char_LCD('W');
        Write_char_LCD('O');
        Write_char_LCD('R');
        Write_char_LCD('L');      //* write the word "World"
        Write_char_LCD('D');

        delay_us(1000000);      //1 second viewing time to see the word displayed
        //LCD_command(0x01);    //testing that the clear command has long enough delay
        LCD_Line2();
        //Home_LCD();           //Using triple commands to test delay and accuracy of the words printed in lcd
        Write_string_LCD("Hello Hayden");//write string to lcd
        LCD_command(0xC6);
        delay_us(2500000);
        Write_string_LCD("NOPE");
        delay_us(5261000);
   }
}





/*
 * p3_3.c: Initialize and display "hello" on the LCD using 4-bit data mode.
 * Data and control pins share Port 4.
 */

/*
#include "msp.h"

#define RS 1     // P4.0 mask
#define RW 2     // P4.1 mask
#define EN 4     // P4.2 mask

void delayMs(int n);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);

int main(void) {
    LCD_init();

    for(;;) {
        LCD_command(1);     // clear display
        delayMs(500);
        LCD_command(0x80);  // set cursor at beginning of first line
        LCD_data('h');      // write the word "Hello"
        LCD_data('e');
        LCD_data('l');
        LCD_data('l');
        LCD_data('o');
        delayMs(500);
    }
}

void LCD_init(void) {
    P4->DIR = 0xFF;     // make P4 pins output for data and controls
    delayMs(30);                // initialization sequence
    LCD_nibble_write(0x30, 0);
    delayMs(10);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x20, 0);  // use 4-bit data mode
    delayMs(1);

    LCD_command(0x28);      // set 4-bit data, 2-line, 5x7 font
    LCD_command(0x06);      // move cursor right after each char
    LCD_command(0x01);      // clear screen, move cursor to home
    LCD_command(0x0F);      // turn on display, cursor blinking
}

// With 4-bit mode, each command or data is sent twice with upper
//nibble first then lower nibble.

void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;           // clear lower nibble for control
    control &= 0x0F;        // clear upper nibble for data
    P4->OUT = data | control;       // RS = 0, R/W = 0
    P4->OUT = data | control | EN;  // pulse E
    delayMs(0);
    P4->OUT = data;                 // clear E
    P4->OUT = 0;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    // upper nibble first
    LCD_nibble_write(command << 4, 0);      // then lower nibble

    if (command < 4)
        delayMs(4);         // commands 1 and 2 need up to 1.64ms
    else
        delayMs(1);         // all others 40 us
}

void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);    // upper nibble first
    LCD_nibble_write(data << 4, RS);      // then lower nibble

    delayMs(1);
}

// delay milliseconds when system clock is at 3 MHz
void delayMs(int n) {
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 750; i > 0; i--);      // Delay
}
*/
