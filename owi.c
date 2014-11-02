//
//  owi.c
//  demo
//
//  Created by Ian Leeder on 28/10/13.
//  Code created from examples in http://teslabs.com/openplayer/docs/docs/other/ds18b20_pre1.pdf
//
//

#include "owi.h"

unsigned char therm_reset();
void therm_write_bit(unsigned char);
unsigned char therm_read_bit(void);

// Delay in cycles
// use therm_delay(us(delay));
inline __attribute__((gnu_inline)) void therm_delay(unsigned int delay){
    while(delay--) asm volatile("nop");
}

unsigned char therm_reset()
{
    unsigned char i;
    
    //Pull line low and wait for 480uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    therm_delay(us(480));
    
    //Release line and wait for 60uS
    THERM_INPUT_MODE();
    THERM_PULLUP();
    therm_delay(us(60));
    
    //Store line value and wait until the completion of 480uS period
    i=(THERM_PIN & (1<<THERM_DQ));
    therm_delay(us(420));
    
    //Return the value read from the presence pulse (0=OK, 1=WRONG)
    return i;
}

void therm_write_bit(unsigned char bit)
{
    //Pull line low for 1uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    therm_delay(us(1));
    
    //If we want to write 1, release the line (if not will keep low)
    if(bit)
    {
        THERM_INPUT_MODE();
        THERM_PULLUP();
    }
    
    //Wait for 60uS and release the line
    therm_delay(us(60));
    THERM_INPUT_MODE();
    THERM_PULLUP();
}

unsigned char therm_read_bit(void)
{
    unsigned char bit=0;
    
    //Pull line low for 1uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    therm_delay(us(1));
    
    //Release line and wait for 14uS
    THERM_INPUT_MODE();
    THERM_PULLUP();
    therm_delay(us(14));
    
    //Read line value
    if(THERM_PIN&(1<<THERM_DQ))
        bit=1;
    
    //Wait for 45uS to end and return read value
    therm_delay(us(45));
    return bit;
}

unsigned char therm_read_byte(void)
{
    unsigned char i=8, n=0;
    while(i--)
    {
        //Shift one position right and store read value
        n>>=1;
        n|=(therm_read_bit()<<7);
    }
    return n;
}

void therm_write_byte(unsigned char byte)
{
    unsigned char i=8;
    
    while(i--)
    {
        //Write actual bit and shift one position right to make the next bit ready
        therm_write_bit(byte&1);
        byte>>=1;
    }
}

char therm_read_degrees()
{
    char digit;
    
    //Reset, skip ROM and start temperature conversion
    therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_CONVERTTEMP);
    
    //Wait until conversion is complete
    while (!therm_read_bit());
    //Reset, skip ROM and send command to read Scratchpad
    therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_RSCRATCHPAD);
    
    digit=0;
    //Read Scratchpad (only 2 first bytes)
    digit = (therm_read_byte()>>4);
    digit |= (therm_read_byte()<<4);
    therm_reset();
    
    return digit;
}