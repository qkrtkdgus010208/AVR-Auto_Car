/*
 * pwm.h
 *
 * Created: 2026-06-18 오후 2:23:17
 *  Author: kccistc
 */ 


#ifndef PWM_H_
#define PWM_H_

#include "button.h"
#include "def.h"
#include <avr/interrupt.h>
#include <stdio.h>

#define MOTOR_PWM_DDR DDRB
#define MOTOR_LEFT_PORT_DDR 5 // OC1A
#define MOTOR_RIGHT_PORT_DDR 6 // OC1B

#define MOTOR_DIRECTION_PORT_DDR DDRF
#define MOTOR_DIRECTION_PORT PORTF

void init_timer1_pwm(void);
void init_timer3_pwm(void);
void init_motor_driver(void);

void forward(int speed);
void backward(int speed);
void turn_left(int speed);
void turn_right(int speed);
void stop(void);

#endif /* PWM_H_ */