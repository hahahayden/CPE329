
#include "msp.h"
#include <stdint.h>
#include "delay.h"

//square output for the fan speed
void squareOutput(double frequency, int dutyCycleTotal){
    int timer2=0;
    //printf("Duty Cycle= %d\n", (sendtoBlu-48)*10);
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

    // SMCLK, UP mode, with ID to have no clock divider
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1;
    TIMER_A0->EX0 = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   // TA0_0 and TA0_N

    __enable_irq();     // Enable global interrupt

    //setting the timer2 values, these were listed to prevent bugs
    if (dutyCycleTotal==50){
        timer2=3750;
    }
    else if (dutyCycleTotal==75){
        timer2=5625;
    }
    else if (dutyCycleTotal==100){
        timer2=7499;
    }

    double periodDelay=1/frequency;
    double dutyCycleDelay=((100-dutyCycleTotal)/100.0)*periodDelay;

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE; // TACCR1 interrupt enabled
    TIMER_A0->EX0= TIMER_A_EX0_TAIDEX_7;
    TIMER_A0->CCR[0] = 7500-1;   // set CCR0 count//timer1
    TIMER_A0->CCR[1] = timer2-1;   // set CCR1 count//timer2
}

//square output for speaker
void squareOutputSound(double frequency, int dutyCycleTotal){
    // set_DCO(FREQ_24_Mhz);
    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

    // SMCLK, UP mode, with ID to have no clock divider
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1;
    TIMER_A0->EX0 = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   // TA0_0 and TA0_N

    __enable_irq();     // Enable global interrupt

    double periodDelay=1/frequency;
    double dutyCycleDelay=((100-dutyCycleTotal)/100.0)*periodDelay;

    int timer1= (periodDelay*3000000/(TIMER_A_EX0_TAIDEX_7+1));
    int timer2= (dutyCycleDelay*3000000/((TIMER_A_EX0_TAIDEX_7+1)));
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE; // TACCR1 interrupt enabled
    TIMER_A0->EX0= TIMER_A_EX0_TAIDEX_7;
    TIMER_A0->CCR[0] = timer1-1;   // set CCR0 count
    TIMER_A0->CCR[1] = timer2-1;   // set CCR1 count
}
