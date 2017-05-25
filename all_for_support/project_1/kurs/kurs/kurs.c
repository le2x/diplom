/*
 * kurs.c
 *
 * Created: 26.08.2015 14:10:09
 *  Author: homeNout
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/iom8.h>
#include "n5110.h"

#define F_CPU 8000000

int sec = 0, min = 0, hour = 0; //no-komments
uint8_t knopka = 0;   // nazhataya knopka
uint8_t temp1 = 25, temp2 = 30;
int vlajnost = 300; //parametri
volatile uint8_t tempNov = 0,
volatile int vlajnostNow = 0;
volatile uint8_t tmpMin = 0;

uint8_t fort = 0;
uint8_t knopkaTemp = 0;
uint8_t knopkaTemp1 = 0;
uint8_t knopkaTemp2 = 0;

void preset()
{
	Lcd_init();
	DDRD = 0b00011111;
	PORTD = 0b11100000;
}

void ADCinit()
{
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //делитель 128 64 к√ц
	
	ADMUX |= (1<<REFS1) | (1<<REFS0); // опорное напр€жение
}

void setTimer()
{
	TCCR0 |= (1<<CS02) | (0<<CS01) | (1<<CS00);
	TIMSK |= (1<<TOIE0);
	sei();
}

int readAdc(unsigned int ch){
	
	unsigned char set_admux = ADMUX;
	
	set_admux &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	
	switch(ch)
	{
		case 1: set_admux |= (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (1<<MUX0); break;
		case 2: set_admux |= (0<<MUX3) | (0<<MUX2) | (1<<MUX1) | (0<<MUX0); break;
		default:set_admux &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0) | (1<<MUX0)); break;
	}
	
	ADMUX = set_admux;
	
	_delay_us(10);
	
	int tmp = 0;
	
	for (int i=0; i<10; i++)
	{
		ADCSRA |= (1<<ADSC);
		
		while((ADCSRA & (1<<ADSC)));
		
		tmp = tmp + ADC;
	}
	
	
	
	return tmp/10;
}


void Chasi()
{
	if (sec/30>59)
	{
		min += 1;
		tmpMin += 1;
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
	
	if(bit_is_clear(PIND,7) )
	{
		knopkaTemp = 1;
	}
	if (PIND & (1<<7) && knopkaTemp)
	{
		knopka |=(1<<0); knopkaTemp = 0;
	}
	
	if(bit_is_clear(PIND,6))
	{
		knopkaTemp1 = 1;
	}
	if (PIND & (1<<6) && knopkaTemp1)
	{
		knopka |=(1<<1); knopkaTemp1 = 0;
	}
	
	
	if(bit_is_clear(PIND,5))
	{
		knopkaTemp2 = 1;
	}
	if (PIND & (1<<5) && knopkaTemp2)
	{
		knopka |=(1<<2); knopkaTemp2 = 0;
	}
	
}

void menu()
{
//perehodi=====================================================================
	if (knopka == 0b00000001 ){knopka = 0b00001000;}
	if (knopka == 0b00001001 ){knopka = 0b00000000;}//vozvrat k glav ekranu
	if (knopka == 0b00001010 ){knopka = 0b00010000;}//perehod k temp
	if (knopka == 0b00001100 ){knopka = 0b00011000;}//perehod k vlajnosti					
	if (knopka == 0b00010100 ){knopka = 0b00100000;}//perehod k temp fort	
	if (knopka == 0b00010010 ){knopka = 0b00101000;}//perehod k vent	
//uvelichenie parametov ======================================================	
	if (knopka == 0b00011100 )
	{
		if(vlajnost < 1023) vlajnost++;
	}
		
	if (knopka == 0b00011010 )
	{
		if(vlajnost >0) vlajnost--;
	}
	
	if (knopka == 0b00011001 )	{knopka = 0b00000000;}//ustanovka vlajnosti
		
	if (knopka == 0b00100100 )
	{
		if(temp1 < 1023) temp1++;
	}
		
	if (knopka == 0b00100010 )
	{
		if(temp1 >0) temp1--;
	}
	
	if (knopka == 0b00100001 )	{knopka = 0b00000000;}//ustanovka temp fort
		
	if (knopka == 0b00101100 )
	{
		if(temp2 < 1023) temp2++;
	}
	
	if (knopka == 0b00101010 )
	{
		if(temp2 >0) temp2--;
	}
	
	if (knopka == 0b00101001 )	{knopka = 0b00000000;}//ustanovka temp vent
//vershini======================================================	
	if (knopka == 0b00000000 )
	{
		Lcd_prints(1,0,FONT_1X,(unsigned char *)PSTR("temperatura"));
		Lcd_printf(4,1,FONT_1X,tempNov,0);//pomeniyat na realnuy temperaturu
		Lcd_prints(1,3,FONT_1X,(unsigned char *)PSTR("vlajnost"));
		Lcd_printf(4,4,FONT_1X,vlajnostNow,0);//pomeniyat na realnuy temperaturu
	}
	
	if (knopka == 0b00001000 )
	{
		Lcd_prints(0,0,FONT_1X,(unsigned char *)PSTR("set temp"));
		Lcd_prints(0,1,FONT_1X,(unsigned char *)PSTR("set vlajnost"));
		Lcd_prints(4,3,FONT_1X,(unsigned char *)PSTR("exit"));
	}
	
	if (knopka == 0b00010000 )
	{
		Lcd_prints(0,0,FONT_1X,(unsigned char *)PSTR("set temp_fort"));
		Lcd_prints(0,1,FONT_1X,(unsigned char *)PSTR("set temp_vent"));
	}
	
	if (knopka == 0b00011000 )
	{
		Lcd_prints(3,0,FONT_1X,(unsigned char *)PSTR("vlajnost"));
		Lcd_printf(5,1,FONT_1X,vlajnost,0);
	}
	
	if (knopka == 0b00100000 )
	{
		Lcd_prints(3,0,FONT_1X,(unsigned char *)PSTR("temp_fort"));
		Lcd_printf(5,1,FONT_1X,temp1,0);
	}
	
	if (knopka == 0b00101000 )
	{
		Lcd_prints(3,0,FONT_1X,(unsigned char *)PSTR("temp_vent"));
		Lcd_printf(5,1,FONT_1X,temp2,0);
	}
	
	knopka &= ~((1<<2) | (1<<1) | (1<<0));
			
}

void motor(unsigned char vrashen)
{
	if(vrashen)
	{
		PORTD |= (1<<PD0);
		_delay_ms(10000);
		fort = 1;
		PORTD &= ~(1<<PD0);
	} else 
	{
		PORTD |= (1<<PD1);
		fort = 0;
		_delay_ms(10000);
		PORTD &= ~(1<<PD1);	
	}
}


void izmVlajnost()
{
	PORTD |= (1<<PD4);
	vlajnostNow = readAdc(0);
	PORTD &= ~(1<<PD4);
}


int main(void)
{
	
	preset();
	setTimer();
	ADCinit();
	while(1)
	{
		tempNov = (((readAdc(2)*3.73*100)/1023)-273.15);
		if(sec/30 == 30) izmVlajnost();
		
		if (tempNov >= temp1 && !fort) motor(1);
		if (tempNov < temp1 && fort) motor(0);
		if (tempNov >= temp2) {PORTD |= (1<<PD2);}
			else
			{
				PORTD &= ~(1<<PD2);
			}
		
		 if (vlajnostNow < vlajnost) 
		 {
			PORTD |= (1<<PD3); tmpMin = 0;
		 }
		
		if (tmpMin == 1)
		{
			PORTD &= ~(1<<PD3); 
		}
				
		Chasi();
		Lcd_clear();
		menu();
		Lcd_update();
		_delay_ms(500);
		
		
	}
}