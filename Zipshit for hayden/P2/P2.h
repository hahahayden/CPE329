/*
* P1.h
*
*  Created on: Apr 22, 2020
*      Author: Anthony
*/
#ifndef P2_H_
#define P2_H_

#include "keypad.h"
#include "LCD.h"
#include "msp.h"
#include<stdio.h>
#include<string.h>

//Declaring the different states
enum states{
  start,
  frequency_state,
  waveform_state,
  dutyCycle_state,
  output_state
}states ;

//Declaring the different events
enum events{
  start_event,
  frequency_event,
  waveform_event,
  dutyCycle_event,
  output_event
}events;

//Declaring the functions
void step_state(enum events event);
void squareOutput();
void sawtoothOutput();
void sineOutput();
void TA0_0_IRQHandler(void);
void TA0_N_IRQHandler(void);

#endif /* P2_H_ */
