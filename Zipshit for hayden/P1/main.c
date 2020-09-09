#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include "P1.h"


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;// stop watchdog timer

    keypad_init();  // setup gpio pins for keypad
    LCD_init();
    //Initializing the correct start state
    states=start;
    events=start_event;

    //Recurring while loop to stay in the states
    while(1){
        step_state(events);
    }
}
