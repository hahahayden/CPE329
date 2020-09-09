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
#include "delay.h"

#define GAIN BIT5
#define SHDN BIT4
#define DAC_CS  BIT0

int main(void)
{
    uint16_t data;
    uint8_t hiByte, loByte;
    uint32_t i;

    set_DCO(FREQ_24_Mhz);

    WDT_A->CTL = WDT_A_CTL_PW |         // Stop watchdog timer
            WDT_A_CTL_HOLD;

    P1->SEL0 |= BIT5 | BIT6 | BIT7;     // Set P1.5, P1.6, and P1.7 as
                                        // SPI pins functionality

    P6->DIR |= DAC_CS;                      // set as output for CS

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
                      EUSCI_B_CTLW0_MST      |  // Set as SPI master
                      EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
                      EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
                      EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
                      EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->BRW = 0x02;              // div by 2 fBitClock = fBRCLK / UCBRx

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Initialize USCI state machine

    while(1)
    {
        // write numbers 0-7 to SPI. Use TXIFG to verify TXBUF is empty
        for(data=0; data<4096; data++)
        {
          // set the low and high bytes of data
          loByte = 0xFF & data;         // mask just low 8 bits
          hiByte = 0x0F & (data >> 8);  // shift and mask bits for D11-D8
          hiByte |= (GAIN | SHDN);      // set the gain / shutdown control bits

          P6->OUT &= ~DAC_CS; // Set CS low

          // wait for TXBUF to be empty before writing high byte
          while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
          EUSCI_B0->TXBUF = hiByte;

          // wait for TXBUF to be empty before writing low byte
          while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
          EUSCI_B0->TXBUF = loByte;

          // wait for RXBUF to be empty before changing CS
          while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));

          P6->OUT |= DAC_CS; // Set CS high
        }
    }
}




/*
*
*
*  ACLK = TACLK = 32kHz, MCLK = SMCLK = default DCO ~3MHz
*
*  TIMER_A0 utilize ACLK - TA0CLK = 32768Hz
*  TIMER_A0 CCR0 and CCR1 to create 2 timing events
*  TIMER_A0 -> CCR[0] = 32768 = 0.5 Hz
*  TIMER_A0 -> CCR[1] = 16384 = 0.5 Hz with 0.5s offset from CCR0
*/
/*
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
   P4-> SEL0 |=BIT3;          // designate P4.3 as a MCLK signal (SEL0=1, SEL1 = 0, Dir= 1)
   P4->SEL1 &= ~BIT3;
   P4-> DIR|= BIT3;

   // setup LEDs
   P4->DIR |= (BIT1|BIT0);
   P4->OUT &= ~(BIT1|BIT0);   // turn off at first

   // setup TIMER_A0
   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
   TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
   TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

   TIMER_A0->CCR[0] = 480-1;   // set CCR0 count

   TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // SMCLK, UP mode

   NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

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
*/
