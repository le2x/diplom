/*
 * GccApplication3.c
 *
 * Created: 25.08.2015 12:45:11
 *  Author: homeNout
 */ 


#include <avr/io.h>
#include "n5110.h"
#include <avr/interrupt.h>
#include <avr/delay.h>

#define F_CPU 8000000

int sec = 0, min = 0, hour = 0;

void preset(){

	Lcd_init();
	DDRC |= (1<<PC0);
}

void setTimer()
{
	TCCR0 |= (1<<CS02) | (0<<CS01) | (1<<CS00);
	TIMSK |= (1<<TOIE0);
	sei();
}

void Chasi()
{
	 
	 if (sec/30>59)
	 {
		 min += 1;
		 sec = 0;
	 }
	 if (min>59)
	 {
		 hour+=1;
		 min = 0;
	 }
}

ISR(TIMER0_OVF_vect)
{
	sec += 1;
	//dobavit opros knopok
}

int main(void)
{
	
	preset();
	setTimer();
    while(1)
    {
         Chasi();
		 
		 PORTC = 0x01;
		 
		 Lcd_clear();
		 Lcd_printf(0,0,FONT_1X,hour,0);
		 Lcd_printf(3,0,FONT_1X,min,0);
		 Lcd_printf(6,0,FONT_1X,sec/30,0);
		 
		 Lcd_update();
		 _delay_ms(500);
		 
		 PORTC = 0x00;
		 
		 _delay_ms(500);
		 
    }
}