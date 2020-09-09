#include "delay.h"
#include "msp.h"


void set_DCO (int freq)
{

    CS->KEY= CS_KEY_VAL;                                //unlock the CS registers by taking the passkey macro defined in msp.h and writing to the CSKEY
    CS->CTL0 = 0;                                       // set the CTL0=0 to reset inputs; also allows for DCO to be source for MCLK, HSMCLK, or SMCLK

    //the following conditional statements picks the
    //correct frequency to output given user input

    if (freq==FREQ_24_Mhz)
    {
        CS->CTL0 |= CS_CTL0_DCORSEL_4;                   //setting register to output correct frequency (24MHz) with the macro defined in the header
    }

    else if (freq==FREQ_12_Mhz)
    {
         CS->CTL0 |= CS_CTL0_DCORSEL_3;                 //setting register to output correct frequency (12MHz)
    }

    else if (freq== FREQ_6_Mhz)
    {
        CS->CTL0 |= CS_CTL0_DCORSEL_2;                   //setting register to output correct frequency (6MHz)
    }

    else if (freq==FREQ_3_Mhz)
    {

        CS->CTL0 |= CS_CTL0_DCORSEL_1;                   //setting register to output correct frequency (3MHz)
    }

    else if (freq==FREQ_1_5_Mhz)
    {
        CS->CTL0 |= CS_CTL0_DCORSEL_0;                  //setting register to output correct frequency (1.5MHz)
    }



    if (freq==FREQ_48_Mhz)                              // 48 Mhz special case code given in lab manual
    {
        /*
        * Setting the Vcore to Level 1 for 48 MHz
        *      AM0_LDO --> AM1_LDO
        */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

        /*
        * Setting flash controller wait states for 48 MHz operation
        * Configure Flash wait-state to 1 for both banks 0 & 1
        */

        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
              ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & \
               ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

       /* Setting the DCO to 48 MHz operation */
       /* Configure the DCO to 48 MHz, ensure MCLK uses DCO as source */

       CS->CTL0 = 0;                                                          // Reset tuning parameters
       CS->KEY = CS_KEY_VAL ;                                                 // Unlock CS module for register access
       CS->CTL0 = CS_CTL0_DCORSEL_5;                                         // Set DCO to 48MHz

       /* Select MCLK = DCO, no divider */

       CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) |
               CS_CTL1_SELM_3;
       CS->KEY = 0;                                                          // Lock CS module from unintended accesses

    }

    else
    {

    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;            // set CTL1 register to allow for selecting the ACLK Source to be REFOCLK(32.768 kHz), and SMCLK, HSMCLK, and MCLK Source to be the DCOCLK frequency
    CS->KEY=0;                                                              // lock the CS registers back up

    }
}

int delay_us(int time, int freq)
{


    //assume DCO is supplying the MCLK frequency and the supplied frequency is valid for DCO; frequency given into function no need to check the register


    int clock;
    int freqMhz= freq/1000000;

    set_DCO(freq);                          // set the DCO frequency to the given frequency to set MCLK, to the given frequency

    if (freqMhz==1)                        // 1.5MHz condition, due to division it goes to 1
    {

        clock=time/16;


    }

    else if (freqMhz==3)                   // 3MHz condition; ratio found from a 10 iterative for loop
    {
        clock=time/8;
    }
    else if (freqMhz==6)                  // 6 MHz condition
    {
        clock = time/4;
    }
    else if (freqMhz==12)                 // 12 Mhz condition
    {
        clock =time/2;
    }
    else if (freqMhz==24)                  // 24 Mhz condition
    {
        clock =time;
    }
    else if (freqMhz==48){                // 48Mhz condition

        clock=2*time;
    }

    return clock;


}

