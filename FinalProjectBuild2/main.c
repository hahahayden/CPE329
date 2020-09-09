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
#include "Blu.h"


int dutyCycleTotal;
double frequency;
int flag;
int sendtoBlu;
double ADCresult;
int interruptFlag=0;
uint32_t calibratedData;
int PWMflag;
int twoCount=0;
int selectFlag=0;
int PWMBLU=0;
char receivedMSPChar;
int printStartCount=0;
int soundflag=1;
int sawtoothSelect=0;
int squareSelect=1;
int increment=0;
int n=0;

void sawtoothOutputSound(float frequencySawtooth){
   // printf("%3.2f\n",frequencySawtooth);

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
     TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
     TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

     // SMCLK, UP mode, with ID to have no clock divider
     TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_MC_1;
     TIMER_A0->EX0 = 0;

     NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

     __enable_irq();     // Enable global interrupt

    double periodDelay=1/frequencySawtooth;
      int timer1= periodDelay*3000000;
      TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;
      TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
      TIMER_A0->CCR[0] = 236-1;   // set CCR0 count   40 us
      double stepLength= 1/(100000); //8 us
      int points = 100000/frequencySawtooth;
      int totalPoints=4096;
      increment=(totalPoints/points);

}

void main(void){
  //  sawtoothOutputSound(365.5);
    double frequencyTF=200.303;
    printf("%3.2lf\n",frequencyTF);
}

    volatile uint32_t i;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    init_Bluetooth();
    init_ADC();
    startDAC();
    //init_UART0();
    init_Sound();

    //GPIO  P4.0 is PWM to EN on teh board
    P4->SEL1 &= ~BIT0;
    P4->SEL0 &= ~BIT0;
    P4->DIR |= BIT0;
    P4->OUT |= BIT0;
    //1.5 is IN1 when motor works change to just a regular GPIO

    /*P1->SEL1 &= ~BIT5;
    P1->SEL0 &= ~BIT5;
    P1->DIR|=BIT5;
    P1->OUT |= BIT5;// IN1 Held HIGH

    //1.6 is IN2   when motor works chagne to just a reuglar GPIO
    P1->SEL1 &= ~BIT6;
    P1->SEL0 &= ~BIT6;
    P1->DIR|=BIT6;
    P1->OUT &= ~BIT6;// IN2 Held LOW
*/
    dutyCycleTotal=40;
  //  frequency=50.0;

    //squareOutput(frequency,dutyCycleTotal);
    //int tempFan=0;
    //int data=0;
    //int sound=1;
    //init_Sound();
    flag=3;

    soundflag=0;
  //  sawtoothSelect=1;
    //squareSelect=0;

    while(1){



        // when press  0 everything is reset

        if (flag==0){
            if (printStartCount==0){
                printf("\n");
                printf("Select a mode:\n");
                printf("1=Temperature Sensing\n2=Fan Speed Control\n");//3=Sound DONT PICK THIS\n");
                printStartCount++;
            }
            twoCount=0;
            PWMflag=0;
            selectFlag=0;
            PWMBLU=0;
            soundflag=0;
        }
        else if (flag==1){
            //printf("here case 1\n");
            for (i = 200000; i > 0; i--);        // Delay allows for fulfillment of sampling
            // Start sampling/conversion
            ADC14->CTL0 |= ADC14_CTL0_SC;

            if (interruptFlag==1){
                //ADCresult=ADC14->MEM[0];
                //calibratedData=(ADCresult*1000.0 *0.003-224070); //in mV
                //calibratedData=(ADCresult*1000.0)/(3.5); // normalized the data value to room temperature about 25 degree C
                int PWMcontroller=0;
                calibratedData=ADCresult;
                //calibratedData=(ADCresult*1000)-0.3;
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
               PWMflag=1;
            }

            // text to send a number PWM
            //clear Receive buffer
            while(1){  // once the value is enetered
                if (PWMflag==1 && twoCount==2 &&  (sendtoBlu==108||109||104)){
                    // printf("Please press 0 to select another Mode Again\n");
                    if (sendtoBlu==104){
                        PWMBLU=100;
                    }
                    else if (sendtoBlu==108){
                        PWMBLU=50;
                    }
                    else if (sendtoBlu==109){
                        PWMBLU=75;
                    }
                    //PWMBLU=((sendtoBlu-48)*10);      // we will take the input you give and make it into duty cycle  3 will be a 30% duty cycle
                    squareOutput(50.0, PWMBLU);
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

    if (flag==3){


            if (soundflag==1){

                soundflag=0;
                   if (receivedMSPChar==49)
                   {
                       if (sawtoothSelect==1){
                           sawtoothOutputSound(466.16);
                       }
                       else if (squareSelect==1){
                      squareOutputSound(466.16,60);
                       }

                   }
                   else if (receivedMSPChar==50)
                        {
                       if (sawtoothSelect==1){
                           sawtoothOutputSound(523.25);
                          }

                       else if (squareSelect==1){
                            squareOutputSound(523.25,60);
                       }

                        }
                   else if (receivedMSPChar==51)
                          {

                       if (sawtoothSelect==1){
                      sawtoothOutputSound(587.33);
                  }

                       else if (squareSelect==1){
                              squareOutputSound(587.33,60);
                       }

                          }

                   else if (receivedMSPChar==52)
                          {
                       if (sawtoothSelect==1){
                                            sawtoothOutputSound(622.25);
                                        }

                       else if (squareSelect==1){
                              squareOutputSound(622.25,60);
                       }

                          }
                   else if (receivedMSPChar==53)
                             {
                       if (sawtoothSelect==1){
                    sawtoothOutputSound(698.46);
                       }

                       else if (squareSelect==1){
                                 squareOutputSound(698.46,60);
                       }

                             }
                   else if (receivedMSPChar==54)
                               {
                       if (sawtoothSelect==1){
                                      sawtoothOutputSound(783.99);
                                                                               }


                       else if (squareSelect==1){
                                   squareOutputSound(783.99,60);
                       }

                               }

                   else if (receivedMSPChar==55)
                                  {

                       if (sawtoothSelect==1){
                                                      sawtoothOutputSound(880.00);
                       }
                       else if (squareSelect==1){
                                      squareOutputSound(880.00,60);
                       }

                                  }
                   else if (receivedMSPChar==56)
                                   {

                       if (sawtoothSelect==1){
                                                      sawtoothOutputSound(932.33);
                       }
                       else if (squareSelect==1){


                                       squareOutputSound(932.33,60);
                       }

                                   }
                   else if (receivedMSPChar==57)
                                   {
                       if (sawtoothSelect==1){
                                                      sawtoothOutputSound(1046.50);
                       }

                       else if (squareSelect==1){

                                       squareOutputSound(1046.50,60);
                       }

                                   }
                   else if (receivedMSPChar==43)
                                   {

                                       if (sawtoothSelect==1){
                                                                      sawtoothOutputSound(1147.66);
                                       }
                                       else if (squareSelect==1){

                                       squareOutputSound(1174.66,60);
                                       }

                                   }
                   else if (receivedMSPChar==43)
                                                         {

                       if (sawtoothSelect==1){
                                                      sawtoothOutputSound(1174.66);
                       }
                       else if (squareSelect==1){
                           squareOutputSound(1174.66,60);
                       }

                                                         }

                   else if (receivedMSPChar==115) //sawtooth s
                   {

                       sawtoothSelect=1;
                       squareSelect=0;
                   }
                   else if (receivedMSPChar==114)  //square output r on keyboard for rectrangle
                                    {

                                        squareSelect=1;
                                        sawtoothSelect=0;
                                    }
                   }

        }
    }

}

//Bluetooth
void EUSCIA2_IRQHandler(void){
    // Send data to speaker
    if (EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG){
        //printf("here0\n");
        //send over to the console the number pressed by bluetooth
        sendtoBlu= EUSCI_A2->RXBUF;
        if (PWMflag==1){
            printf("\n");
            printf("\n");
            printf("\n");
            printf("You selected your fan speed\nYou can select:\n0 to reset\n2 for fan speed control\n");//3 for ...\n");
            flag=2;
            twoCount++;
        }
        else if (sendtoBlu==51 && PWMflag==0){

                  printf("\n");
                  printf("\n");
                  printf("\n");
                  printf("You selected bluetooth sound\n");//3 for ...\n");
                  flag=3;
                  twoCount=0;
              }
        else if (sendtoBlu==48 && PWMflag==0){
            printf("\n");
            printf("\n");
            printf("\n");
            printf("You selected 0 to reset\n");//sdjkfhlaskdjfhaskdjfhlaskjdhfkajshflakjshdflkajshdflajksdhflkjasdfl
            printf("\n");
            printStartCount=0;
            flag=0;
        }
        else if (sendtoBlu==49 &&PWMflag==0){
            flag=1;
            printf("\n");
            printf("\n");
            printf("\n");
            printf("You selected 1 for temperature sensing\nYou can select:\n0 to reset\n");//\n1 for temperature sensing\n2 for fan speed control\n3 for ...\n");
        }
        else if (sendtoBlu==50 && PWMflag==0){
            printf("\n");
            printf("\n");
            printf("\n");
            printf("You selected 2 for fan speed\nYou can select l=low, m=medium, h=high\n");
            flag=2;
            twoCount=0;
            twoCount++;
        }
        //else if (sendtoBlu==51 && PWMflag==0){
          //  printf("\n");
          // printf("\n");
           // printf("\n");
          //  printf("You selected 3 for sound\nYou can select...\n");
           // flag=3;
        //}
        //EUSCI_A2->TXBUF=sendtoBlu;
        //printf("%d\n", sendtoBlu);
        //while (!(EUSCI_A2->IFG & EUSCI_B_IFG_RXIFG));
        //sendtoBlu = EUSCI_A2->RXBUF;
        EUSCI_A2->IFG &= ~EUSCI_B_IFG_RXIFG;
    }
}

// Timer A0_0 interrupt service routine
//Square Wave PWM

void TA0_0_IRQHandler(void) {
   /* if (soundSet==1 ){
            soundflag=1;
           TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
           dataDAC(4095);
       }
    else{
    */
    soundflag=0;
    if (flag==3){

        if (squareSelect==1){

        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
      dataDAC(4095);     //Bit0
        //P4->OUT |= BIT0;     //Bit0
        }
        else if (sawtoothSelect==1){

            if (n+3>4096){
                n=0;
                dataDAC(0);
            }
            else{
                n=n+increment;
                dataDAC(n);

            }
        }

    }
    else {
   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
   P4->OUT |= BIT0;     //Bit0
    }
}


// Timer A0_N interrupt service routine for CCR1 - CCR4
void TA0_N_IRQHandler(void){
   /* if(TIMER_A0->CCTL[1]&TIMER_A_CCTLN_CCIFG){   // check for CCR1 interrupt
        if (soundset==1)
        {
            soundflag=1;
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
                 dataDAC(0);
        }
        else{
        */

    if(TIMER_A0->CCTL[1]&TIMER_A_CCTLN_CCIFG){
        soundflag=0;
        if (flag==3 && squareSelect==1){

            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR1 interrupt
                 dataDAC(0);                         //switch the output to see on the OSCOPE^=Bit0
          //  P4->OUT &= ~BIT0;
        }
        else if (flag==3 && sawtoothSelect==1)
        {

            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
        }
        else{
            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR1 interrupt
            P4->OUT &= ~BIT0;                         //switch the output to see on the OSCOPE^=Bit0

        }

}
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {

    ADCresult=ADC14->MEM[0];
    interruptFlag=1;


    //UART_transmit(ADCresult);
    //printf("%d\n", ADCresult);
    //  if (ADC14->MEM[0] >= 0x2000)      // ADC12MEM0 = A1 > 0.5AVcc?
    // P1->OUT |= BIT0;                // P1.0 = 1
    // else
    // P1->OUT &= ~BIT0;               // P1.0 = 0
}

// UART interrupt service routine
void EUSCIA0_IRQHandler(void){
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){          // if interrupt flag is interrrupted due to receive
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        receivedMSPChar= EUSCI_A0->RXBUF;
     if (flag==3){
            EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

                     while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                     soundflag = 1;
     }
        EUSCI_A0->IFG&=~EUSCI_A_IFG_RXIFG;

    }
}

void UART_transmit(uint32_t data){
    // 1 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000000) / 1000000) + 48;

    // Decimal point
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '.';

    // 0.1 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 1000000) / 100000) + 48;

    // 0.01 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 100000) / 10000) + 48;

    // 0.001 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

    // 0.0001 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10000) / 1000) + 48;

    // 0.00001 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 1000) / 100) + 48;

    // 0.000001 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 100) / 10) + 48;

    // 0.0000001 place
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = ((data % 10) / 1) + 48;

    //* newline
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '\n';
    // carriage return
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '\r';
}

