/*
 * UART.c
 *
 * Created: 31.03.2013 16:34:53
 *  Author: Alex_EXE
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL	//	������� �������
#endif

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>		//	��� _delay_ms()
#include <stdlib.h>
#include <avr/interrupt.h>

int int1Led = 1;
int int0Led = 1;

void init_UART(void)
{
	//	��������� �������� 9600
	UBRRH=0;	//	UBRR=f/(16*band)-1 f=8000000�� band=9600, 
	UBRRL=51;	//	���������� ����������� ��������������� ����� ������
	
//			RXC			-	���������� �����
//			|TXC		-	���������� ��������
//			||UDRE 		-	���������� ������ ��� ��������
//			|||FE		-	������ �����
//			||||DOR		-	������ ������������ ������
//			|||||PE		-	������ ��������
//			||||||U2X	-	������� ��������
//			|||||||MPCM	-	����������������� �����
//			76543210
	UCSRA=0b00000000;

//			RXCIE		-	���������� ��� ����� ������
//			|TXCIE		-	���������� ��� ���������� ��������
//			||UDRIE		-	���������� ���������� ������ ��� ��������
//			|||RXEN		-	���������� �����
//			||||TXEN	-	���������� ��������
//			|||||UCSZ2	-	UCSZ0:2 ������ ����� ������
//			||||||RXB8	-	9 ��� �������� ������
//			|||||||TXB8	-	9 ��� ���������� ������
//			76543210
	UCSRB=0b00011000;	//	�������� ���� � �������� �� UART

//			URSEL		-	������ 1
//			|UMSEL		-	�����:1-���������� 0-�����������
//			||UPM1		-	UPM0:1 ��������
//			|||UPM0		-	UPM0:1 ��������
//			||||USBS	-	��� ����: 0-1, 1-2
//			|||||UCSZ1	-	UCSZ0:2 ������ ����� ������
//			||||||UCSZ0	-	UCSZ0:2 ������ ����� ������
//			|||||||UCPOL-	� ���������� ������ - ������������
//			76543210
	UCSRC=0b10000110;	//	8-������� �������
}

void init_pwm()
{
	TCCR1A = (1<<COM1A1) | (1<<WGM10);
	TCCR1B = (1<<CS10); //������������ = 1
	OCR1A = 0x00; //��������� ��������� 0
}

void int0_init( void )
{
	//����������� �� ������������ INT0 �� ��������� ������
	MCUCR |= (1<<ISC01)|(1<<ISC00);
	//��������� ������� ���������� INT0
	GICR |= (1<<INT0);
	sei();
}

ISR( INT0_vect )
{
	//int0_cnt++;
	PORTC = 0b00000001;			//	�������� ���� PC0 = 1 = Vcc
}


//	UART
void send_Uart(unsigned char c)//	�������� �����
{
	while(!(UCSRA&(1<<UDRE)))	//	���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_Uart_str(const char *s)//	�������� ������
{
	while (*s != 0) send_Uart(*s++);
}

void send_int_Uart(unsigned int c)//	�������� ����� �� 0000 �� 9999
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

unsigned char getch_Uart(void)//	��������� �����
{
	while(!(UCSRA&(1<<RXC)))	//	���������������, ����� ������� ��������
	{}
	return UDR;
}

int main(void)
{
	/*DDRC = 0xff;				//	������������� ����� C, PC0 - �����
	DDRB = 0xff;
	DDRD &= (1<<PD2);
	PORTD |= (1<<PD2);
	//init_pwm();
	init_extInt();*/
	/*init_UART();					//	������������� UART
	_delay_ms(1000);				//	�������� 1c
	send_Uart_str("alex-exe.ru");	//	�������� ������
	send_Uart(13);					//	������� ������
	send_int_Uart(2013);			//	�������� �����
	send_Uart(13);					//	������� ������
   
   send_Uart_str("AT+CWMODE=3");   //������������� ����� wi-fi ������+����� �������
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPMODE=0");  //������������� ����� �������������� ������ ������� ��� ip �������
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPMUX=1");  //������������� ����� ���������� ���������� ��� ip �������
   send_Uart(13);
   _delay_ms(500);
   
   send_Uart_str("AT+CIPSERVER=1,8888");  //������� ip ������ �� ����� 8888
   send_Uart(13);
   _delay_ms(500);
     
   char buff;
   */
		//��������� ����������
		unsigned long tmp;
		DDRC = 0b00000001;				//	������������� ����� C, PC0 - �����


		int0_init();

		

		while(1) {
			
			printf( "int0_cnt = %lu\r\n", tmp );
			//����� 1 �������
			PORTC = 0b00000000;			//	��������� ���� PC0 = 0 = Vcc
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
		// send_Uart_str("hello");  //������� ip ������ �� ����� 8888
		 //send_Uart(13);
		/*if(UCSRA&(1<<RXC))			//	���� ������ ���� �� UART
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
				//	�������� ����������� �� 1 ��������� ������
			// LED on					//	� ����� ������
			PORTC |= (1<<PC0);			//	�������� ���� PC0 = 1 = Vcc
			_delay_ms(50);				//	�������� 50��
			// LED off
			PORTC &= ~(1<<PC0);			//	��������� ���� PC0 = 0 = Vcc
		}	*/	
}