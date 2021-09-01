#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "buffer.h"

// UART macros
#define BAUD 9600 // baud rate
#define FOSC 16e06 // misko clock speed
#define MYUBRR	FOSC/16/BAUD -1 // UBRR formula for setting correct baud rate

//function definitions
void UART_init(unsigned int ubrr);


#endif