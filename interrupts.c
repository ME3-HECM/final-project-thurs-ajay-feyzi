#include <xc.h>
#include "interrupts.h"
#include "serial.h"
#include "i2c.h"
#include "color.h"
#include "LEDs.h"
#include "timers.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
    INTCONbits.IPEN = 1; // Interrupt Priority Levels: Enable
    //PIE0bits.TMR0IE = 1; //TMR0 Interrupt: Enable
    PIE5bits.TMR1IE = 1; //TMR1 Interrupt: Enable
    IPR5bits.TMR1IP = 1; //TMR1 Interrupt Priority: High
    
    
    //****Clicker Interrupt Initialisations
    TRISBbits.TRISB0 = 1; // Pin RB0: Input(1)
    ANSELBbits.ANSELB0 = 0; // Pin RB0: Digital input(0)
    
    PIE0bits.INT0IE = 1; //Interrupt on Pin RB0: Enable
    PIR0bits.INT0IF = 0; //Interrupt Flag: Off
    INTCONbits.INT0EDG = 0; // Interrupt on Falling Edge - Trigger below threshold
    IPR0bits.INT0IP = 1; // Interrupt Priority: High
    
    color_click_interrupt_init(); // Write interrupt configurations to clicker
    
    // Enable all interrupts
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1; 
}

/************************************
 * High priority interrupt service routine for:
 * TMR1 overflows(1s) (For Testing)
 * Serial transmission TX (For Testing)
 * Clicker Interrupt
 ************************************/
void __interrupt(high_priority) HighISR()
{    
    //Colour Clicker RGBC Clear Channel Interrupt
    if(PIR0bits.INT0IF){
        getTMR0_in_ms(); // Get movement duration
        
        //HeadLamp = !HeadLamp; // Testing
        color_click_interrupt_off(); // Turn off clicker interrupt(also clears it)
        wall_flag = 1; // Raise flag for main loop
        PIR0bits.INT0IF = 0; // Clear Interrupt Flag
    }

    // Interrupt for transmitting data- FOR TESTING
    if(PIR4bits.TX4IF){
        sendCharSerial4(getCharFromTxBuf()); // read buffer and send
        if(!isDataInTxBuf()) {
            PIE4bits.TX4IE = 0; //disable interrupt if buffer is empty
        }
    }
    
    // Timer 1 Interrupt - Triggers every second (almost)) - FOR TESTING
    if(PIR5bits.TMR1IF) {
            timer_flag = 1;
            TMR1H = 0;
            TMR1L = 0;
            PIR5bits.TMR1IF = 0; // clear flag
        }
}
