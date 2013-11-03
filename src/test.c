#include <arch/antares.h> 
#include <avr/io.h> 
#include <util/delay.h>  
#include <lib/printk.h>
#include <lib/earlycon.h>
#include "xmodem.h"

void delay()
{
	_delay_ms(1000);
}

int testWr(unsigned char* buf, int sz)
{
	return 0;
}

void USART_Init()
{
	int ubrr = 51; //103
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
		;
		/* Put data into buffer, sends the data */
		UDR = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) )
		;
		/* Get and return received data from buffer */
		return UDR;
}

unsigned char USART_Avail()
{
	return (UCSRA & (1<<RXC));
}


ANTARES_INIT_LOW(io_init) 
{ 
  DDRA=0xff;
  USART_Init();
}

ANTARES_APP(blink) 
{ 
  struct xmodem_receiver testRcv;
//  testRcv.get_char = early_getc;
//  testRcv.put_char = early_putc;
//  testRcv.char_avail = early_avail;
//  testRcv.delay_1s = delay;
//  testRcv.writer = testWr;
  testRcv.get_char = USART_Receive;
  testRcv.put_char = USART_Transmit;
  testRcv.char_avail = early_avail;
  testRcv.delay_1s = delay;
  testRcv.writer = testWr;
//  testRcv.put_char('A');
//  while(1)
//  {
//    PORTA^=0xff; 

//	printk("12345");

    _delay_ms(1000);

//	early_putc("Started new");

	xmodem_receive(&testRcv);
	while(1);;;
//	USART_Transmit(0x41);
//	early_putc(0x41);

//  }   
}

