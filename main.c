/* Name: main.c
 * Author: Ian Leeder
 * Date: 29/10/2013
 */

/*
 
                      ATtiny85
                    _____   _____
                    |    \_/    |
      (RESET*) PB5 -| 1        8|- VCC
               PB3 -| 2        7|- PB2 (SCK)
               PB4 -| 3        6|- PB1 (MISO/OC0B)
               GND -| 4        5|- PB0 (MOSI/OC0A)
                    |___________|

 */

#include <avr/io.h>
#include <util/delay.h>
#include "owi.h"

#define F_CPU 8000000

#define MIN_TEMP 50
#define MAX_TEMP 80
#define MIN_AMBIENT 5
#define MIN_DUTY 51     // 20% of full range (255)
#define MAX_DUTY 255    // 100% of full range (255)

int main(void)
{
    // Set OC0A (Output Compare 0A used for PWM generation) as an output pin
    DDRB |= (1<<DDB0);
    
    // Set WGM bits to Fast PWM wave generation
    TCCR0A = (1<<WGM00)|(1<<WGM01);
    
    // Set COM bit for to normal PWM mode (clear on match, set on bottom).
    // This seems counter-intuitive but is actually the correct setting.
    // In this mode, the higher number, the longer the duty cycle.
    TCCR0A |= (1<<COM0A1);
    
    // PWM frequency = (F_CPU)/(prescaler * 256)
    // This is because it is an 8-bit register (256 values),
    // so it has to update 256 times for each cycle.
    // Set CS bits to indicate no clock prescaler
    // 8MHz / 256 means frequency is 31.25kHz.  Perfect!
    TCCR0B |= (1<<CS00);
    
    // Setup our compare value to set duty cycle
	OCR0A = MIN_DUTY;

    volatile char temp = 0;
    
    for(;;)
    {
        temp = 0;
        temp = therm_read_degrees();
        
        // Failsafe in case temperature sensor gives unexpected readings (ie 0 or -ve)
        if (temp < MIN_AMBIENT)
            OCR0A = MAX_DUTY;
        // Else If temperature is below the minimum threshold, run fan at minimum speed (duty cycle).
        else if (temp < MIN_TEMP)
            OCR0A = MIN_DUTY;
        // Else If temperature is above the maximum threshold, run fan at maximum speed (duty cycle).
        else if (temp > MAX_TEMP)
            OCR0A = MAX_DUTY;
        // Else scale linearly.
        // (temp - min) / (max - min) gives me a percentage within that range
        // Multiply by duty range (max_duty - min_duty)
        // And add min_duty
        else
            OCR0A=((temp-MIN_TEMP)*(MAX_DUTY-MIN_DUTY))/(MAX_TEMP-MIN_TEMP) + MIN_DUTY;
        
        // delay for 0.25 seconds
        // (temperature conversion, with 12-bit resolution, takes 750ms already)
        // Means updates occur roughly every second
        _delay_ms(250);
    }
    return 0;
}