/*
 * 08.AUTO_CAR.c
 *
 * Created: 2026-06-22 오전 10:33:36
 * Author : kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수
#include <stdio.h>
#include "button.h"
#include "led.h"
#include "fnd.h"
#include "uart0.h"
#include "uart1.h"
#include "ultrasonic.h"
#include "pwm.h"
#include "def.h"

#define OBSTACLE_DISTANCE  20  // 장애물 판단 거리

#define SPEED 400
#define TURN_SPEED 1500

void init_timer0(void);

void manual_mode(void);
void auto_mode(void);
void auto_mode_check(void);
void distance_check(void);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

volatile uint32_t msec_count = 0;

TypeMode type_mode = MANAUL_MODE;

extern volatile int distance_cm[3];

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0 6 ~ 256 : 250개 펄스 count하기 위해
	msec_count++; // 1ms count
}

void (*pfunc[]) () =
{
	manual_mode,
	auto_mode,
	auto_mode_check,
	distance_check
};

void manual_mode(void)
{
	switch (bt_data)
	{
		case 'F':
		case 'f':
			forward(SPEED); // 4us x 500 = 0.002sec (2ms)
			break;
		case 'B':
		case 'b':
			backward(SPEED);
			break;
		case 'L':
		case 'l':
			turn_left(TURN_SPEED);
			break;
		case 'R':
		case 'r':
			turn_right(TURN_SPEED);
			break;
		case 'S':
		case 's':
			stop();
			break;
		default:
			break;
	}
}

void auto_mode(void)
{
	distance_check();  // 초음파 센서 실시간 거리 갱신
	
	int left_dis  = distance_cm[0];
	int front_dis = distance_cm[1];
	int right_dis = distance_cm[2];
	
	int max_dis = front_dis; // 정면을 가장 멀다고 가정
	
	if (left_dis > max_dis)
	{
		max_dis = left_dis;  // 왼쪽이 더 멀다면 교체
	}
	if (right_dis > max_dis)
	{
		max_dis = right_dis; // 오른쪽이 더 멀다면 교체
	}
	
	// 만약 삼면이 다 벽에 막혔으면 후진
	if (max_dis <= OBSTACLE_DISTANCE)
	{
		backward(SPEED);
	}
	// 가장 넓은 최적의 방향을 선택해서 이동
	else
	{
		if (max_dis == front_dis)
		{
			forward(SPEED); // 정면이 가장 넓음 -> 직진
		}
		else if (max_dis == left_dis)
		{
			turn_left(TURN_SPEED); // 왼쪽이 가장 넓음 -> 좌회전
		}
		else if (max_dis == right_dis)
		{
			turn_right(TURN_SPEED); // 오른쪽이 가장 넓음 -> 우회전
		}
	}
}

void auto_mode_check(void)
{
	if (get_button(BUTTON0, BUTTON0PIN))
	{
		if (cur_drive_state != STOP)
		{
			mode_change();
		}
		else
		{
			fnd_change_display_mode();
		}
	}
}

void mode_change(void)
{
	stop();
	type_mode = (type_mode == MANAUL_MODE) ? AUTO_MODE : MANAUL_MODE;
	led_auto_mode(type_mode);
}

void distance_check(void)
{
	ultrasonic_processing();
}

int main(void)
{
	init_led();
	init_fnd();
	init_timer0();
	init_uart0();
	init_uart1();
	init_button();
	init_motor_driver();
	init_timer1_pwm(); // 모터 전용 타이머
	init_timer3_pwm(); // 초음파 전용 타이머
	init_ultrasonic();
	
	stdout = &OUTPUT; // printf가 동작할 수 있도록 stdout을 설정
	sei(); // 전역(대문) interrupt 허용
		
    while (1)
    {
		auto_mode_check();
		fnd_processing();
		pfunc[type_mode]();
    }
}

void init_timer0(void)
{
	TCNT0 = 6; // TCNT0 6 ~ 256 : 250개 펄스 count하기 위해
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; // 64분주
	TIMSK |= 1 << TOIE0; // TIMER0 Overflow INT
}


