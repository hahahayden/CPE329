/*
 *  Example using SPI to connect to MCP 4921
 *  P1.4  Port1.4   CS
 *  P1.5  UCB0CLK   SCK
 *  P1.6  UCB0SIMO  SDI
 *
 *  LDAC - ground to always set low, no buffering
 *  MISO / SIMO is not needed because no data coming from DAC
 *
 *  Write values 0 to 4095 to
 */


#include "msp.h"
#include <stdint.h>
#include "DAC.h"


int time=1;




int main(void){

    WDT_A->CTL = WDT_A_CTL_PW |         // Stop watchdog timer
            WDT_A_CTL_HOLD;


    startDAC();

    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled


    TIMER_A0->CCR[0] = 30000;   // set CCR0 count   10 ms

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1; // SMCLK, UP mode, with ID to have no clock divider

    TIMER_A0->EX0 = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

    __enable_irq();     // Enable global interrupt
    dataDAC(0);
    int data=0;

    while(1){
        /*
        if (time==2){          //Create a Square Pulse, outside of the ISR
            dataDAC(0);
                 time=0;
        }
        else if(time==1) {
             dataDAC(2493);

         }
        else if (time==0){
            dataDAC(0);
        }
*/
        //For Triangle Wave Part B

        if (time==1)   // rising
        {
            if (data < 2482)    // 2V max
            {
                data=data+10;
                dataDAC(data);
            }
        }
        else      // falling
        {
            if (data > 0)
            {

                data=data-10;
                dataDAC(data);
            }

        }



    }
}

// Timer A0_0 interrupt service routine
void TA0_0_IRQHandler(void){
   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
   time^=1;

}
