/*
 * Bluetooth----------MSP
 * RXD----------------P3.3 (TXD on MSP)
 * TXD----------------P3.2 (RXD On MSP)
 * GND----------------GND
 * VCC----------------3.3V
 *
 *
 */

#include "msp.h"
#include "Bluetooth.h"

int sendtoBlu;//=76
int flag;
int main(void){



   // Stop watchdog timer
   WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

   // Configure UART pins
   P3->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

   // Configure UART
   EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Put eUSCI in reset
   EUSCI_A2->CTLW0 =  EUSCI_A_CTLW0_SWRST |      // Remain eUSCI in reset
                      EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK

   // Baud Rate calculation
   // 3000000/(9600) = 312.5
   // Fractional portion = 0.5
   // User's Guide Table 21-4: UCBRSx = 0x55
   // UCBRx = int (312.5 / 16) = 19
   // UCBRFx = int (((312.5/16)-19)*16) = 8

   EUSCI_A2->BRW = 19;                       // Using baud rate calculator
   EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
   EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
   EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;     // Clear eUSCI RX interrupt flag
   EUSCI_A2->IE |= EUSCI_A_IE_RXIE;         // Enable USCI_A0 RX interrupt

   // Enable global interrup
   __enable_irq();

   // Enable eUSCIA0 interrupt in NVIC module

   NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);


   flag = 0;
   //EUSCI_A2->TXBUF= 'h';
   initScreen();

   while(1){
       //In this while function, it constantly sends out the data to the DAC
       if(flag){
           printf("here1\n");
           flag=0;
       }
}
}
void EUSCIA2_IRQHandler(void)
{   // Send data to speaker
   if (EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)
   {
       //printf("here0\n");
       EUSCI_A2->TXBUF=EUSCI_A2->RXBUF;
       sendtoBlu= EUSCI_A2->RXBUF;
       printf("%d\n", sendtoBlu);
       if (sendtoBlu==49){
           printf("this a 1\n");
       }
       else if (sendtoBlu==50){
           printf("this a 2\n");
       }
       else if (sendtoBlu==51){
           printf("this a 3\n");
       }
       //EUSCI_A2->TXBUF=sendtoBlu;
       //printf("%d\n", sendtoBlu);

    //while (!(EUSCI_A2->IFG & EUSCI_B_IFG_RXIFG));
    //sendtoBlu = EUSCI_A2->RXBUF;
      EUSCI_A2->IFG &= ~EUSCI_B_IFG_RXIFG;
    }

}

/*
// UART interrupt service routine
void EUSCIA2_IRQHandler(void){
    printf("here0\n");
   if (EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG){     // if intrpt flag is intrpted due to receive
       // Check if the TX buffer is empty first
       while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
       //sendtoBlu= EUSCI_A2->RXBUF;
       EUSCI_A2->IFG&=~EUSCI_A_IFG_RXIFG;
       printf("here2\n");
       flag = 1;
   }
}
*/
