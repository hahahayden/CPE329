/* Keypad.c: Matrix keypad scanning
*
*/
#include "msp.h"
#include "LCD.h"
#include "keypad.h"

int main(void) {
    uint8_t key, rgb;
    int LCDletterCounter=0;
    // setup GPIO for multicolor LED (2.0-2.2) for key bits 2-0
    P2->DIR |= 0x07;    // make pins output
    P2->OUT &= ~0x07;   // turn LED off

    P1->DIR |= BIT0;     // use red led for bit key bit 3
    P1->OUT &= ~BIT0;    // turn LED off

    keypad_init();      // setup gpio pins for keypad
    LCD_init();

    while(1) {
        key = keypad_getkey();  // read the keyboard value; returns as unsigned int of 8bits
        // for * and # they return as ascii value

        if (key>9 && key!=255)  // check if the input is either nothing and greater than 9

        {
            LCDletterCounter++;         // counter to see how many times written onto LCD
             if (LCDletterCounter==41)  // if 40 written, clear screen and reset count
             {
                 LCD_command(0x01);
                 LCDletterCounter=0;
             }
             else if (LCDletterCounter==21)    // if 20 filled up, go to line 2
             {
                 LCD_Line2();
             }

             if (key==42)              // using ascii value to determine if '*' is the input
            {                          // we have to print as char
                printf("%c\n", key);  // printing key value to console;
                printToLCD(key);     // printing to the LCD
                key=10;             // key value for '*'

            }
            else                    // for case '#', 0 button taken care of in getKey
            {
                printf("%c\n",key);   // print key value to console; unsigned char
                printToLCD(key);      // printing '#' on the LCD
                key=12;              // key value for '#'
            }

        }
        else if (key<=9)     // for cases less than or equal to 9 on the key value
        {
            LCDletterCounter++;
                 if (LCDletterCounter==41)
                 {
                     LCD_command(0x01);
                     LCDletterCounter=0;
                 }
                 else if (LCDletterCounter==21)
                 {
                     LCD_Line2();
                 }
                 printToLCD(key);
                 printf("%d\n",key);            // print key value to console
        }

        rgb = key & 0x07;                       // only keep bottom 3 bits
        P2->OUT = (P2->OUT &= ~ 0x07) | rgb;    // zero bottom 3 bits before
                                                // being set by key value

        key = (key >> 3); // shift bit 4 to bit 0
        P1->OUT = (P1->OUT &= ~BIT0) | key;   // only set bit 0 wi8 idk th key
    }
}













/* Keypad.c: Matrix keypad scanning
*
*/
/*
#include "msp.h"
#include "LCD.h"
#include "keypad.h"



int main(void) {
    uint8_t key, rgb;
    int LCDletterCounter=0;
    // setup GPIO for multicolor LED (2.0-2.2) for key bits 2-0
    P2->DIR |= 0x07;    // make pins output
    P2->OUT &= ~0x07;   // turn LED off

    P1->DIR |= BIT0;     // use red led for bit key bit 3
    P1->OUT &= ~BIT0;    // turn LED off

    keypad_init();      // setup gpio pins for keypad
    LCD_init();
    while(1) {
        key = keypad_getkey();  // read the keyboard value

        if (key>9 & key!=255)
        {
            LCDletterCounter++;
             if (LCDletterCounter==41)
             {
                 LCD_command(0x01);
                 LCDletterCounter=1;
             }
             else if (LCDletterCounter==21)
             {
                 LCD_Line2();
             }

             if (key==42)
            {

                printf("%c\n", key);
                printToLCD(key);

                key=10;
            }
            else
            {
                printf("%c\n",key);
                printToLCD(key);
                key=12;
            }

        }
        else if (key<=9)
        {
            LCDletterCounter++;
                 if (LCDletterCounter==41)
                 {
                     LCD_command(0x01);
                     LCDletterCounter=1;
                 }
                 else if (LCDletterCounter==21)
                 {
                     LCD_Line2();
                 }

                 printToLCD(key);
                 printf("%d\n",key);     // print key value to console
        }


        rgb = key & 0x07; // only keep bottom 3 bits
        P2->OUT = (P2->OUT &= ~ 0x07) | rgb;    // zero bottom 3 bits before
                                                // being set by key value

        key = (key >> 3); // shift bit 4 to bit 0
        P1->OUT = (P1->OUT &= ~BIT0) | key;   // only set bit 0 wi8 idk th key
    }
}
*/





/* Keypad.c: Matrix keypad scanning
 *
 */
/*
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD.h"
#include "LCD.c"
#include "keypad.h"
#include "keypad.c"

void delay(void);

int main(void) {
    uint8_t key, rgb;
    int cursorcnt=0;
    char input[19] = {0};
    // setup GPIO for multicolor LED (2.0-2.2) for key bits 2-0
    P2->DIR |= 0x07;    // make pins output
    P2->OUT &= ~0x07;   // turn LED off

    P1->DIR |= BIT0;     // use red led for bit key bit 3
    P1->OUT &= ~BIT0;    // turn LED off

    keypad_init();      // setup gpio pins for keypad
    LCD_init();
    LCD_command(0x01);
    Write_string_LCD("   Keypad Entry:");
    LCD_Line2();
    delay_us(500000);

    while(1) {
        int i =0;
        key = keypad_getkey();  // read the keyboard value

        if (key>9 & key!=255){
            if (key==42){
                printToLCD(key);
                key=10;
                cursorcnt++;
                input[cursorcnt-1]='*';

                //Used for identifying the array
                for(i=0;i<20;i++){
                    printf("%d ", input[i]);
                }
                printf("\n");

                delay_us(400000);

                //determining if cursor cnt is over 21
                if  (cursorcnt>=21){
                    over();
                    cursorcnt = 1;
                    printToLCD(key);
                    for(i=0;i<20;i++){
                        input[i]=0;
                    }
                    input[cursorcnt-1]='*';
                    //Used for identifying the array
                    for(i=0;i<20;i++){
                        printf("%d ", input[i]);
                    }
                    printf("\n");

                }
            }

            else{
                printToLCD(key);
                key=12;
                cursorcnt++;
                input[cursorcnt-1]='#';
                for(i=0;i<20;i++){
                    printf("%d ", input[i]);
                }
                printf("\n");
                delay_us(400000);

                if  (cursorcnt>=21){
                    over();
                    cursorcnt = 1;
                    printToLCD(key);
                    for(i=0;i<20;i++){
                        input[i]=0;
                    }
                    input[cursorcnt-1]='#';
                    //Used for identifying the array
                    for(i=0;i<20;i++){
                        printf("%d ", input[i]);
                    }
                    printf("\n");
                }
            }
        }

        else{
        if (key!=255){
        printToLCD(key);
        cursorcnt++;
        input[cursorcnt-1]=key;
        for(i=0;i<20;i++){
            printf("%d ", input[i]);
        }
        printf("\n");
        delay_us(400000);

        if  (cursorcnt>=21){
            over();
            cursorcnt = 1;
            printToLCD(key);
            for(i=0;i<20;i++){
                input[i]=0;
            }
            input[cursorcnt-1]=key;
            //Used for identifying the array
            for(i=0;i<20;i++){
                printf("%d ", input[i]);
            }
            printf("\n");
        }
        }
        }
        rgb = key & 0x07; // only keep bottom 3 bits
        P2->OUT = (P2->OUT &= ~ 0x07) | rgb;    // zero bottom 3 bits before
                                                // being set by key value

        key = (key >> 3); // shift bit 4 to bit 0
        P1->OUT = (P1->OUT &= ~BIT0) | key;   // only set bit 0 wi8 idk th key
    }
}
*/
