#include "msp.h"
#include "DAC.h"
char data[3];
int flag;
char receivedMSPChar;
int count=0;
int dataCalculate;
int string_to_int(char string[])
{
    int i = 0;
    int intOut = 0;
    for (i = 0; i < count; i++)
    {
        if (string[i] != '\0')
        {
            intOut *= 10;
            intOut += string[i] - 48;
        }
        else
            break;

    }
    printf("Data:%d\n",intOut);
    return intOut;
}


int main(void){
    startDAC();
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

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



    EUSCI_A0->BRW = 1;                      // Using baud rate calculator
    EUSCI_A0->MCTLW = (10 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    receivedMSPChar=0;

    while(1){ // do nothing

        if(flag)
        {
            if ((receivedMSPChar>47 && receivedMSPChar<58)){
                    data[count]= receivedMSPChar;
                    count+=1;
                    if (count>4){  //check if this works
                              printf("Too many Characters\n");
                              break;
                          }

                    EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

                    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

                }
            else if (receivedMSPChar==13){

                int a=0;

                 //for(a=0; a<count; a++){
                   //  printf("%d", data[a]);
                // }

               dataCalculate= string_to_int(data);
               if (dataCalculate<4096){
                 dataDAC(dataCalculate);
                 count=0;
            }

        }
            flag=0;

    }


}
}
// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)          // if interrupt flag is interrrupted due to receive
    {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        receivedMSPChar= EUSCI_A0->RXBUF;

        EUSCI_A0->IFG&=~EUSCI_A_IFG_RXIFG;
        flag = 1;


    }
}
