/*
 * fnd.h
 *
 * Created: 2026-06-12 오전 10:46:18
 *  Author: kccistc
 */ 

#ifndef FND_H_
#define FND_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수
#include "def.h"

typedef enum 
{
	DISPLAY_DRIVE_STATE, // 주행 중 방향 표시
	DISPLAY_BACK_COUNT,  // 후진 횟수 표시
	DISPLAY_LEFT_COUNT,  // 좌회전 횟수 표시
	DISPLAY_RIGHT_COUNT  // 우회전 횟수 표시
} FndDisplayMode;

// --- FND 데이터 포트 설정 ---
#define FND_DATA_DDR        DDRC
#define FND_DATA_PORT       PORTC

// --- FND 자리수 제어 포트 1 (PORTF 4~7번 핀) ---
#define FND_DIGIT_DDR_F     DDRF
#define FND_DIGIT_PORT_F    PORTF
#define FND_DIGIT_F4        4
#define FND_DIGIT_F5        5
#define FND_DIGIT_F6        6
#define FND_DIGIT_F7        7

// --- FND 자리수 제어 포트 2 (PORTC 0~3번 핀) ---
#define FND_DIGIT_DDR_D     DDRD
#define FND_DIGIT_PORT_D    PORTD
#define FND_DIGIT_D4        4
#define FND_DIGIT_D5        5
#define FND_DIGIT_D6        6
#define FND_DIGIT_D7        7

void init_fnd(void);
void fnd_change_display_mode(void);
int fnd_processing(void);
void fnd_stop_watch(void);
void fnd_drive_direction(void);
void fnd_record_display(void);
void reset_count(void);
void fnd_clear(void);
void fnd_select_digit(int digit_select);

#endif /* FND_H_ */