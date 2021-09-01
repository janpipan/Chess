/*
 * joystick.c
 *
 * Created: 20/07/2021 23:41:09
 *  Author: Jan
 */ 

#include "joystick.h"

void ADC_Init()
{
	// set IO pins of joystick to high-Z state without pullup
	DDRC &= ~(1 << DDRC2) & ~(1 << DDRC3);
	PORTC &= ~(1 << PORTC2) & ~(1 << PORTC3);
	// disable digital input buffer
	// setting bit to one disables ADC pin
	DIDR0 = 0x0C;
	// set voltage reference to AVCC with external capacitor
	ADMUX = 0x40;
	// set ADC prescaler to 128 and enable ADC
	// prescaler must be set to closest value to 50kHz and 200kHz
	ADCSRA = 0x87;
}

void ADC_SelectChannel(uint8_t channel)
{
	// selects input for ADC, variable must be 4-bit
	ADMUX &= ~(0b1111 << MUX0);
	ADMUX |= (channel << MUX0);
}

int ADC_Read()
{
	//starts ADC conversion
	// when ADC conversion is completed bit is set to zero
	ADCSRA |= (1 << ADSC);
	// wait for ADC to complete when ADSC is set to zero we can read the ADC
	while((ADCSRA & (1 << ADSC)) != 0){};
	// return combined ADCH and ADCL registers
	return ADC;
}

int ADC_ReadChannel(uint8_t channel)
{
	// selects ADC channel and reads value form ADC
	ADC_SelectChannel(channel);
	_delay_us(10);
	return ADC_Read();
}

int JOY_read_axis(uint8_t axis)
{
	// gets value from ADC for specified axis
	return ADC_ReadChannel(axis);
}

// adds offset to a square based on where the joystick was moved
int update_square_position(int square, int x_pos, int y_pos)
{
	// if joystick is not moved square has the same position else add offset
	int offset = 0;
	if (x_pos > 800) offset = 1;
	if (x_pos < 270) offset = -1;
	if (y_pos > 800) offset = 16;
	if (y_pos < 270) offset = -16;
	square += offset;
	if (square & 0x88) square -=offset;
	return square;
}