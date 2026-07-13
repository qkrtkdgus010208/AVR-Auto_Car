/*
 * ultrasonic.c
 *
 * Created: 2026-06-17 오후 1:33:09
 *  Author: kccistc
 */ 

#include "ultrasonic.h"

// 각 센서별 거리 및 데이터 보관 배열 [0]:좌, [1]:정면, [2]:우
volatile int distance_cm[3] = {0, 0, 0};
volatile uint8_t falling_edge_flag = 0;

// 현재 측정 중인 센서를 기억하는 변수
volatile TypeSensorSelect current_sensor = LEFT_SENSOR;
char scm_buf[60];

// [INT4] 좌측 초음파 (PE4)
ISR(INT4_vect)
{
	if (ECHO_PORT & (1 << ECHO_LEFT)) // 상승 에지
	{
		TCNT3 = 0;
	}
	else // 하강 에지
	{
		uint32_t ultrasonic_distance = TCNT3 * 1000000.0 * 1024 / F_CPU;
		distance_cm[LEFT_SENSOR] = ultrasonic_distance / 58;
		falling_edge_flag = 1;
	}
}

// [INT5] 정면 초음파 (PE5)
ISR(INT5_vect)
{
	if (ECHO_PORT & (1 << ECHO_FRONT)) // 상승 에지
	{
		TCNT3 = 0;
	}
	else // 하강 에지
	{
		uint32_t ultrasonic_distance = TCNT3 * 1000000.0 * 1024 / F_CPU;
		distance_cm[FRONT_SENSOR] = ultrasonic_distance / 58;
		falling_edge_flag = 1;
	}
}

// [INT6] 우측 초음파 (PE6)
ISR(INT6_vect)
{
	if (ECHO_PORT & (1 << ECHO_RIGHT)) // 상승 에지
	{
		TCNT3 = 0;
	}
	else // 하강 에지
	{
		uint32_t ultrasonic_distance = TCNT3 * 1000000.0 * 1024 / F_CPU;
		distance_cm[RIGHT_SENSOR] = ultrasonic_distance / 58;
		falling_edge_flag = 1;
	}
}

void init_ultrasonic(void)
{
	// TRIG 핀 출력 설정 (PA0, PA1, PA2)
	TRIG_DDR |= (1 << TRIG_LEFT) | (1 << TRIG_FRONT) | (1 << TRIG_RIGHT);
	
	// ECHO 핀 입력 설정 (PE4, PE5, PE6)
	ECHO_DDR &= ~((1 << ECHO_LEFT) | (1 << ECHO_FRONT) | (1 << ECHO_RIGHT));
	
	// 외부 인터럽트 4, 5, 6 제어 레지스터 설정 (EICRB)
	// ISC4, ISC5, ISC6 비트를 모두 '0 1'로 만들어 상승/하강 에지 둘 다 감지
	EICRB |= (1 << ISC40) | (1 << ISC50) | (1 << ISC60);
	EICRB &= ~((1 << ISC41) | (1 << ISC51) | (1 << ISC61));
	
	// 외부 인터럽트 4, 5, 6 마스크 허용
	EIMSK |= (1 << INT4) | (1 << INT5) | (1 << INT6);
	
	// Timer1 설정 (1024 분주)
	TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30)); // 초기화
	TCCR3B |= (1 << CS32) | (1 << CS30);
	
	current_sensor = LEFT_SENSOR;
	make_trigger(TRIG_LEFT);
}

void make_trigger(uint8_t sensor_pin)
{
	TRIG_PORT &= ~(1 << sensor_pin); // low
	_delay_us(2);
	TRIG_PORT |= (1 << sensor_pin); // high
	_delay_us(15);
	TRIG_PORT &= ~(1 << sensor_pin); // low
}

void ultrasonic_processing(void)
{
	if (falling_edge_flag == 1)
	{
		falling_edge_flag = 0;
		
		// 현재 끝난 센서 측정 결과 출력 및 다음 타자 트리거
		if (current_sensor == LEFT_SENSOR)
		{
			//sprintf(scm_buf, "L: %d cm, ", distance_cm[LEFT_SENSOR]);
			//printf("%s", scm_buf);
			
			current_sensor = FRONT_SENSOR; // 다음은 정면
			make_trigger(TRIG_FRONT);
		}
		else if (current_sensor == FRONT_SENSOR)
		{
			//sprintf(scm_buf, "F: %d cm, ", distance_cm[FRONT_SENSOR]);
			//printf("%s", scm_buf);
			
			current_sensor = RIGHT_SENSOR; // 다음은 우측
			make_trigger(TRIG_RIGHT);
		}
		else if (current_sensor == RIGHT_SENSOR)
		{
			//sprintf(scm_buf, "R: %d cm\n", distance_cm[RIGHT_SENSOR]);
			//printf("%s", scm_buf);
						
			current_sensor = LEFT_SENSOR;  // 다시 좌측으로 턴 루프
			make_trigger(TRIG_LEFT);
		}
	}
}