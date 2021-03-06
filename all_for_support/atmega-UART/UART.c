/*
 * UART.c
 *
 * Created: 31.03.2013 16:34:53
 *  Author: Alex_EXE
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL	//	������� �������
#endif

#include <avr/io.h>
#include <util/delay.h>		//	��� _delay_ms()

int init_UART(void)
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

//	UART
void send_Uart(unsigned char c)//	�������� �����
{
	while(!(UCSRA&(1<<UDRE)))	//	���������������, ����� ������� ��������
	{}
	UDR = c;
}

void send_Uart_str(unsigned char *s)//	�������� ������
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
	DDRC = 0b00000001;				//	������������� ����� C, PC0 - �����
	init_UART();					//	������������� UART
	_delay_ms(1000);				//	�������� 1c
	send_Uart_str("alex-exe.ru");	//	�������� ������
	send_Uart(13);					//	������� ������
	send_int_Uart(2013);			//	�������� �����
	send_Uart(13);					//	������� ������
    while(1)						//	����������� ������� ����
    {
		if(UCSRA&(1<<RXC))			//	���� ������ ���� �� UART
		{
			send_Uart(getch_Uart()+1);	//	�������� ����������� �� 1 ��������� ������
			// LED on					//	� ������ ������
			PORTC = 0b00000001;			//	�������� ���� PC0 = 1 = Vcc
			_delay_ms(50);				//	�������� 50��
			// LED off
			PORTC = 0b00000000;			//	��������� ���� PC0 = 0 = Vcc
		}		
    }
}