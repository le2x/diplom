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
#define wateringTime 60 // minutes
#define lightingTime 1 //hours 
#define lightstart 1 // o'clock am
#define hoursToPoliv 1 

uint8_t tempPoliv = 0, polivCounter =0, lightingCounter = 0, intervalCounter = 0;
uint8_t polivTriger = 0, lightTrig = 0;
uint8_t sec = 0, min = 0, hour = 0, polivInHours = 0;
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
	sec = (tik/30);
	if (tik>30)
	{
		tik = 0;
		if (polivTriger){polivCounter++;}
			
	}
		
	if (sec>60)
	{
		min += 1;
		tik = 0;
		
	}
	if (min>60)
	{
		hour+=1;
		polivInHours++;
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
		tempPoliv = hoursToPoliv*1;
		if(bit_is_clear(PINC,3))
		{
			tempPoliv = hoursToPoliv*2;
			if(bit_is_clear(PINC,4))
			{
				tempPoliv = hoursToPoliv*3;
				if(bit_is_clear(PINC,5))
				{
					tempPoliv = hoursToPoliv*4;
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
		polivInHours = 0;
		polivTriger = 0;
		polivCounter = 0;
	}
}

void light()
{
	
	if (lightingCounter < lightingTime && lightTrig)
	{
		if (lightIsON == false) {powerswitch(2,1);}
		
	}
	if ((lightingCounter >= lightingTime && lightTrig) || !lightTrig)
	{
		if (lightIsON == true) {powerswitch(2,2);}
		lightingCounter = 0;
		lightTrig = 0;
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
		light();
		checkWateringIntervals();
		asm("wdr");
		
		if(polivInHours >= tempPoliv)
		{
			polivTriger = 1;
		}
		if(bit_is_clear(PINC,0))
		{
			polivTriger = 1;
		}
		
		if (bit_is_clear(PINC,1) && !lightIsON)
		{
			lightTrig = 1;
		}
		if(hour == lightstart && !lightIsON)
		{
			lightTrig = 1;
		}
		
		if (bit_is_clear(PINC,1) && lightIsON)
		{
			lightTrig = 0;
		}
		/*
		Lcd_prints(0,0,FONT_1X,(unsigned char *)PSTR("lightIsON"));
		Lcd_printf(14,0,FONT_1X,lightIsON,0);//pomeniyat na realnuy temperaturu
		Lcd_prints(0,2,FONT_1X,(unsigned char *)PSTR("tempPoliv"));
		Lcd_printf(14,2,FONT_1X,tempPoliv,0);//pomeniyat na realnuy temperaturu	
		Lcd_prints(0,3,FONT_1X,(unsigned char *)PSTR("lightTrig"));
		Lcd_printf(14,3,FONT_1X,lightTrig,0);//pomeniyat na realnuy temperaturu
		Lcd_prints(0,4,FONT_1X,(unsigned char *)PSTR("lightingCounter"));
		Lcd_printf(14,5,FONT_1X,lightingCounter,0);//pomeniyat na realnuy temperaturu
		Lcd_update();
		_delay_ms(500);
		*/
			
    }
}