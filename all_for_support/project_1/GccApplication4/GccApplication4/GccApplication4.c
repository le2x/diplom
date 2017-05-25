/*
 * GccApplication4.c
 *
 * Created: 27.10.2015 21:36:37
 *  Author: homeNout
 */ 



#include <avr/io.h>
#include <util/delay.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include "n5110.h"

#define F_CPU 8000000
#define wateringInterwals 15 // minutes
#define wateringTime 4 // minutes
#define lightingTime 16 //hours 
#define lightstart 10 // o'clock am

uint8_t tempWatering = 0, polivCounter =0, lightingCounter = 0, intervalCounter = 0;
uint8_t polivTriger = 1, lightTrig = 0;
uint8_t sec = 0, min = 0, hour = 0;
int tik = 0; //no-komments
bool lightIsON = false;
bool polivIsON = false;


void preset()
{
	DDRD = 0xff; 
	DDRC = 0x00;
	PORTD = 0xff;
	PORTC = 0xff;
	PORTB = 0xff;
	//Lcd_init();
}

void setTimer()
{
	TCCR0 |= (1<<CS02) | (0<<CS01) | (1<<CS00);
	TIMSK |= (1<<TOIE0);
	sei();
}

void setWatchdog()
{
	WDTCR=0x1F;
	WDTCR=0x0F;
}

void Chasi()
{
	sec = tik/30;
		
	if (sec>60)
	{
		min += 1;
		intervalCounter++;
		if (polivTriger){polivCounter++;}
		tik = 0;
		
	}
	if (min>60)
	{
		hour+=1;
		if (lightTrig){lightingCounter++;}
		min = 0;
	}
	if (hour>24)
	{
		hour = 0;
	}
}

ISR(TIMER0_OVF_vect)
{
	tik += 1;
		
}

void checkWateringIntervals()
{
	if(bit_is_clear(PINC,2))
	{
		tempWatering = wateringInterwals*1;
		if(bit_is_clear(PINC,3))
		{
			tempWatering = wateringInterwals*2;
			if(bit_is_clear(PINC,4))
			{
				tempWatering = wateringInterwals*3;
				if(bit_is_clear(PINC,5))
				{
					tempWatering = wateringInterwals*4;
				}
			}
		}
	}
}

void poliv()
{
	
	if (polivCounter < wateringTime && polivTriger)
	{
		if (polivIsON == false) {powerswitch(1,1);}
	
	} 
	if (polivCounter >= wateringTime && polivTriger)
	{
		if (polivIsON == true) {powerswitch(1,2);}
		intervalCounter = 0;
		polivTriger = 0;
		polivCounter = 0;
	}
}

void powerswitch(uint8_t obj, uint8_t condition) // obj(1-poliv; 2-light) condition(1-ON; 2-OFF)
{
	if (obj == 1 && condition == 1)
	{
		PORTD &= ~(1<<PD0);
		polivIsON = true;
	}
	if (obj == 1 && condition == 2)
	{
		PORTD |= (1<<PD0);
		polivIsON = false;
	}
	
	if (obj == 2 && condition == 1)
	{
		PORTD &= ~(1<<PD1);
		lightIsON = true;
	}
	if (obj == 2 && condition == 2)
	{
		PORTD |= (1<<PD1);
		lightIsON = false;
	}
}

int main(void)
{
	preset();
	setTimer();
	setWatchdog();
	
    while(1)
    {
        Chasi();
		poliv();
		checkWateringIntervals();
		asm("wdr");
		
		if(intervalCounter >= tempWatering)
		{
			polivTriger = 1;
		}
		if(bit_is_clear(PINC,0))
		{
			polivTriger = 1;
		}
		
		if (hour == lightstart)
		{
			lightTrig = 1;
		}
		if (bit_is_clear(PINC,1))
		{
			lightTrig = 1;
			 powerswitch(2,1);
			_delay_ms(100);
		}
		
		if (lightingCounter >= lightingTime)
		{
			lightTrig = 0;
		}
		
		if (lightTrig == 1)
		{
			 powerswitch(2,1);
		}
		if (lightTrig == 0)
		{
			powerswitch(2,2);
			lightingCounter = 0;
		}	
	
		if (bit_is_clear(PINC,1) && lightTrig)
		{
			powerswitch(2,2);
			_delay_ms(100);
		}	
			
    }
}