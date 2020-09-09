/*
 * P1.h
 *
 *  Created on: Apr 22, 2020
 *      Author: Anthony
 */

#ifndef P1_H_
#define P1_H_

#include "keypad.h"
#include "LCD.h"
#include "msp.h"
#include<stdio.h>
#include<string.h>

//Declaring the different states
enum states{
    start,
    locked_state,
    unlocked_state,
    reset_state,
    confirm_oldcode_state,
    newCode_state,
    confirm_newcode_state
}states ;

//Declaring the different events
enum events{
    start_event,
    locked_event,
    unlocked_event,
    reset_event,
    confirm_oldcode_event,
    newCode_event,
    confirm_newcode_event
}events;

void step_state(enum events event);


#endif /* P1_H_ */
