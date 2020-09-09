//******************************************************************************
//  MSP432P401 Demo - ADC14, Sample A1, AVcc Ref, Set P1.0 if A1 > 0.5*AVcc
//
//   A single sample is made on A1 with reference to AVcc. Software sets
//   ADC14_CTL0_SC to start sample and conversion - ADC14_CTL0_SC automatically
//   cleared at EOC. Using MODCLK = 25 MHz for sample and conversion timing
//   Pulse mode with sample timing of 16 cycles. ADC14 interrupts on conversion
//   completion. If A1 > 0.5*AVcc, P1.0 set, else reset.
//
//
//                MSP432P401R
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|P5.4/A1      P1.0|-->LED
//
//******************************************************************************
#include "msp.h"
#include "math.h"
int ADCresult;
int interruptFlag=0;
uint32_t calibratedData;
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



int main(void) {
    volatile uint32_t i;
    init_UART0();
    WDT_A->CTL = WDT_A_CTL_PW |       // Stop WDT
                 WDT_A_CTL_HOLD;

    // GPIO Setup
    P1->OUT &= ~BIT0;                 // Clear LED to start
    P1->DIR |= BIT0;                  // Set P1.0/LED to output
    P5->SEL1 |= BIT4;                 // Configure P5.4 for ADC
    P5->SEL0 |= BIT4;

    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 =ADC14_CTL0_SHP | ADC14_CTL0_ON|ADC14_CTL0_SHT0_0;  //16 CLK cycles
    ADC14->CTL1 = ADC14_CTL1_RES_3;     // Use sampling timer, 14-bit conversion

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1; // Pin 5.4 -> A1 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;        // Enable ADC conv complete interrupt
    ADC14->CTL0 |= ADC14_CTL0_ENC;        // Enable conversions


    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Enable global interrupt
    __enable_irq();

    while (1)
    {
        for (i = 20000; i > 0; i--);        // Delay allows for fulfillment of sampling

        // Start sampling/conversion

        ADC14->CTL0 |= ADC14_CTL0_SC;

        if (interruptFlag==1){
            //ADCresult=ADC14->MEM[0];
            calibratedData=(ADCresult*200.7-224070); //in uV
            interruptFlag=0;
            UART_transmit(calibratedData);
        }
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    ADCresult=ADC14->MEM[0];
    interruptFlag=1;
    //UART_transmit(ADCresult);
    //printf("%d\n", ADCresult);

    /*  if (ADC14->MEM[0] >= 0x2000)      // ADC12MEM0 = A1 > 0.5AVcc?
      P1->OUT |= BIT0;                // P1.0 = 1
    else
      P1->OUT &= ~BIT0;               // P1.0 = 0
*/
}


