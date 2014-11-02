//
//  owi.h
//  demo
//
//  Created by Ian Leeder on 28/10/13.
//  Code created from examples in http://teslabs.com/openplayer/docs/docs/other/ds18b20_pre1.pdf
//
//

#ifndef demo_owi_h
#define demo_owi_h

unsigned char therm_read_byte(void);
void therm_write_byte(unsigned char);
char therm_read_degrees(void);

#include <avr/io.h>

/* Thermometer Connections (At your choice) */
#define THERM_PORT  PORTB
#define THERM_DDR   DDRB
#define THERM_PIN   PINB
#define THERM_DQ    PB4

/* Utils */
#define THERM_INPUT_MODE()  THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW()         THERM_PORT&=~(1<<THERM_DQ)
#define THERM_HIGH()        THERM_PORT|=(1<<THERM_DQ)
#define THERM_PULLUP()      THERM_HIGH()

#define LOOP_CYCLES 8 // Number of cycles performed when executing the therm_delay function

// scaling factor to allow for difference clock frequencies
// determines the amount of loops run in 1us
#define us(num) (num/(LOOP_CYCLES*(1/(F_CPU/1000000.0))))

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec

#endif
