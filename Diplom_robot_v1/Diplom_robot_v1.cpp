/*
 * UART.c
 *
 * Created: 31.03.2013 16:34:53
 *  Author: Alex_EXE
 */ 

#ifndef F_CPU
#define F_CPU 1000000UL	//	рабочая частота must be 8000000
#define GROUND_LEFT_MOTOR PA7
#define POWER_LEFT_MOTOR PA6
#define GROUND_RIGHT_MOTOR PA5
#define POWER_RIGHT_MOTOR PA4
#define COMPARE_RATE 20 // rate that regulated sensitivity of PWM balancing
#endif

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>		//	для _delay_ms()
#include <stdlib.h>
#include <avr/interrupt.h>

int WholePWM = 192, LeftBalancePWM = 0, RightBalancePWM = 0, Spiral = 1; // +-63 to balance PWM; wholePWM = 75% of power
int impulseLeftPerWheel = 0, impulseLeftPerWheeltemp = 0, impulseRightPerWheel = 0, impulseRightPerWheeltemp = 0, compareTemp = 0; //переменные для скорости вращения колёс
bool isFirstInt1 = false, isFirstInt0 = false; //?
bool isImpulseLeftUsed = false, isImpulseRightUsed = false; // each int must be compared only once


void init_UART()
{
	//	Установка скорости 9600
	UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000Гц band=9600, 
	UBRRL=51;	//	нормальный асинхронный двунаправленный режим работы
	UCSRA=0b00000000;
	UCSRB=0b00011000;	//	разрешен приём и передача по UART
	UCSRC=0b10000110;	//	8-битовая посылка
}

//	UART
void send_Uart(unsigned char c)//	Отправка байта
{
	while(!(UCSRA&(1<<UDRE)))	//	Устанавливается, когда регистр свободен
	{}
	UDR = c;
}

void send_Uart_str(const char *s)//	Отправка строки
{
	while (*s != 0) send_Uart(*s++);
}

void send_int_Uart(unsigned int c)//	Отправка числа от 0000 до 9999
{
	unsigned char temp;
	c=c%10000;
	temp=c/100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
	temp=c%100;
	send_Uart(temp/10+'0');
	send_Uart(temp%10+'0');
}

unsigned char getch_Uart()//	Получение байта
{
	while(!(UCSRA&(1<<RXC)))	//	Устанавливается, когда регистр свободен
	{}
	return UDR;
}

void init_pwm()
{
//	TCCR0|=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS01);
//	OCR0 = 0x00; //начальное состояние 0
	
//	TCCR2|=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS21);
//	OCR2 = 0x00; //начальное состояние 0
	
	TCCR1A = (1<<COM1A1)|(1<<WGM10)|(1<<COM1B1)|(1<<WGM12);
	TCCR1B = (1<<CS11); //предделитель = 1
	OCR1A = 0x00; //начальное состо¤ние 0
	
	DDRD |= (1<<PD4)|(1<<PD5);
	
	OCR1A = (WholePWM+RightBalancePWM)*Spiral;
	OCR1B = WholePWM+RightBalancePWM;
	//DDRB |= (1<<PB3);
}

void initTimer(){
	TIMSK |= (1<<TOIE0);
	TCCR0|=(1<<CS00);
}

void externalIntInit( void )
{
	//настраиваем на срабатывание INT0 по переднему фронту
	MCUCR |= (1<<ISC01)|(1<<ISC00)|(1<<ISC11)|(1<<ISC10);
	//разрешаем внешнее прерывание INT0
	GICR |= (1<<INT0)|(1<<INT1);
	sei();
}

ISR( INT0_vect )
{
	impulseLeftPerWheeltemp = impulseLeftPerWheel;
	isImpulseLeftUsed = false;
	send_Uart_str("impulseLeft = ");
	send_int_Uart(impulseLeftPerWheel);
	send_Uart_str(" OCR1A = ");
	send_int_Uart(OCR1A);
	send_Uart(13);
	impulseLeftPerWheel = 0;
}

ISR( INT1_vect ){
	impulseRightPerWheeltemp = impulseRightPerWheel;
	isImpulseRightUsed = false;
	send_Uart_str("impulseRight = ");
	send_int_Uart(impulseRightPerWheel);
	send_Uart_str(" OCR1B = ");
	send_int_Uart(OCR1B);
	send_Uart(13);
	impulseRightPerWheel = 0;
}

ISR(TIMER0_OVF_vect){
	impulseLeftPerWheel++;
	impulseRightPerWheel++;
}

void MoveForward(){
	send_Uart_str("MoveForward");	//	отправка строки
	PORTA |= (1<<POWER_LEFT_MOTOR) | (1<<POWER_RIGHT_MOTOR); 
	if(!isImpulseLeftUsed && !isImpulseRightUsed){
	compareTemp = impulseLeftPerWheeltemp - impulseRightPerWheeltemp;
	isImpulseLeftUsed = true; isImpulseRightUsed = true;
	impulseLeftPerWheeltemp = 0; impulseRightPerWheeltemp = 0;
	send_Uart_str("  comparetemp = ");
	send_int_Uart(compareTemp);
	}
	
	send_Uart(13);		
	if (compareTemp<0-COMPARE_RATE){
		if (OCR1A < 255){
			LeftBalancePWM += 5;
		}else{
			RightBalancePWM -=5;
		}
	}
	if (compareTemp>0+COMPARE_RATE){
		if(OCR1B< 255){
			RightBalancePWM += 5;
		}else{
			LeftBalancePWM-=5;
		}
	}
	compareTemp = 0;	
	OCR1B = (WholePWM+LeftBalancePWM)*Spiral;
	OCR1A = WholePWM+RightBalancePWM;
}

void PortsInit(){
	DDRA |= (1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7); // set driver outputs to exit
	DDRC |= (1<<PC7); // signaling led
	PORTA |= (1<<POWER_LEFT_MOTOR) | (1<<POWER_RIGHT_MOTOR); 
}


int main()
{
	init_UART();					//	инициализация UART
	_delay_ms(1000);				//	задержка 1c
	send_Uart_str("alex-exe.ru");	//	отправка строки
	send_Uart(13);					//	перенос строки
	send_int_Uart(2013);			//	отправка числа
	send_Uart(13);					//	перенос строки
	
	init_pwm();
	externalIntInit();
	initTimer();
	PortsInit();

	send_int_Uart(OCR2);
	send_int_Uart(OCR0);
	
	PORTC |= (1<<PC7);
	_delay_ms(1000);
	PORTC &= ~(1<<PC7);
	_delay_ms(1000);
     
  	while(1){
			MoveForward();
		}
			
}

//    send_Uart_str("AT+CWMODE=3");   //устанавливаем режим wi-fi клиент+точка доступа
   //    send_Uart(13);
   //    _delay_ms(500);
   //    
   //    send_Uart_str("AT+CIPMODE=0");  //устанавливаем режим двухстороннего обмена данными для ip сервера
   //    send_Uart(13);
   //    _delay_ms(500);
   //    
   //    send_Uart_str("AT+CIPMUX=1");  //устанавливаем режим нескольких соединений для ip сервера
   //    send_Uart(13);
   //    _delay_ms(500);
   //    
   //    send_Uart_str("AT+CIPSERVER=1,8888");  //создаем ip сервер на порте 8888
   //    send_Uart(13);
   //    _delay_ms(500);
   
   /*
			 i = PIND;
			if((1<<PD2) & i) {
			}
			*/