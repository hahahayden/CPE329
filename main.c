#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    int counter=0;
    int i;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer



	    P1-> SEL1 &= ~BIT1;
	    P1-> SEL0 &= ~BIT1;

	    P2-> SEL1 &= ~BIT1;
	    P2-> SEL0 &= ~BIT1;
	    P1->DIR |= BIT0;
	    P2->DIR |= 0x07;
	    while(1)
	    {
	   switch (counter%3)
	       {
	                   case 0:
	                               P2->OUT|= BIT0;
	                               P2->OUT &= ~BIT1;
	                               P2->OUT &= ~BIT2;
	                               counter++;
	                               break;
	                   case 1:
                                   P2->OUT&= ~BIT0;
                                   P2->OUT |= BIT1;
                                   P2->OUT &= ~BIT2;
                                   counter++;
                                   break;

	                   case 2:
	                               P2->OUT&= ~BIT0;
	                               P2->OUT |= ~BIT1;
	                               P2->OUT &= BIT2;

                                   counter=0;
                                   break;
	       }


	    P1->OUT^=BIT0;
	    for (i=0; i<100000; i++);



        }
}




