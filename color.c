#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "LEDs.h"
#include <stdio.h>
#include "serial.h"
#include "buttons.h"
#include <math.h>


void color_click_init(void)
{   
    I2C_2_Master_Init();//Initialise i2c Master

	color_writetoaddr(0x00, 0x01); //set device PON to turn ColourClick on
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
	color_writetoaddr(0x00, 0x03); // Turn on device ADC

    // TO CONFIGURE WITH TESTING
	color_writetoaddr(0x01, 0xD5); //integration time: 64 Integ Cycles, 154ms, 65535 max count

    
    //color_writetoaddr(0x00,0x1B); // Enable Wait time register?
    //color_writetoaddr(0x03, 0x00); // Wait time register: 256
    
    color_writetoaddr(0x0F, 0x00); // Gain: 1X
    
    //color_writetoaddr(0x0C, 0x00);
}   


void color_click_interrupt_init(void){
    //__debug_break();
    color_int_clear();
    color_writetoaddr(0x00, 0x13); //turn on Clicker Interrupt(write 1 to AIEN bit)
    //Configure interrupt thresholds RGBC clear channel
    //color_writetoaddr(0x04, 0x2C);          // low thresh lower byte
    //color_writetoaddr(0x05, 0x01);          // low thresh upper byte
    //color_writetoaddr(0x06, 0xAC);          // upper thresh lower byte
    //color_writetoaddr(0x07, 0x0D);          // upper thresh upper byte
    
    // Use global variables that can be updated by the calibration function
    color_writetoaddr(0x04, int_low & 0xFF); 
    color_writetoaddr(0x05, int_low >> 8); 
    color_writetoaddr(0x06, int_high & 0xFF); 
    color_writetoaddr(0x07, int_high >>8);
    
    color_writetoaddr(0x0C, 0b0001); // Persistence register = 1
    color_int_clear();
}

// We call this function in main.c
void interrupt_threshold_calibrate(void) {
    // Calibration procedure
    // start the procedure with button press
    int amb_and_LED;
    while (LeftButton); //empty while loop (wait for button press)
    for(int i=0;i<5;i++){   // indicate the procedure has started with 5 flashes of LED
        __delay_ms(100);
        LED2=1;
        __delay_ms(100);
        LED2=0;
    }
    clear = color_read(0x14); // read clear color channel for blue card
    int_high = clear;
    
    while (LeftButton); //empty while loop (wait for button press)
    for(int i=0;i<5;i++){   // indicate the procedure has started with 5 flashes of LED
        __delay_ms(100);
        LED2=1;
        __delay_ms(100);
        LED2=0;
    }
    clear = color_read(0x14); // read clear color channel for blue card
    amb_and_LED = clear;    
    
    while (LeftButton); //empty while loop (wait for button press)
    for(int i=0;i<5;i++){   // indicate the procedure has started with 5 flashes of LED
        __delay_ms(100);
        LED2=1;
        __delay_ms(100);
        LED2=0;
    }    
    clear = color_read(0x14); // read clear color channel for blue card
    
    if(clear<amb_and_LED){int_low=clear+(clear/20);}
    else{int_low=0;}
    /*
    // wait for a second button press to finish the calibration
    while (LeftButton); //empty while loop (wait for button press)
    for(int i=0;i<5;i++){ // indicate the procedure has ended
        __delay_ms(100);
        LED2=1;
        __delay_ms(100);
        LED2=0;
    }
     */
    // update global variables for high and low (low is 0 by default has we don't want to trigger interrupt when light level falls)
    //int_low = 0;
    //int_high = clear;   
}

void color_click_interrupt_off(void){
    color_int_clear();
    color_writetoaddr(0x00,0x03);
}


void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode (note 0x52=0x29<<1)
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}


void color_int_clear(void){
    I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
    I2C_2_Master_Write(0xE6); // Clear RGBC interrupt
}


unsigned int color_read(unsigned char address)
{  
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode

    I2C_2_Master_Write(0xA0 | address);//command (auto-increment protocol transaction) + start at a colour's low register

    I2C_2_Master_RepStart();// start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01);//7 bit address + Read (1) mode

    tmp=I2C_2_Master_Read(1);//read the  LSB
    tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();      
    return tmp;
}
/*
// read all colour channel values for an array
void color_read_all(unsigned int *colorArray)
{
    for(int i=0;i<4;i++){                           // read RGBC channel values
        *(colorArray+i) = color_read(0x14+2*i);
    }
}
*/

/*Send the interrupt status over the Serial port
 Function for debugging purposes only - delete later*/ 
void get_int_status(void)
{  
    //__debug_break();
	unsigned char tmp;
    unsigned char intstatus[9];
    unsigned char throw;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode

    I2C_2_Master_Write(0xA0 | 0x13);//command (auto-increment protocol transaction) + start at a colour's low register

    I2C_2_Master_RepStart();// start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01);//7 bit address + Read (1) mode

    tmp = I2C_2_Master_Read(1);//read the  LSB
    throw = I2C_2_Master_Read(0); //read the MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();
    tmp = tmp >>4;
    sprintf(intstatus, "  INT:%d  ",tmp );
    TxBufferedString(intstatus);
    sendTxBuf();
}


void Color2String(char *ptr, unsigned int *pval){
    sprintf(ptr,"  %05d ", *pval);
}


//TODO: Shorten Below function
void SendColorReadings(void){
    char colorstring[8];

    putCharToTxBuf(' ');
    
    //putCharToTxBuf('R');
    sprintf(colorstring," R%05d ", red);
    TxBufferedString(colorstring); 
    
    //putCharToTxBuf('G');
    sprintf(colorstring," G%05d ", green);
    TxBufferedString(colorstring); 
    
    //putCharToTxBuf('B');
    sprintf(colorstring," B%05d ", blue);
    TxBufferedString(colorstring); 
    
    //putCharToTxBuf('C');
    sprintf(colorstring," C%05d ", clear);
    TxBufferedString(colorstring);

    sendTxBuf(); //interrupt will handle the rest
}

/*
void read_All_Colors(void){
    red = color_read(0x16); 
    green = color_read(0x18); 
    blue = color_read(0x1A); 
    clear = color_read(0x14); 
}

// Work in Progress
int decide_color(void){
    int color_decision;
    
    __delay_ms(500);                                // allow readings to stabilize   
    read_All_Colors();                              // read color channel values
    unsigned int LED_and_amb_read[4] = {red,green,blue,clear};      // light from ambient + LED cross talk + LED reflection {red,green,blue,clear)
    LightToggle();                                  // turn RGB LED off 
    
    int black_thresh = clear;
    
    __delay_ms(500);                                // allow readings to stabilize
    read_All_Colors();                              // read color channel values
    unsigned int ambient[4] = {red,green,blue,clear};               // read color channel values
    
    
    // correct the readings to remove ambient and LED cross_talk
    red = LED_and_amb_read[0]-ambient[0]-LED_cross_talk[0];
    green = LED_and_amb_read[1]-ambient[1]-LED_cross_talk[1];
    blue = LED_and_amb_read[2]-ambient[2]-LED_cross_talk[2];
    clear = LED_and_amb_read[3]-ambient[3]-LED_cross_talk[3];
    
    
    // calculate integer percentage RGB values of clear channel
    int redPercentage = 100*(red*pow(clear,-1));
    int greenPercentage = 100*(green*pow(clear,-1));
    int bluePercentage = 100*(blue*pow(clear,-1));

    //unsigned int red_channel_red_orange_sep_thresh = 200; // a threshold to distinguish between red and orange in decision process
    //unsigned int clear_channel_black_thresh = 2200;        // a threshold to tell if the color is black

    //Procedure to decide - will explain in read me file to follow logic process via graphs
    if (black_thresh <= int_low){color_decision=9;}
    else if (redPercentage >= 65){                               // distinguish between red and orange

        if(greenPercentage<11){
            //color_decision = 'r';
            color_decision = 1;
        }
        else{
            //color_decision = 'o';
            color_decision = 6;
        }
    }
    else if(redPercentage < 65 && redPercentage >= 52){     // distinguish between yellow and pink
        if (greenPercentage>30 && bluePercentage<21){
            //color_decision = 'y';
            color_decision = 4;
        }
        else{
            //color_decision = 'p';
            color_decision = 5;
        }
    }
    else if(redPercentage < 52 && redPercentage >= 35){     // distinguish between white and light blue
        if (redPercentage>=45){
            //color_decision = 'w';
            color_decision = 8;
        }
        else{
            //color_decision = 'l';
            color_decision = 7;
        }
    }
    else if(redPercentage<35 && redPercentage >= 20){       // distinguish between blue and green
        
        if (bluePercentage>=29){
            color_decision = 3;
        }
        else{
            //color_decision = 'g';
            color_decision = 2;
        }
    }
    
    return color_decision;                                  // return the specific color value
 
}
*/

int decide_color(struct color_card *c){
    int color_decision;
    
    __delay_ms(500);                                // allow readings to stabilize   
    read_All_Colors(LED_and_amb_read);              // read color channel values

    LightToggle();                                  // turn RGB LED off 
    
    int black_thresh = *(LED_and_amb_read);         // determine threshold for black card
    
    __delay_ms(500);                                // allow readings to stabilize
    read_All_Colors(ambient);                       // read color channel values
    
    correct_readings(c,LED_and_amb_read, ambient,LED_cross_talk); // subtract ambient and cross talk from 
    
    c->redPercentage = (100*(c->red_real))/ (c->clear_real);
    c->greenPercentage = (100*(c->green_real)) / (c->clear_real);
    c->bluePercentage = (100*(c->blue_real)) / (c->clear_real);
    

    //Procedure to decide - will explain in read me file to follow logic process via graphs
    if (black_thresh <= int_low){color_decision=9;}
    else if (c->redPercentage >= 65){                             // distinguish between red and orange
        if(c->greenPercentage <11){color_decision = 1;}
        else{color_decision = 6;}
    }
    else if(c->redPercentage < 65 && c->redPercentage >= 52){     // distinguish between yellow and pink
        if (c->greenPercentage >30 && c->bluePercentage<21){color_decision = 4;}
        else{color_decision = 5;}
    }
    else if(c->redPercentage < 52 && c->redPercentage >= 35){     // distinguish between white and light blue
        if (c->redPercentage>=45){color_decision = 8;}
        else{color_decision = 7;}
    }
    else if(c->redPercentage<35 && c->redPercentage >= 15){       // distinguish between blue and green      
        if (c->bluePercentage>=29){color_decision = 3;}
        else{color_decision = 2;}
    }
    
    return color_decision;                                        // return the specific color value
 
}

void read_All_Colors(unsigned int *writeArray){
    for(int i=0;i<4;i++){ 
        *(writeArray+i) = color_read(0x14+2*i); 
	}
}


void correct_readings(struct color_card *c, unsigned int *readArray1, unsigned int *readArray2,unsigned int *readArray3){
    unsigned int corrected_colors[4];
    
    for(int i=0;i<4;i++){ 
        *(corrected_colors+i) = *(readArray1+i)-*(readArray2+i)-*(readArray3+i);
	}
    
    c->clear_real=*(corrected_colors);
    c->red_real=*(corrected_colors+1);
    c->green_real=*(corrected_colors+2);
    c->blue_real=*(corrected_colors+3);
    
}