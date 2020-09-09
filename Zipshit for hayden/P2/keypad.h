/*
* keypad.h
*
*  Created on: Apr 17, 2020
*      Author: Anthony
*/

#ifndef keypad_C_
#define keypad_C_
#include "msp.h"
#include "LCD.h"
#include <stdint.h>
#include <stdio.h>

// defining the variables to be used
#define COL1  BIT5
#define COL2  BIT6
#define COL3  BIT7
#define ROW1  BIT4
#define ROW2  BIT5
#define ROW3  BIT6
#define ROW4  BIT7
// declaring the functions to be used
uint8_t keypad_getkey(void);
void keypad_init(void);
void printToLCD(uint8_t key);
void over(void);

#endif /* keypad_C_ */
