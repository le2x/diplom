/*
 * lesson_7.c
 *
 * Created: 14.08.2015 14:22:12
 *  Author: homeNout
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "n5110.h"

void preset()
{
	
	Lcd_init();
			
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //делитель 128 64 к√ц
	
	ADMUX |= (1<<REFS1) | (1<<REFS0); // опорное напр€жение
	// (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);
	
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
	
	for (int i=0; i<40; i++)
	{
		ADCSRA |= (1<<ADSC);
		
		while((ADCSRA & (1<<ADSC)));
		
		tmp = tmp + ADC;
	} 
	
	
	
	return tmp/40;
}


int main(void)
{
	preset();
	
    while(1)
    {
		
         Lcd_clear();
		Lcd_printf(0,0,FONT_1X,readAdc(0)*0.02935,1);
		Lcd_printf(0,1,FONT_1X,readAdc(1)*0.02935,1);
		 Lcd_update();
		 _delay_ms(2000);
    }
}