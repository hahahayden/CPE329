/*
*  Example using SPI to connect to MCP 4921
*  P4.4  Port4.4   CS
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
   WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // Stop watchdog timer

   startDAC();

   // setup TIMER_A0
   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
   TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt

   TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

   TIMER_A0->CCR[0] = 30000;   // set CCR0 count 10 ms

   // SMCLK, UP mode, with ID to have no clock divider
   TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1;

   TIMER_A0->EX0 = 0;    // Make TAIDEX=0

   NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

   __enable_irq();     // Enable global interrupt

   dataDAC(0);

   int data=0;

   while(1){
       if(time==1) {     // If the toggling bit is a HIGH produce a High
           dataDAC(2493);
       }
       else if (time==0){ // If its a 0 then lets produce a low
           dataDAC(0);
       }

       //For Triangle Wave Part B
       /*
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
       */
   }
}

// Timer A0_0 interrupt service routine
void TA0_0_IRQHandler(void){
  TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

  time^=1;                  // Toggle the time variable so we can execute in main()
}



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

/*
#include "msp.h"
#include <stdint.h>
#include "DAC.h"

uint16_t data;
uint8_t hiByte, loByte;

int time=1;

void setSquare(){
    if(time==2){
        dataDAC(0);
        time=0;
    }
    else if(time==1){
        dataDAC(2493);
    }
}

int main(void){

    WDT_A->CTL = WDT_A_CTL_PW |         // Stop watchdog timer
            WDT_A_CTL_HOLD;

    startDAC();

    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled


    TIMER_A0->CCR[0] = 30000;   // set CCR0 count   10 ms

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1; // SMCLK, UP mode

    TIMER_A0->EX0 = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

    __enable_irq();     // Enable global interrupt
    dataDAC(0);
    int data=0;
    while(1){
        if (time==1){   // rising
            if (data < 3723){    // 2V max
                data=data+15;
                dataDAC(data);
            }
        }
        else{      // falling
            if (data > 0){
                data=data-15;
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

*/



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
/*
#include "msp.h"
#include <stdint.h>
#include "A6.h"
#define GAIN BIT5
#define SHDN BIT4
#define DAC_CS  BIT0

int main(void)
{
    set_DCO(24000000);
    uint16_t data;
    uint8_t hiByte, loByte;
    uint32_t i;

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

          // delay before changing output voltage
          for (i=10000; i>0; i--);

        }
    }
}
*/
