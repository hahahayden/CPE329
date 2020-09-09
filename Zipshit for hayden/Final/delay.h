/*
 * delay.h
 *
 *  Created on: Apr 10, 2020
 *      Author: Hayden
 */

#ifndef DELAY_H_
#define DELAY_H_

//declaring the values with their respective frequencies
#define FREQ_1_5_Mhz 1500000
#define FREQ_3_Mhz 3000000
#define FREQ_6_Mhz 6000000
#define FREQ_12_Mhz 12000000
#define FREQ_24_Mhz 24000000
#define FREQ_48_Mhz 48000000

//declaring the functions to be used
void set_DCO(int freq);

#endif /* DELAY_H_ */
