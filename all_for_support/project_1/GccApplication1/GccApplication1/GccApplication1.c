/*
 * GccApplication1.c
 *
 * Created: 11.08.2015 14:47:50
 *  Author: homeNout
 */ 


#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 8000000

void preset()
{
	DDRC = 0xff;
	PORTC = 0x00;
	DDRD = 0x00;
	PORTD = 0b00000001;
}


int main(void)
{
	preset();
	
    while(1)
    {
       PORTC = 0x00;
	   
	   if(!PIND)
	   {
		   
	    PORTC = 0b00000001;
		_delay_ms(1500);
		
		for (int i =0; i<7; i++)
		{
			PORTC = (PORTC<<1);
			_delay_ms(1500);
		}
		
	   }
	   else {PORTC = 0xff;
				_delay_ms(3000);	}
    }
}