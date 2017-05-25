#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
 
//переменная счетчик
volatile unsigned long int0_cnt = 0;

//настройка внешнего прерывния INT0
void int0_init( void )
{
  //настраиваем на срабатывание INT0 по переднему фронту
  MCUCR |= (1<<ISC01)|(1<<ISC00);
  //разрешаем внешнее прерывание INT0 
  GICR |= (1<<INT0);
}



//функция обработчик внешнего прерывания INT0
ISR( INT0_vect )
{
  int0_cnt++;
}



//настройка UART
void uart_init( void )
{
  //настройка скорости обмена
  UBRRH = 0;
  UBRRL = 3; //115200 при кварце 7.3728 МГц
  //8 бит данных, 1 стоп бит, без контроля четности
  UCSRC = ( 1 << URSEL ) | ( 1 << UCSZ1 ) | ( 1 << UCSZ0 );
  //разрешить прием и передачу данных
  UCSRB = ( 1 << TXEN ) | ( 1 <<RXEN );
}


int uart_putc(  char c, FILE *file )
{
  //ждем окончания передачи предыдущего байта
  while( ( UCSRA & ( 1 << UDRE ) ) == 0 );
  UDR = c;
  return 0;
}

FILE uart_stream = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE );
 

int main( )
{
  //временная переменная
  unsigned long tmp;

  stdout =  &uart_stream;

  int0_init();
  uart_init();

  sei();

  while(1) {
    //на время копирования значения счетчика запрещаем прерывания
    cli();
    tmp = int0_cnt;
    //разрешаем прерывания
    sei(); 
    printf( "int0_cnt = %lu\r\n", tmp );
    //пауза 1 секунда
    _delay_ms( 1000 );   
  }
  return 0;
}
