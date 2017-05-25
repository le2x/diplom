/*
 * GccApplication2.c
 *
 * Created: 12.08.2015 16:20:55
 *  Author: homeNout
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


#include "n5110.h"

void preset(){
	Lcd_init();
	Lcd_clear();
	Lcd_update();
	DDRC |=(1<<PC0);
	
	
}

void gradusnic(){
	Lcd_circle(42,5,3,PIXEL_ON);
	
	for(int i =0; i<7; i++)
	{
	Lcd_circle(42,30,i,PIXEL_ON);	
	}
	
	Lcd_line(39,5,39,26,PIXEL_ON);
	Lcd_line(45,5,45,26,PIXEL_ON);
	
	
}

void shkala(int a)
{
	for(int i = 24; i>3; i--)
	{
		Lcd_line(40,i,44,i,PIXEL_OFF);
	}
	
	for(int i = 24; i>a+3; i--)
	{
		Lcd_line(40,i,44,i,PIXEL_ON);
	}
	
	
}


int main(void)
{
	preset();
	
	int times = 1;
    while(1)
    {
       	gradusnic();
		PORTC |= (1<<PC0);
		
		srand(times);  
		shkala(rand()%23);
		
		Lcd_update();
		_delay_ms(500);
		PORTC = 0x00;
		_delay_ms(1000);
		times++;
		
    }
}