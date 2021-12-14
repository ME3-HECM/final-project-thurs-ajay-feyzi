#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

/*************************  Color Card Structure ********************************/

struct color_card { 
    unsigned int red;           // raw red readings
    unsigned int green;         // raw green readings
    unsigned int blue;          // raw blue readings
    unsigned int clear;         // raw clear readings
    __int24 red_real;           // corrected red reading
    __int24 green_real;         // corrected green reading
    __int24 blue_real;          // corrected blue reading
    __int24 clear_real;         // corrected clear reading
    char redPercentage;         // percentage red of clear channel
    char greenPercentage;       // percentage green of clear channel
    char bluePercentage;        // percentage blue of clear channel
};


/*************************  Variable Prototypes ********************************/


unsigned int red, green, blue, clear;
unsigned int int_low, int_high;         // Interrupt thresholds
unsigned int LED_and_amb_read[4];      // light from ambient + LED cross talk + LED reflection {red,green,blue,clear)
unsigned int ambient[4];               // read color channel values
unsigned int LED_cross_talk[4] = {993,484,268,183};
//unsigned int LED_cross_talk[4] = {484,268,183,993};

/*************************  Function Prototypes ********************************/


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/**********************************************
 *  Function to write necessary bytes via I2C to clicker for interrupt
 ***********************************************/
void color_click_interrupt_init(void);

/**********************************************
 *  Function to to turn off color click interrupt
 ***********************************************/
void color_click_interrupt_off(void);


/********************************************//**
 *  Function to write to the color click module
 *  address is the register within the color click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);


/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read(unsigned char address);

/**********************************************
 *  Function that calls color_read for all four RGBC channels
 ***********************************************/
//void read_All_Colors(void);
void read_All_Colors(unsigned int *writeArray);


/***********************************************
 * Function to clear the RGBC interrupt in the clicker
 ***********************************************/
void color_int_clear(void);

/*****************
 * Function to take RGBC channel readings and return a 
 * a char corresponding to the color
 ******************/
//int decide_color(void);
int decide_color(struct color_card *c);

void correct_readings(struct color_card *c, unsigned int *readArray1, unsigned int *readArray2,unsigned int *readArray3);


/****** Functions for Testing Purposes - Will be removed from final build *****/

/**********************************************
 *  Function to convert color reading to string
 ***********************************************/
void Color2String(char *ptr, unsigned int *pval);


/**********************************************
 *  Function to send all Color readings over serial port to PC
 ***********************************************/
void SendColorReadings(void);


/**********************************************
 * Function to send back interrupt status at register 0x12 to PC
 ***********************************************/
void get_int_status(void);

/**********************************************
 * Function to calibrate interrupt thresholds
 * 
 ***********************************************/
void interrupt_threshold_calibrate(void);


#endif