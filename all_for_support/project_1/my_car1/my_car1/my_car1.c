/*
 * my_car1.c
 *
 * Created: 13.09.2015 11:35:50
 *  Author: homeNout
 */ 


#define F_CPU 8000000
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "n5110.h"



void presetADC()
{
	
	Lcd_init();
	
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //делитель 128 64 к√ц
	
	ADMUX |= (0<<REFS1) | (1<<REFS0); // опорное напр€жение
	// (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);
	
}

void preset()
{
	DDRD = 0xff;
	PORTD = 0x00;
	
}

int readAdc(unsigned int ch){
	
	unsigned char set_admux = ADMUX;
	
	set_admux &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0) | (1<<MUX0));
	
	switch(ch)
	{
		case 1: set_admux |= (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (1<<MUX0); break;
		default:break;
	}
	
	ADMUX = set_admux;
	
	_delay_us(10);
	
	int tmp = 0;
	
	for (int i=0; i<20; i++)
	{
		ADCSRA |= (1<<ADSC);
		
		while((ADCSRA & (1<<ADSC)));
		
		tmp = tmp + ADC;
	}
	
	
	
	return tmp/20;
}


int main(void)
{	
	short int fotolev = 0, fotoprav = 0;
	presetADC();
	preset();
	
    while(1)
    {
		fotoprav = readAdc(0); fotolev = readAdc(1);
		if (fotoprav > fotolev)
		{
			PORTD |= (1<<PD0);
			_delay_ms(50);
			PORTD &= ~(1<<PD0);
		} 
		if (fotoprav < fotolev)
		{
			PORTD |= (1<<PD1);
			_delay_ms(50);
			PORTD &= ~(1<<PD1);
		}
		if (fotoprav == fotolev)
		{
			PORTD |= (1<<PD1);
			PORTD |= (1<<PD0);
			_delay_ms(50);
			PORTD &= ~(1<<PD1);
			PORTD &= ~(1<<PD0);
		}
		
         
    }
}