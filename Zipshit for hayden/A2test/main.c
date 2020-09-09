/**
 * main.c
 */
#include "msp.h"
#include "delay.h"


void main(void)
    {
    //setting integer values for the main function
    int i=0;
    int LEDcounter=0;
    int counter=0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
        //Port to start designating LED Blink (I need Pin 1.0 and 2.0,2.1,2.2 for LED blinking)
        P1->SEL0 &= ~BIT0;  //designate P1.0 as a GPIO
        P1->SEL1 &= ~BIT0;  //designate P1.0 as a GPIO
        P1->DIR|=BIT0;      // select P1.0 into a output

        P2-> SEL1 &= (~BIT2)|(~BIT1)|(~BIT0);  //designate P2.0-2.2 to GPIO
        P2-> SEL0 &= (~BIT2)|(~BIT1)|(~BIT0);  //designate P2.0-2.2 to GPIO
        P2->DIR |= 0x07; //setting direction of RGB leds to outputs

        P1->OUT&=0x00; // setting the output of P1.0 to 0

        P1-> SEL0 &= ~BIT5; //designate P1.5 as a GPIO
        P1->SEL1 &= ~BIT5; //designate P1.5 as a GPIO
        P1-> DIR|= BIT5; //setting direction of P1.5
        P1->OUT|=BIT5;    // turn on output P1.5 and the previous turned on 1.0

        P4-> SEL0 |=BIT3; //designate P4.3 as a GPIO
        P4->SEL1 &= ~BIT3;//designate P4.3 as a GPIO
        P4-> DIR|= BIT3; //select pin 4.3 as output
        while(1)
        {
            // LED code
            P1->OUT&=~BIT5; //toggle pin 1.5
            switch (LEDcounter%3)//defining the different cases for RGB leds
                {
                    case 0:
                                P2->OUT|= BIT0; //setting P2.0 high
                                P2->OUT &= ~BIT1;//setting P2.1 low
                                P2->OUT &= ~BIT2;//setting P2.2 low
                                LEDcounter++;// increment counter up by 1
                                break;
                    case 1:
                                P2->OUT&= ~BIT0;//setting P2.0 low
                                P2->OUT |= BIT1;//setting P2.1 high
                                P2->OUT &= ~BIT2;//setting P2.2 low
                                LEDcounter++;//increment counter up by 1
                                break;
                    case 2:
                                P2->OUT&= ~BIT0;//setting P2.0 low
                                P2->OUT |= ~BIT1;//setting P2.1 low
                                P2->OUT &= BIT2;//setting P2.2 high
                                LEDcounter=0;//increment counter up by 1
                                break;
                  }
            P1->OUT^=BIT0;                          // allows P1.0 (LED) to toggle between on and off

            for (i=0; i<100000; i++);


            /*set_DCO(6000000); used to set the MCLK frequency for Part 2*/

            P1->OUT&=~BIT5;                        // start the bit from 0 so that we are able to determine the start of the delay on the scope
            counter= delay_us(48,3000000);         // call the delay function ( time is in us and frequency is in Hz)
            //printf("%d", counter);

            //executing the delay
            for (i=counter; i>0; i--)               // create a for loop with the appropriate number of iterations to create a delay approximate to the input
            {
                P1->OUT^=BIT5;                      // toggle the P1.5 to allow us to see each on and off; allows us to see where it truly ends
            }
            P1->OUT&=~BIT5;                          // turn off the bit to see where the delay ends (still need to see if the counter is even or odd to further determine if a low was produced so we don't undercount the number of highs and lows)
            for (counter=1000; counter>0; counter--){}  // needed to create some delay between our toggling


            }
}
