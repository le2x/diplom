/*
 * UART.c
 *
 * Created: 31.03.2013 16:34:53
 *  Author: Alex_EXE
 */ 

#ifndef F_CPU
#define F_CPU 1000000UL	//	рабочая частота
#define GROUND_LEFT_MOTOR PA7
#define POWER_LEFT_MOTOR PA6
#define GROUND_RIGHT_MOTOR PA5
#define POWER_LEFT_MOTOR PA4
#endif

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>		//	для _delay_ms()
#include <stdlib.h>
#include <avr/interrupt.h>

int int1Led = 1;
int int0Led = 1;

int WholePWM = 0, LeftBalancePWM = 0, RightBalancePWM = 0, Spiral = 1;

void init_UART()
{
	//	Установка скорости 9600
	UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000Гц band=9600, 
	UBRRL=51;	//	нормальный асинхронный двунаправленный режим работы
	
//			RXC			-	завершение приёма
//			|TXC		-	завершение передачи
//			||UDRE 		-	отсутствие данных для отправки
//			|||FE		-	ошибка кадра
//			||||DOR		-	ошибка переполнение буфера
//			|||||PE		-	ошибка чётности
//			||||||U2X	-	Двойная скорость
//			|||||||MPCM	-	Многопроцессорный режим
//			76543210
	UCSRA=0b00000000;

//			RXCIE		-	прерывание при приёме данных
//			|TXCIE		-	прерывание при завершение передачи
//			||UDRIE		-	прерывание отсутствие данных для отправки
//			|||RXEN		-	разрешение приёма
//			||||TXEN	-	разрешение передачи
//			|||||UCSZ2	-	UCSZ0:2 размер кадра данных
//			||||||RXB8	-	9 бит принятых данных
//			|||||||TXB8	-	9 бит переданных данных
//			76543210
	UCSRB=0b00011000;	//	разрешен приём и передача по UART

//			URSEL		-	всегда 1
//			|UMSEL		-	режим:1-синхронный 0-асинхронный
//			||UPM1		-	UPM0:1 чётность
//			|||UPM0		-	UPM0:1 чётность
//			||||USBS	-	топ биты: 0-1, 1-2
//			|||||UCSZ1	-	UCSZ0:2 размер кадра данных
//			||||||UCSZ0	-	UCSZ0:2 размер кадра данных
//			|||||||UCPOL-	в синхронном режиме - тактирование
//			76543210
	UCSRC=0b10000110;	//	8-битовая посылка
}

void init_pwm()
{
	TCCR0|=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS01) |(1<<CS00);
	OCR0 = 0x00; //начальное состояние 0
	
	TCCR2|=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS21) |(1<<CS20);
	OCR2 = 0x00; //начальное состояние 0
	
	DDRD |= (1<<PD7);
	DDRB |= (1<<PB3);
}
/*
void int0_init( void )
{
	//настраиваем на срабатывание INT0 по переднему фронту
	MCUCR |= (1<<ISC01)|(1<<ISC00);
	//разрешаем внешнее прерывание INT0
	GICR |= (1<<INT0);
	sei();
}

ISR( INT0_vect )
{
	//int0_cnt++;
	PORTC = 0b00000001;			//	Включаем диод PC0 = 1 = Vcc
}
*/

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

int main()
{
	init_UART();					//	инициализация UART
	_delay_ms(1000);				//	задержка 1c
	send_Uart_str("alex-exe.ru");	//	отправка строки
	send_Uart(13);					//	перенос строки
	send_int_Uart(2013);			//	отправка числа
	send_Uart(13);					//	перенос строки
	
	init_pwm();
	
	DDRA = 0xFF;
	PORTA |= (1<<PA4) | (1<<PA6);
	
	WholePWM = 180;
	OCR2 = (WholePWM+LeftBalancePWM)*Spiral;
	OCR0 = WholePWM+RightBalancePWM;
	send_int_Uart(OCR2);
	send_int_Uart(OCR0);
	/*DDRC = 0xff;				//	инициализация порта C, PC0 - выход
	DDRB = 0xff;
	DDRD &= (1<<PD2);
	PORTD |= (1<<PD2);
	init_pwm();
	init_extInt();*/
	/*init_UART();					//	инициализация UART
	_delay_ms(1000);				//	задержка 1c
	send_Uart_str("alex-exe.ru");	//	отправка строки
	send_Uart(13);					//	перенос строки
	send_int_Uart(2013);			//	отправка числа
	send_Uart(13);					//	перенос строки
   
   send_Uart_str("AT+CWMODE=3");   //устанавливаем режим wi-fi клиент+точка доступа
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPMODE=0");  //устанавливаем режим двухстороннего обмена данными для ip сервера
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPMUX=1");  //устанавливаем режим нескольких соединений для ip сервера
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPSERVER=1,8888");  //создаем ip сервер на порте 8888
   send_Uart(13);
   _delay_ms(500);
     
   char buff;
   */
	
	/* interrupt testing
		//временная переменная
		unsigned long tmp;
		DDRC = 0b00000001;				//	инициализация порта C, PC0 - выход


		int0_init();

		

		while(1) {
			
			printf( "int0_cnt = %lu\r\n", tmp );
			//пауза 1 секунда
			PORTC = 0b00000000;			//	Выключаем диод PC0 = 0 = Vcc
		}
		return 0;
		PORTC = 0b00000000;		
		/*if(i<255){
			OCR1A++;
			i++;
			_delay_ms(250);
		}else{
			OCR1A = 0x00;
			i=0;
		}
		*/
		// send_Uart_str("hello");  //создаем ip сервер на порте 8888
		 //send_Uart(13);
		/*if(UCSRA&(1<<RXC))			//	если пришёл байт по UART
		{
			buff = getch_Uart();
			send_Uart(buff+1);
			if (buff == 'a')
			{
				PORTC |= (1<<PC1);
			}
			if (buff == 'w')
			{
				PORTC |= (1<<PC2);
			}
				//	отвечаем увеличенным на 1 пришедшим байтом
			// LED on					//	и мигнём диодом
			PORTC |= (1<<PC0);			//	Включаем диод PC0 = 1 = Vcc
			_delay_ms(50);				//	задержка 50мс
			// LED off
			PORTC &= ~(1<<PC0);			//	Выключаем диод PC0 = 0 = Vcc
		}	*/
		
		
			
}