#include "keypad.h"
#include "LCD.h"
#include "msp.h"
#include<stdio.h>
#include<string.h>

void over(void){
        LCD_command(0x01);
        Write_string_LCD("   Keypad Entry:");
        LCD_Line2();
        delay_us(500000);
}

void printToLCD(uint8_t key){
    //we have an unsigned integer and we want to be able to make it to unsigned char to write to LCD

    // unsigned char displays these numbers in ASCII so we basically making these ints to ASCII so that it can read it as ASCII
    // convert these integers to ASCII numbers so that unsigned char reads it by the ASCII code
    //LCD_command(0x01);
    unsigned char variable;
    if (key<10){
    variable= key+0x30;
    }
    else{
        variable=key;
    }
        //printf("Variable%u\n", variable);

    Write_char_LCD(variable);
    delay_us(10);
    LCD_command(0x06);
}

void keypad_init(void) {
    P5->DIR = 0;                            // make all pins an input
    P5->REN |= (ROW1 | ROW2 | ROW3 | ROW4); // enable resistor for row pins
    P5->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4);// make row pins pull-down
}

uint8_t keypad_getkey(void) {
    uint8_t row, col, key;

    /* check to see any key pressed */
    P3->DIR |= (COL1 | COL2 | COL3);  // make the column pins outputs
    P3->OUT |= (COL1 | COL2 | COL3);  // drive all column pins high
    _delay_cycles(25);                // wait for signals to settle

    row = P5->IN & (ROW1 | ROW2 | ROW3 | ROW4);   // read all row pins

    if (row == 0)           // if all rows are low, no key pressed
        return 0xFF;

    /* If a key is pressed, it gets here to find out which key.
     * It activates one column at a time and reads the input to see
     * which row is active.
     */

    for (col = 0; col < 3; col++) {
        // zero out bits 6-4
        P3->OUT &= ~(COL1 | COL2 | COL3);

        // shift a 1 into the correct column depending on which to turn on
        P3->OUT |= (COL1 << col);
        _delay_cycles(25);            // wait for signals to settle

        row = P5->IN & (ROW1 | ROW2 | ROW3 | ROW4); // mask only the row pins

        if (row != 0) break;      // if the input is non-zero, key detected
    }

    P3->OUT &= ~(COL1 | COL2 | COL3);   // drive all columns low
    P3->DIR &= ~(COL1 | COL2 | COL3);   // disable the column outputs

    if (col == 3)   return 0xFF;        // if we get here, no key was detected

    // rows are read in binary, so powers of 2 (1,2,4,8)
    if (row == 16) row = 1;
    if (row == 32) row = 2;
    if (row==64) row=3;
    if (row==128) row=4;

    /*******************************************************************
     * IF MULTIPLE KEYS IN A COLUMN ARE PRESSED THIS WILL BE INCORRECT *
     *******************************************************************/

    // calculate the key value based on the row and columns where detected
    if (col == 0) key = row*3 - 2;
    if (col == 1) key = row*3 - 1;
    if (col == 2) key = row*3;

    if (key==10)    key = '*';
    if (key==12)    key = '#';
    if (key == 11)  key = 0; // fix for 0 key

    return key;
}

