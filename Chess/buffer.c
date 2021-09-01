#include "buffer.h"
#include "LCD_Ili9341.h"

// writes data buffer storage
int BUFF_store_data(Circular_buffer *buffer, char data)
{
	if (buffer->Q_size == BUFFER_LENGTH)
	{
		return BUFFER_ERROR;		// return buffer error if buffer is full	
	}else if(buffer->Q_size == 0)		// check if buffer is empty
	{
		//reset buffer pointers, add data to buffer
		buffer->rear = buffer->front = 0;
		buffer->storage[buffer->rear] = data;
		buffer->Q_size++;
	}
	else
	{
		//reset rear buffer if it overflows
		if(++buffer->rear == BUFFER_LENGTH)
			buffer->rear = 0;
		// add data to buffer
		buffer->storage[buffer->rear] = data;
		buffer->Q_size++;
	}
	return BUFFER_OK;			
	
}


// reads data from buffer storage and writes it to data location
int BUFF_get_data(Circular_buffer *buffer, char *data)
{
	// first check if buffer empty
	if (buffer->Q_size == 0)
	{
		return BUFFER_ERROR;	// return BUFFER_ERROR error code and exit this function
	}
	
	// else read and return data from the buffer "front"
	*data = buffer->storage[buffer->front];	//Get data from the queue "front"
	
	//update the number of elements in the queue
	buffer->Q_size--;
	
	// update the "front" value NOT using the modulo arithmetics but
	// using if-then instead because it is faster on uC
	// updates front
	buffer->front++;
	// if front overflows reset it on 0
	if (buffer->front >= BUFFER_LENGTH)
	{ 
		buffer->front = 0;
	}
	
	return BUFFER_OK;	// return BUFFER_OK error code
}


// returns number of items in buffer 
uint16_t BUFF_get_number_of_items(Circular_buffer *buffer)
{
	return buffer->Q_size;
}


