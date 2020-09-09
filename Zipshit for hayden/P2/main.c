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
#include "DAC.h"
#include"math.h"
#include "LCD.h"
#include "keypad.h"
#include "P2.h"
#include "delay.h"

int main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // Stop watchdog timer

    keypad_init();  // setup gpio pins for keypad
    LCD_init();
    startDAC();

    int i=0;
    set_DCO(FREQ_24_Mhz);
    Write_string_LCD("Frequency (1-5)");
    delay_us(1000000);
    LCD_command(0xC0);

    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1; // SMCLK, UP mode, with ID to have no clock divider

    TIMER_A0->EX0 = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   // TA0_0 and TA0_N
    __enable_irq();     // Enable global interrupt

    states=start;
    events=start_event;

    while(1){
        step_state(events);
    }
}
