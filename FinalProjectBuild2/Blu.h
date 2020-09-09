/*
 * Blu.h
 *
 *  Created on: Jun 3, 2020
 *      Author: Anthony
 */

#ifndef BLU_H_
#define BLU_H_

int string_to_int(char string[]);
void init_Bluetooth();
void init_Sound();
void init_UART0();
void init_ADC();
void determinePWM(uint32_t calibratedData);
void UART_transmit(uint32_t data);

#endif /* BLU_H_ */
