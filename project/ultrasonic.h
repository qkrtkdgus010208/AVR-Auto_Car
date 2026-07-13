/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:35
 *  Author: kccistc
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수

// --- TRIG 핀 설정 (PORTA) ---
#define TRIG_DDR     DDRA
#define TRIG_PORT    PORTA
#define TRIG_LEFT    0    // PA0
#define TRIG_FRONT   1    // PA1
#define TRIG_RIGHT   2    // PA2

// --- ECHO 핀 설정 (PORTE - 외부 인터럽트 4, 5, 6) ---
#define ECHO_DDR     DDRE
#define ECHO_PORT    PINE
#define ECHO_LEFT    4    // PE4 (external INT4)
#define ECHO_FRONT   5    // PE5 (external INT5)
#define ECHO_RIGHT   6    // PE6 (external INT6)

typedef enum 
{
	LEFT_SENSOR,
	FRONT_SENSOR,
	RIGHT_SENSOR
} TypeSensorSelect;

void init_ultrasonic(void);
void make_trigger(uint8_t sensor_pin);
void ultrasonic_processing(void);

#endif /* ULTRASONIC_H_ */