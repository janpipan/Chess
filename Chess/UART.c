/*
 * UART.c
 *
 * Created: 25/03/2021 13:01:47
 *  Author: Jan
 */ 
#include "UART.h"


// initializes UART communication
void UART_init(unsigned int ubrr)
{
	UBRR0H = (unsigned char) (ubrr>>8); //set baud rate
	UBRR0L = (unsigned char) ubrr;
	// enable receiver and transmitter, enable RX interrupt 
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); 
	// set asynchronous mode, data bits = 8, stop bits = 1, no parity, no interrupts
	UCSR0C = 0x06;
}	
