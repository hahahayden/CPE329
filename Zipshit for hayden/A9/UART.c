
/*
* UART.c
*
*  Created on: May 25, 2020
*      Author: Anthony
*/

#include "msp.h"
#include "math.h"

//This function initializes the UART of the MSP
void init_UART0() {

   // Configure UART
   EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
   EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
           EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK

   // Baud Rate calculation
   // 3000000/(115200) = 26.041667
   // Fractional portion = 0.041667
   // User's Guide Table 21-4: UCBRSx = 0x00
   // UCBRx = int (26.041667 / 16) = 1
   // UCBRFx = int (((26.041667/16)-1)*16) = 10

   P1->SEL0 |= 0x0C;                       /* UART TX & RX: P1.3, P1.2     */
   P1->SEL1 &= ~0x0C;
   EUSCI_A0->BRW = 1;                      // Using baud rate calculator
   EUSCI_A0->MCTLW = (10 << EUSCI_A_MCTLW_BRF_OFS) |
           EUSCI_A_MCTLW_OS16;
   EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
}

//This function outputs
void UART_transmit(uint32_t data) {

   /* 1 place                                                              */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 10000000) / 1000000) + 48;

   /* Decimal point                                                        */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = '.';

   /* 0.1 place                                                            */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 1000000) / 100000) + 48;

   /* 0.01 place                                                           */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 100000) / 10000) + 48;

   /* 0.001 place                                                          */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

   /* 0.0001 place                                                         */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

   /* 0.00001 place                                                        */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 1000) / 100) + 48;

   /* 0.000001 place                                                       */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 100) / 10) + 48;

   /* 0.0000001 place                                                      */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = ((data % 10) / 1) + 48;

   /* newline                                                              */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = '\n';

   /* carriage return                                                      */
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
   EUSCI_A0->TXBUF = '\r';
}
