/*
 * DAC.c
 *
 *  Created on: May 7, 2020
 *      Author: Hayden
 */

#include "msp.h"
#include <stdint.h>
#define GAIN BIT5
#define SHDN BIT4
#define DAC_CS  BIT7    //BIT4

void startDAC() {
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
                      EUSCI_B_CTLW0_MST      |  // Set as SPI master
                      EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
                      EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
                      EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
                      EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->BRW = 0x02; // divider for keeping SMCLK full speed

    P1->SEL0 |= BIT5 | BIT6;
    P1->SEL1 &= ~(BIT5 | BIT6);
    P1->DIR |= BIT7;//P4BIT4

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void dataDAC(uint16_t data){
    uint8_t hiByte, loByte;
    loByte = 0xFF & data;         // mask just low 8 bits
    hiByte = 0x0F & (data >> 8);  // shift and mask bits for D11-D8
    hiByte |= (GAIN | SHDN);      // set the gain / shutdown control bits

    P1->OUT &= ~DAC_CS; // Set CS low

    // wait for TXBUF to be empty before writing high byte
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = hiByte;

    // wait for TXBUF to be empty before writing low byte
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = loByte;

    // wait for RXBUF to be empty before changing CS
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));

    P1->OUT |= DAC_CS; // Set CS high
}


