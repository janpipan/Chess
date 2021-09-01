/*
 * joystick.h
 *
 * Created: 20/07/2021 23:41:21
 *  Author: Jan
 */ 

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <avr/io.h>
#include <util/delay.h>

#define X_AXIS 3
#define Y_AXIS 2

void Init_IO();
void ADC_Init();
int ADC_Read();
void ADC_SelectChannel(uint8_t channel);
int ADC_ReadChannel(uint8_t channel);

int JOY_read_axis(uint8_t axis);

int update_square_position(int square, int x_pos, int y_pos);




#endif /* JOYSTICK_H_ */