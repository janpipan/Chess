#ifndef BUFFER_H_
#define BUFFER_H_

#include <avr/io.h>

// macro definitions for the buffer
#define BUFFER_LENGTH	100		// buffer lenght (number of characters buffer can hold)

// return error values
#define BUFFER_OK		1		// no error return value
#define BUFFER_ERROR	0		// return value if error occurred

typedef struct circular_buffers{
	volatile char *storage;
	volatile uint16_t rear;
	volatile uint16_t front;
	volatile uint16_t Q_size;
} Circular_buffer;

// function prototypes
int BUFF_store_data(Circular_buffer *buffer, char data);		// store one byte of data (returns BUFFER_OK or BUFFER_ERROR)
int BUFF_get_data(Circular_buffer *buffer, char *data);			// read one byte of data (returns BUFFER_OK or BUFFER_ERROR)
uint16_t BUFF_get_number_of_items(Circular_buffer *buffer);		// Get the number of bytes currently in the buffer



#endif /* BUFFER_H_ */