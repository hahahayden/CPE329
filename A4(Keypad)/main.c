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
        key = keypad_getkey();  // read the keyboard value

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

            {

                printf("%c\n", key);  // printing key value to console

                printToLCD(key);     // printing to the LCD


                key=10;           // key value for '*'

            }
            else                 // for case '#', 0 button taken care of in getKey
            {
                printf("%c\n",key);   // print key value to console
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

                 LCD_command(0x45);
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
