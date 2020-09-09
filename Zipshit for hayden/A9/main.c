//******************************************************************************
//  MSP432P401 Demo - ADC14, Sample A1, AVcc Ref, Set P1.0 if A1 > 0.5*AVcc
//
//   A single sample is made on A1 with reference to AVcc. Software sets
//   ADC14_CTL0_SC to start sample and conversion - ADC14_CTL0_SC automatically
//   cleared at EOC. Using MODCLK = 25 MHz for sample and conversion timing
//   Pulse mode with sample timing of 16 cycles. ADC14 interrupts on conversion
//   completion. If A1 > 0.5*AVcc, P1.0 set, else reset.
//
//
//                MSP432P401R
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|P5.4/A1      P1.0|-->LED
//
//******************************************************************************

#include "msp.h"
#include "math.h"
#include "UART.h"

int ADCresult;
int interruptFlag=0;
float calibratedData;

int main(void) {

   volatile uint32_t i;
   init_UART0();
   WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop WDT

   // GPIO Setup
   P1->OUT &= ~BIT0;                 // Clear LED to start
   P1->DIR |= BIT0;                  // Set P1.0/LED to output
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

   while (1){
       for (i = 20000; i > 0; i--);        // Delay
       // Start sampling/conversion
       ADC14->CTL0 |= ADC14_CTL0_SC;

       //When the interrupt flag is 1, output the uV to the terminal
       if (interruptFlag==1){
           calibratedData=(ADCresult*201.19-2711); //in uV
           interruptFlag=0;
           UART_transmit(calibratedData);
       }
   }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {

   ADCresult=ADC14->MEM[0];
   interruptFlag=1;
}
