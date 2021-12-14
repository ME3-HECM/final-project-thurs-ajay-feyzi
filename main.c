// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)
// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "i2c.h"
#include "color.h"
#include "LEDs.h"
#include "serial.h"
#include "timers.h"
#include "interrupts.h"
#include "CardMoves.h"
#include "buttons.h"
#include <stdio.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


void main(void){
    initUSART4();
    color_click_init();
    LEDsInit();
    Timer1_init();    
    //initDCmotorsPWM(199);
    buttonsInit();
    
    // COLOR CLICK RGB LEDs    
    LightToggle();

    interrupt_threshold_calibrate();
    
    // Initialize interrupt after threshold calibration
    Interrupts_init();
    
    color_click_interrupt_off();
    

    char color;
    // Set correct friction value for turns
    
    struct color_card currentCard;
    struct color_card * c = &currentCard;
    
    char buf1[] = {0x00};
    char buf2[] = {0x00};
    char buf3[] = {0x00};
    char buf4[] = {0x00};
    
    unsigned int raw_readings[4]; // for testing

    while(1){
        
        //Edit for Color
        
        if(timer_flag) { //1 second has passed 
            read_All_Colors(raw_readings); // read colors
            /*
            sprintf(buf1,"%d",red);
            TxBufferedString(" ");
            TxBufferedString("\n");
            //TxBufferedString("RED:   "); // writes string to buffer
            TxBufferedString(buf1);
            TxBufferedString("\n");
            
            sprintf(buf2,"%d",green);
            //TxBufferedString("GREEN: "); // writes string to buffer
            TxBufferedString(buf2);
            TxBufferedString("\n");
            
            sprintf(buf3,"%d",blue);
            //TxBufferedString("BLUE:  "); // writes string to buffer
            TxBufferedString(buf3);
            TxBufferedString("\n");
            */
            
            sprintf(buf2,"%d",int_high);
            //TxBufferedString("GREEN: "); // writes string to buffer
            TxBufferedString(buf2);
            TxBufferedString("\n");
            
            sprintf(buf3,"%d",currentCard.redPercentage);
            //TxBufferedString("BLUE:  "); // writes string to buffer
            TxBufferedString(buf3);
            TxBufferedString("\n");
            
            sprintf(buf4,"%d",raw_readings[0]);
            //TxBufferedString("CLEAR: "); // writes string to buffer
            TxBufferedString(buf4);
            TxBufferedString("\n");
            sendTxBuf(); //interrupt will handle the rest
            
            timer_flag =0;
         }
        
        
  
        if (!RightButton) {LightToggle();}                                  // turn RGB light off manually if required
        if (!LeftButton) {color_click_interrupt_init();BrakeLight=0;}       // turn on interrupt source manually for testing
            
        if (test_flag){
            LED1 = 0;
            //color = decide_color();
            color = decide_color(&currentCard);
            for (int i=0;i<color;i++){
                LED1 = 1;
                __delay_ms(350);
                LED1 = 0;
                __delay_ms(350);
            }
            LightToggle();
            test_flag=0;
        }
    }
}

