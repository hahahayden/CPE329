/*
 * P1.c
 *
 *  Created on: Apr 22, 2020
 *      Author: Anthony
 */
#include "keypad.h"
#include "LCD.h"
#include "msp.h"
#include "P1.h"
#include<stdio.h>
#include<string.h>

int lockedArray[4];
int indexer=0;
int indexerLocked=0;
int compareArray[4];

void step_state(enum events event){
    uint8_t key;

    switch (states){
    //First state is the Start state
    case start:
        switch (events){
            case start_event:
                //Displaying the Start State on LCD
                Write_string_LCD("ENTER YOUR CODE:");
                LCD_command(0xC0);
                Write_string_LCD("START:");
                //This while loop takes in the user input and adds the numbers to the array for comparison
                //when * is pressed, the user can restart their input
                while (indexerLocked<4){
                    key = keypad_getkey();
                    if (key==42){
                        indexerLocked=0;
                        LCD_command(0xC6);
                        delay_us(50000);
                    }
                    else if (key!=255){
                        lockedArray[indexerLocked]=key;
                        printToLCD(key);
                        indexerLocked++;
                        delay_us(500000);
                    }
                }
                //When indexerLocked is 4, it goes to the next state
                states= locked_state;
                events= locked_event;
                break;
        }
    //Locked state is reached when a valid 4-digit pin is entered in the start state
    case locked_state:{
        switch (events){
            case locked_event:
            //This part clears the display to prepare for the locked state and waits for user to input the correct code
            LCD_command(0x01);
            Write_string_LCD("LOCKED");
            LCD_command(0xC0);
            Write_string_LCD("ENTER CODE:");
                //This while loop takes the user input and directs you to the change code state if # is input
                while(indexer<4){
                    key = keypad_getkey();
                    if (indexer==0 && key==35){
                    states=confirm_oldcode_state;
                    events=confirm_oldcode_event;
                    break;
                    }
                //When * is pressed, the user can restart their input
                else if(key==42){
                    key=10;
                    indexer=0;
                    LCD_command(0xCB);
                }
                //Adds the user input to an array for comparison and displays the value to the LCD
                else if (key<=9 || key==35){
                    printToLCD(key);
                    compareArray[indexer]=key;
                    indexer++;
                    delay_us(500000);
                }
                }
                //if the size entered is equal to 4 compare the input. If it matches it is unlocked. If it doesnt match it is redirected to the locked state
                if (indexer==4){
                    int comparison=memcmp(compareArray,lockedArray, sizeof(compareArray));
                    if (comparison==0){
                        LCD_command(0x01);
                        Write_string_LCD("UNLOCKED!");
                        int i=0;
                        for(i=0; i<4; i++){
                            compareArray[i]=0;
                        }
                        indexer=0;
                        states=unlocked_state;
                        events= unlocked_event;
                        break;
                    }
                    else{
                        indexer=0;
                        LCD_command(0x01);
                        states=locked_state;
                        events= locked_event;
                        break;
                    }
                }
        }
    }
    //Unlocked state is reached when a valid 4-digit pin is entered and matches with the code
    case unlocked_state:{
        switch (events){
            case unlocked_event:
                //This part allows for the user to change their code or keep their code
                LCD_command(0xC0);
                Write_string_LCD("CHANGE:1");
                Write_string_LCD("    RETAIN:2");
                int unlockedDeciderKey=255;
                while(unlockedDeciderKey==255){
                    unlockedDeciderKey=keypad_getkey();
                    if(unlockedDeciderKey==1){
                        //changing old code
                        states=confirm_oldcode_state;
                        events=confirm_oldcode_event;
                    }
                    else if (unlockedDeciderKey==2){
                        //go back to locked state
                        states=locked_state;
                        events=locked_event;
                    }
                }
        }
    }
    //Confirm old code state is reached when the user wants to change their code
    case confirm_oldcode_state:{
        switch (events){
            case confirm_oldcode_event:
                //This sets up the LCD to change their code
                LCD_command(0x01);
                Write_string_LCD("CHANGE CODE");
                LCD_command(0xC0);
                Write_string_LCD("CONFIRM CODE:");
                indexer=0;    // make sure the index is at 0 should be set already
                //This while statement takes in the user input and compares it to the old code
                while(indexer<4){
                    key = keypad_getkey();
                    if (key>9 && key!=255){
                        //When * is pressed, the user can restart their input
                        if (key==42){
                            LCD_command(0xCD);
                            key=10;
                            indexer=0;
                        }
                        else{
                            printToLCD(key);
                            compareArray[indexer]=key;
                            delay_us(500000);
                            indexer++;
                        }
                    }
                    else if (key<=9){
                        printToLCD(key);
                        compareArray[indexer]=key;
                        indexer++;
                        delay_us(500000);
                    }
                }
                //This checks if the user input matches the new code. If it does the next state is New Code. If it doesnt, the next state is the locked state
                if (indexer==4){
                    int comparison=memcmp(compareArray,lockedArray, sizeof(compareArray));
                    if (comparison==0){
                        LCD_command(0x01);
                        Write_string_LCD("OLD CODE CONFIRMED!");
                        delay_us(2000000);
                        int i=0;
                        for(i=0; i<4; i++){
                            compareArray[i]=0;
                        }
                        indexer=0;
                        states=newCode_state;
                        events= newCode_event;
                        break;
                    }
                    else{
                        indexer=0;
                        LCD_command(0x01);
                        states=locked_state;
                        events= locked_event;
                        break;
                    }
                }
        }
    }
    //New code state is reached only when the user puts in the correct old code
    case newCode_state:
        switch (events){
            case newCode_event:
                //Setting up the LCD display for user input. This takes in the values for the new 4-digit code.
                LCD_command(0x01);
                Write_string_LCD("ENTER NEW CODE:");
                LCD_command(0xC0);
                indexerLocked=0;
                while (indexerLocked<4){
                    key = keypad_getkey();
                    //If * is input, it resets the user input
                    if (key==42){
                        indexerLocked=0;
                        LCD_command(0xC0);
                    }
                    else if (key!=255){
                        lockedArray[indexerLocked]=key;
                        printToLCD(key);
                        indexerLocked++;
                        delay_us(500000);
                    }
                }
                //Going to the next state
                states= confirm_newcode_state;
                events= confirm_newcode_event;
                break;
        }
    //Confirm new code is reached once the user inputs the initial new code. If it matches it confirms new code and goes to the unlocked state
    case confirm_newcode_state:
        switch (events){
            case confirm_newcode_event:
                //Setting up lcd for the new code input for the user
                LCD_command(0x01);
                Write_string_LCD("CONFIRM NEW CODE:");
                LCD_command(0xC0);
                indexer=0;
                //This while statement checks if the user input is a vaild input and displays it on the LCD
                while(indexer<4){
                    key = keypad_getkey();
                    if (key>9 && key!=255){
                        if (key==42){
                            LCD_command(0xC0) ;
                            key=10;
                            indexer=0;
                        }
                    else{
                        printToLCD(key);
                        compareArray[indexer]=key;
                        delay_us(500000);
                        indexer++;
                    }
                    }
                    else if (key<=9){
                        printToLCD(key);
                        compareArray[indexer]=key;
                        indexer++;
                        delay_us(500000);
                    }
                }
                //if the size entered is equal to 4 compare the input. If correct, the code will change and the unlocked state will be reached
                //if incorrect, the lcd will display incorrect, try again.
                //then it will wait for the user to input the correct code
                int comparison=memcmp(compareArray,lockedArray, sizeof(compareArray));
                if (comparison==0){
                    LCD_command(0x01);
                    Write_string_LCD("CODE CHANGED!");
                    int i=0;
                    for(i=0; i<4; i++){
                        compareArray[i]=0;
                    }
                    indexer=0;
                    states=unlocked_state;
                    events= unlocked_event;
                    break;
                }
                else{
                    indexer=0;
                    LCD_command(0x01);
                    Write_string_LCD("INCORRECT");
                    LCD_command(0xC0);
                    Write_string_LCD("TRY AGAIN");
                    delay_us(2000000);
                    states=confirm_newcode_state;
                    events= confirm_newcode_event;
                    break;
                }
        }

    }
}
