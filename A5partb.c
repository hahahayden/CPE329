/*
 *  TIMER_A Demo program
 *
 *  ACLK = TACLK = 32kHz, MCLK = SMCLK = default DCO ~3MHz
 *
 *  TIMER_A0 utilize ACLK - TA0CLK = 32768Hz
 *  TIMER_A0 CCR0 and CCR1 to create 2 timing events
 *  TIMER_A0 -> CCR[0] = 32768 = 0.5 Hz
 *  TIMER_A0 -> CCR[1] = 16384 = 0.5 Hz with 0.5s offset from CCR0
 */

#include "msp.h"
#include "delay.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    set_DCO(FREQ_24_Mhz);
     //GPIO
    P4->SEL1&= ~(BIT1|BIT0);   //P4.0 use for output 4.1 for ISR
    P4->SEL0 &= ~(BIT1|BIT0);
    //bring out MCLK

    P4-> SEL0 |=BIT3;                               // designate P4.3 as a MCLK signal (SEL0=1, SEL1 = 0, Dir= 1)
        P4->SEL1 &= ~BIT3;
        P4-> DIR|= BIT3;
       // setup LEDs
    P4->DIR |= (BIT1|BIT0);


    P4->OUT &= ~(BIT1|BIT0);   // turn off at first

    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
  //  TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE; // TACCR1 interrupt enabled

    TIMER_A0->CCR[0] = 480-1;   // set CCR0 count   40 us
    //TIMER_A0->CCR[1] = 240-1;   // set CCR1 count   10 us

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // SMCLK, UP mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt
   // NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   // TA0_0 and TA0_N

    __enable_irq();     // Enable global interrupt

    while(1);

}
// Timer A0_0 interrupt service routine
void TA0_0_IRQHandler(void) {
    P4->OUT&= ~BIT1;   // make the output 0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
    P4->OUT ^= BIT0;
    P4->OUT|=BIT1;    // make it 1  so the time between 0 and 1 is the ISR time
}

// Timer A0_N interrupt service routine for CCR1 - CCR4
/*
void TA0_N_IRQHandler(void)
{
    if(TIMER_A0->CCTL[1]&TIMER_A_CCTLN_CCIFG)   // check for CCR1 interrupt
    {
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR1 interrupt
        P4->OUT ^= BIT0;
    }
}
*/
