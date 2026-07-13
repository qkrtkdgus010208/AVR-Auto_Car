/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:02
 *  Author: kccistc
 */

#include "pwm.h"

uint8_t is_running = 0;
Drive_State cur_drive_state = STOP;
Drive_State pre_drive_state = STOP;

uint32_t left_count = 0;
uint32_t right_count = 0;
uint32_t back_count = 0;

// 16bit 1번 timer/counter를 사용
/*
	PWM 출력 신호
	============
	PB5: OC1A: 왼쪽 바퀴
	PB6: OC1B: 오른쪽 바퀴
	BTN0: auto / manual mode 설정
	
	방향 설정
	========
	1. 왼쪽 바퀴
		PORTF0 -- IN1 (DC motor Driver)
		PORTF1 -- IN2
	2. 오른쪽 바퀴
		PORTF3 -- IN3 (DC motor Driver)
		PORTF4 -- IN4
		
		IN1/IN3    IN2/IN4
		=======    =======
		   0          1    : 역회전
		   1          0    : 정회전
		   1          1    : stop
*/
void init_timer1_pwm(void)
{
	// 분주비 64 설정 (16MHz / 64 = 250kHz)
	TCCR1B |= (1 << CS11) | (1 << CS10);
	
	// 모드 14: 고속 PWM 모드 사용하겠다. timer1 (327p 표 14-5)
	TCCR1A |= 1 << WGM11;
	TCCR1B |= 1 << WGM12 | 1 << WGM13;
	
	// COM1A1, COM1B1을 켜서 PB5, PB6 핀으로 비반전 PWM 신호를 출력 350p 표15-7
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	
	ICR1 = 0x3ff; // 1023 * 4us ==> 4ms TOP 값
}

// 16bit 3번 timer/counter를 사용
/*
	PWM 출력 신호
	============
	PE3: OC3A
	PE4: OC3B: INT4 사용(초음파 센서)
	PE5: OC3C ==> DC MOTOR (PWM 출력)
	BTN1: speed-up
	BTN2: speed-down
*/
void init_timer3_pwm(void)
{
	// PE4 초음파 연결
	DDRE |= 1 << 3 | 1 << 5;
	
	// 모드 5: 8bit 고속 PWM 모드를 사용하겠다. timer3, 348p 표 15-6
	TCCR3A |= 1 << WGM30;
	TCCR3B |= 1 << WGM32;
	
	// 비반전 모드 top: 0x00ff 비교일치 값 (PWM) 지정 OCR3C, 350p 표 15-7
	TCCR3A |= 1 << COM3C1;
	
	// ---- 분주비 설정 ----
	// 분주비 64
	// 16000000Hz / 64 --> 250000HZ (250KHz)
	// T = 1 / f -> 1 / 250000Hz --> 0.000004sec ---> 4us
	// 250000Hz에서 256개의 펄스를 count하면 소요시간: 1.024ms
	// 250000Hz에서 128개의 펄스를 count하면 소요시간: 0.512ms
	// TCNT3: 0 ~ 255(0x00ff)까지 count한 후 0으로 다시 돌아간다.
	TCCR3B |= 1 << CS31 | 1 << CS30; // 분주비 64
	OCR3C = 0; // OCR (Output Compare Register): PWM 값
	// OCR3C가 50인 경우 Duty(High)가 몇 %인가?
	// Duty Cycle: (OCR3C / Top) x 100 = 50 / 255 x 100 = 19.61%
}

/*
	PE5: PWM control
	PF6: IN1 방향 설정
	PF7: IN2
*/
void init_motor_driver(void)
{
	// 출력 모드로 설정
	MOTOR_PWM_DDR &= ~(1 << 5 | 1 << 6); // 초기화
	MOTOR_PWM_DDR |= 1 << 5 | 1 << 6;
	MOTOR_DIRECTION_PORT_DDR &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT_DDR |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
	
	// 모터를 전진 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2; // 정회전
	//MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2 | 1 << 3 | 1 << 4; // 정지
}

void forward(int speed)
{
	is_running = 1;
	cur_drive_state = FORWARD;
	
	if (cur_drive_state != pre_drive_state)
	{
		pre_drive_state = cur_drive_state;
	}
	
	// 모터를 전진 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2; // 정회전
	
	OCR1A = OCR1B = speed; // OCR1A: PWM left OCR1B: PWM right
}

void backward(int speed)
{
	is_running = 1;
	cur_drive_state = BACKWARD;
	
	if (cur_drive_state != pre_drive_state)
	{
		pre_drive_state = cur_drive_state;
		back_count++;
	}
	
	// 모터를 후진 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 1 | 1 << 3; // 역회전
	
	OCR1A = OCR1B = speed; // OCR1A: PWM left OCR1B: PWM right
}

void turn_left(int speed)
{
	is_running = 1;
	cur_drive_state = LEFT;
	
	if (cur_drive_state != pre_drive_state)
	{
		pre_drive_state = cur_drive_state;
		left_count++;
	}
	
	// 모터를 좌회전 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2; // 정회전
	
	OCR1A = 0; // OCR1A: PWM left
	OCR1B = speed; // OCR1B: PWM right
}

void turn_right(int speed)
{
	is_running = 1;
	cur_drive_state = RIGHT;
	
	if (cur_drive_state != pre_drive_state)
	{
		pre_drive_state = cur_drive_state;
		right_count++;
	}
	
	// 모터를 우회전 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2; // 정회전
	
	OCR1A = speed; // OCR1A: PWM left
	OCR1B = 0; // OCR1B: PWM right
}

void stop(void)
{
	is_running = 0;
	cur_drive_state = STOP;
	
	if (cur_drive_state != pre_drive_state)
	{
		pre_drive_state = cur_drive_state;
	}
	
	// 모터를 stop 모드로
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); // 초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
	
	OCR1A = 0; // OCR1A: PWM left
	OCR1B = 0; // OCR1B: PWM right
}