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
#include "math.h"
#include "MotorController.h"
#include "DAC.h"
int dutyCycleTotal;
double frequency;
char receivedMSPChar;
int flag;
char data[3];
int sendtoBlu;
double ADCresult;
int interruptFlag=0;
int PWMcontroller=0;
int time=0;
int count=0;
int dataCalculate;
uint32_t calibratedData;
int PWMflag;
int twoCount=0;
int selectFlag=0;
int PWMBLU=0;
int string_to_int(char string[])
{
    int i = 0;
    int intOut = 0;
    for (i = 0; i < count; i++)
    {
        if (string[i] != '\0')
        {
            intOut *= 10;
            intOut += string[i] - 48;
        }
        else
            break;

    }
    printf("Data:%d\n",intOut);
    return intOut;
}

void init_Bluetooth(){
    // Configure UART pins
     P3->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

     // Configure UART
     EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST;       // Put eUSCI in reset
     EUSCI_A2->CTLW0 =  EUSCI_A_CTLW0_SWRST |      // Remain eUSCI in reset
                        EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK

     // Baud Rate calculation
     // 3000000/(9600) = 312.5
     // Fractional portion = 0.5
     // User's Guide Table 21-4: UCBRSx = 0x55
     // UCBRx = int (312.5 / 16) = 19
     // UCBRFx = int (((312.5/16)-19)*16) = 8

     EUSCI_A2->BRW = 19;                       // Using baud rate calculator
     EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
     EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
     EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;     // Clear eUSCI RX interrupt flag
     EUSCI_A2->IE |= EUSCI_A_IE_RXIE;         // Enable USCI_A0 RX interrupt

     // Enable global interrup
     __enable_irq();

     // Enable eUSCIA0 interrupt in NVIC module

     NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);


     flag = 0;
}


void init_Sound(){
    startDAC();
    init_UART0();
}

//For Temperature Sensor Monitoring
void init_UART0() {
    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK

    // Baud Rate calculation
    // 3000000/(115200) = 26.041667
    // Fractional portion = 0.041667
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRx = int (26.041667 / 16) = 1
    // UCBRFx = int (((26.041667/16)-1)*16) = 10


    P1->SEL0 |= 0x0C;                       /* UART TX & RX: P1.3, P1.2     */
    P1->SEL1 &= ~0x0C;

    EUSCI_A0->BRW = 1;                      // Using baud rate calculator
    EUSCI_A0->MCTLW = (10 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI

}
void UART_transmit(uint32_t data) {

    /* 1 place                                                              */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000000) / 1000000) + 48;

    /* Decimal point                                                        */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '.';

    /* 0.1 place                                                            */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 1000000) / 100000) + 48;

    /* 0.01 place                                                           */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 100000) / 10000) + 48;

    /* 0.001 place                                                          */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

    /* 0.0001 place                                                         */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

    /* 0.00001 place                                                        */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 1000) / 100) + 48;

    /* 0.000001 place                                                       */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 100) / 10) + 48;

    /* 0.0000001 place                                                      */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10) / 1) + 48;

    /* newline                                                              */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '\n';
    /* carriage return                                                      */
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '\r';
}

void init_ADC(){

    init_UART0();
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


}

void determinePWM(uint32_t calibratedData){
    if (PWMcontroller!=dutyCycleTotal){
        if (2.285<calibratedData< 2.365){
           dutyCycleTotal=50;
           PWMcontroller=dutyCycleTotal;
       }

       else if (2.065<calibratedData<=2.285){  // hotter
           dutyCycleTotal=75;
           PWMcontroller=dutyCycleTotal;
       }

       else if (2.5>calibratedData>= 2.365){
           dutyCycleTotal=25;
           PWMcontroller=dutyCycleTotal;
       }
       squareOutput(300.0, dutyCycleTotal);
    }

}


void main(void)
{
    volatile uint32_t i;
   WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
   init_Bluetooth();
   init_ADC();
   init_UART0();


   //GPIO  P4.0 is PWM to EN on teh board
   P4->SEL1 &= ~BIT0;
   P4->SEL0 &= ~BIT0;
   P4->DIR |= BIT0;
   P4->OUT |= BIT0;
   //1.5 is IN1 when motor works change to just a regular GPIO
   P1->SEL1 &= ~BIT5;
   P1->SEL0 &= ~BIT5;
   P1->DIR|=BIT5;
   P1->OUT |= BIT5;// IN1 Held HIGH

   //1.6 is IN2   when motor works chagne to just a reuglar GPIO
   P1->SEL1 &= ~BIT6;
     P1->SEL0 &= ~BIT6;
     P1->DIR|=BIT6;
     P1->OUT &= ~BIT6;// IN2 Held LOW

   dutyCycleTotal=40;
   frequency=300.0;

  // squareOutput(frequency,dutyCycleTotal);

   //int tempFan=0;
  //int data=0;

 //  int sound=1;

 // init_Sound();
   int printStartCount=0;
   while(1){
      // when press  0 everything is reset
       if (flag==0){

           //interruptFlag=0;
           if (printStartCount==0){
           printf("select a mode\n");
           printStartCount++;
           }
           twoCount=0;
           PWMflag=0;
           selectFlag=0;
           PWMBLU=0;
           squareOutput(300.0, 50);
       }
       else if (flag==1){


            for (i = 200000; i > 0; i--);        // Delay allows for fulfillment of sampling

                     // Start sampling/conversion

                     ADC14->CTL0 |= ADC14_CTL0_SC;

                     if (interruptFlag==1){
                         //ADCresult=ADC14->MEM[0];
                         //calibratedData=(ADCresult*1000.0 *0.003-224070); //in mV
                         calibratedData=(ADCresult*1000.0)/(3.5);   // normalized the data value to room temperature about 25 degree C
                         interruptFlag=0;
                         UART_transmit(calibratedData);
                         determinePWM(calibratedData);
                         selectFlag=1;
                     }

      }

      //MAKE A PLACE THAT RECEIVES DIFFERENT PWMS so when flag ==2, u want to prompt the phone to send more data in for PWM like wat we did in keypad

      //check if the PWM you enter from phone is the as what you see on oscope

      else if (flag==2){

          if (twoCount==1){    // once 2 is  selected display this
              printf("here\n");
              PWMflag=1;

              // printf("Please enter a number from 1-9  (break out of choosing PWM by pressing 0 twice): \n") ;
           }

        // text to send a number PWM
           //clear Receive buffer

          while(1){  // once the value is enetered

              if (PWMflag==1 && twoCount==2 &&  (sendtoBlu==48||49||50||51||52||53||54||55||56||57)){

                 //printf("Duty Cycle= %d\n", (sendtoBlu-48)*10);
                // printf("Please press 0 to select another Mode Again\n");
               PWMBLU=(100- ((sendtoBlu-48)*10));      // we will take the input you give and make it into duty cycle  3 will be a 30% duty cycle
                squareOutput(300.0, PWMBLU);
                PWMflag=0;
                selectFlag=2;
                printStartCount=0;
                twoCount=0;
                break;
                }
              else{
                  break;
              }
          }
      }

}
}

//Bluetooth
void EUSCIA2_IRQHandler(void)
{   // Send data to speaker
   if (EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)
   {
       //printf("here0\n");
       //send over to the console the number pressed by bluetooth

       sendtoBlu= EUSCI_A2->RXBUF;


      if (PWMflag==1){

              flag=2;
              twoCount++;
          }
       else if (sendtoBlu==48 && PWMflag==0){


           flag=0;
       }
       else if (sendtoBlu==49 &&PWMflag==0){

           flag=1;


       }


       else if (sendtoBlu==50 && PWMflag==0){

           flag=2;
           twoCount=0;
           twoCount++;
       }
       else if (sendtoBlu==51 && PWMflag==0){

           flag=3;
       }
       //EUSCI_A2->TXBUF=sendtoBlu;
       //printf("%d\n", sendtoBlu);

    //while (!(EUSCI_A2->IFG & EUSCI_B_IFG_RXIFG));
    //sendtoBlu = EUSCI_A2->RXBUF;
      EUSCI_A2->IFG &= ~EUSCI_B_IFG_RXIFG;
    }

}


   // UART interrupt service routine
   void EUSCIA0_IRQHandler(void)
   {
       if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)          // if interrupt flag is interrrupted due to receive
       {
           // Check if the TX buffer is empty first
           while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
           receivedMSPChar= EUSCI_A0->RXBUF;

           EUSCI_A0->IFG&=~EUSCI_A_IFG_RXIFG;
           flag = 1;


       }
   }
// Timer A0_0 interrupt service routine
//Square Wave PWM

void TA0_0_IRQHandler(void) {

   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

   P4->OUT ^= BIT0;
}

// Timer A0_N interrupt service routine for CCR1 - CCR4
void TA0_N_IRQHandler(void)
{

   if(TIMER_A0->CCTL[1]&TIMER_A_CCTLN_CCIFG)   // check for CCR1 interrupt
   {

       TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR1 interrupt

       P4->OUT ^= BIT0;                         //switch the output to see on the OSCOPE
   }
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    ADCresult=ADC14->MEM[0];
    interruptFlag=1;
    //UART_transmit(ADCresult);
    //printf("%d\n", ADCresult);

    /*  if (ADC14->MEM[0] >= 0x2000)      // ADC12MEM0 = A1 > 0.5AVcc?
      P1->OUT |= BIT0;                // P1.0 = 1
    else
      P1->OUT &= ~BIT0;               // P1.0 = 0
*/
}
// UART interrupt service routine
