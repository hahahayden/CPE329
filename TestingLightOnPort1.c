#include "msp.h"
void delay_ms (int ms);

/**
 * main.c
 */
int main(void)
{
	int i;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	P1->SEL0 &= ~BIT0;
	P1->SEL1 &= ~BIT0;
	P1->DIR |= BIT0;

	while (1)
	{
		P1-> OUT|= BIT0;

	}
}
