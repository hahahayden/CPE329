/*
 * Blu.c
 *
 *  Created on: Jun 3, 2020
 *      Author: Anthony
 */

#include "msp.h"

int count=0;
int flag;
uint32_t calibratedData;
int PWMcontroller=0;
int dutyCycleTotal;
char data[3];

//Used for printing characters
int string_to_int(char string[]){
    int i = 0;
    int intOut = 0;
    for (i = 0; i < count; i++){
        if (string[i] != '\0'){
            intOut *= 10;
            intOut += string[i] - 48;
        }
        else
            break;
    }
    printf("Data:%d\n",intOut);
    return intOut;
}

//Initialize the bluetooth module
void init_Bluetooth(){
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
}

//Initialize the speaker
void init_Sound(){
    startDAC();
    init_UART0();
}

//For Temperature Sensor Monitoring
void init_UART0() {
    // Configure UART

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

}

//Initialize the temp sensor
void init_ADC(){

    init_UART0();
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
}

//Set the fan speed for the temp sensing
void determinePWM(uint32_t calibratedData){
    PWMcontroller=0;
    if (PWMcontroller!=dutyCycleTotal){
        if (calibratedData<=7900){
            dutyCycleTotal=100;
            PWMcontroller=dutyCycleTotal;
        }
        else if (calibratedData>=9000){
            dutyCycleTotal=50;
            PWMcontroller=dutyCycleTotal;
        }
        else{
            dutyCycleTotal=75;
            PWMcontroller=dutyCycleTotal;
        }
        squareOutput(300.0, dutyCycleTotal);
    }
}
