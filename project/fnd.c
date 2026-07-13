/*
 * fnd.c
 *
 * Created: 2026-06-12 오전 10:52:36
 *  Author: kccistc
 */ 

#include "fnd.h"

#define ANIM_DISPLAY_UNIT 125
#define TOGGLE_DISPLAY_UNIT 500

static uint32_t ms_count = 0; // ms를 재는 count
static int32_t sec_count = 0; // sec를 재는 count

static uint32_t anim_ms_count = 0;
static uint8_t anim_index = 0;

static uint32_t last_fnd_time = 0;

					        //  0     1     2     3     4     5     6     7     8     9     .
static uint8_t fnd_font[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67, 0x80}; // common cathode
	
static uint8_t fnd_circle_anim_left[] = { 0x08, 0x04, 0x02, 0x01, 0x20, 0x10}; // common cathode
static uint8_t fnd_circle_anim_right[] = { 0x10, 0x20, 0x01, 0x02, 0x04, 0x08 }; // common cathode

extern volatile uint32_t msec_count;
extern Drive_State cur_drive_state;
extern uint32_t left_count;
extern uint32_t right_count;
extern uint32_t back_count;

static FndDisplayMode current_fnd_mode = DISPLAY_DRIVE_STATE;

void init_fnd(void)
{
	// 데이터 포트 전체 출력 설정 및 초기화
	FND_DATA_DDR = 0xFF;
	FND_DATA_PORT = 0x00;
	
	// PORTF 자리수 제어 핀(4, 5, 6, 7) 출력 설정 및 끄기(High)
	FND_DIGIT_DDR_F |= (1 << FND_DIGIT_F4) | (1 << FND_DIGIT_F5) | (1 << FND_DIGIT_F6) | (1 << FND_DIGIT_F7);
	FND_DIGIT_PORT_F |= (1 << FND_DIGIT_F4) | (1 << FND_DIGIT_F5) | (1 << FND_DIGIT_F6) | (1 << FND_DIGIT_F7);
	
	// PORTC 자리수 제어 핀(0, 1, 2, 3) 출력 설정 및 끄기(High)
	FND_DIGIT_DDR_D |= (1 << FND_DIGIT_D4) | (1 << FND_DIGIT_D5) | (1 << FND_DIGIT_D6) | (1 << FND_DIGIT_D7);
	FND_DIGIT_PORT_D |= (1 << FND_DIGIT_D4) | (1 << FND_DIGIT_D5) | (1 << FND_DIGIT_D6) | (1 << FND_DIGIT_D7);
}

// 버튼 조작 시 외부에서 호출해줄 함수
void fnd_change_display_mode(void)
{
	if (cur_drive_state == STOP)
	{
		current_fnd_mode = (current_fnd_mode + 1) % 4;
		
		if (current_fnd_mode == DISPLAY_DRIVE_STATE)
		{
			type_mode = (type_mode == MANAUL_MODE) ? AUTO_MODE : MANAUL_MODE;
			led_auto_mode(type_mode);
		}
	}
}

int fnd_processing(void)
{
	if (msec_count - last_fnd_time >= 1)
	{
		last_fnd_time = msec_count;
		
		if (cur_drive_state != STOP)
		{
			// 주행이 시작되면 조회 모드를 다시 기본 상태로 강제 리셋
			current_fnd_mode = DISPLAY_DRIVE_STATE;
			
			ms_count++;
			anim_ms_count++;
			
			if (anim_ms_count >= ANIM_DISPLAY_UNIT)
			{
				anim_ms_count = 0;
				anim_index = (anim_index + 1) % 6;
			}
			
			if (ms_count >= 1000)
			{
				ms_count = 0;
				sec_count++;
			}
		}
		
		static uint8_t toggle_fnd = 0;
		if (toggle_fnd == 0)
		{
			fnd_stop_watch();
			toggle_fnd = 1;
		}
		else
		{
			// 주행 여부를 판별
			if (cur_drive_state != STOP)
			{
				fnd_drive_direction(); // 주행 중일 때는 방향/애니메이션 표시
			}
			else
			{
				fnd_record_display();  // 정지 상태일 때는 버튼 기록 조회 표시
			}
			toggle_fnd = 0;
		}
	}
	
	return 0;
}

// 잔상 제거 및 초기화
void fnd_clear(void)
{
	FND_DATA_PORT = 0x00;
	FND_DIGIT_PORT_F = 0xF0;
	FND_DIGIT_PORT_D = 0xF0;
}

void fnd_select_digit(int digit_select)
{
	switch (digit_select)
	{
		case 0: FND_DIGIT_PORT_D = 0xEF; break; // 오른쪽 끝 칸 (1의 자리)
		case 1: FND_DIGIT_PORT_D = 0xDF; break; // 10의 자리
		case 2: FND_DIGIT_PORT_D = 0xBF; break; // 100의 자리
		case 3: FND_DIGIT_PORT_D = 0x7F; break; // 왼쪽 끝 칸 (1000의 자리)
	}
}

void fnd_stop_watch(void)
{
	static int digit_select = 0; // 자리수 선택
	
	fnd_clear();
	
	switch (digit_select)
	{
		case 0: // 1 단위
			FND_DIGIT_PORT_F &= ~(1 << FND_DIGIT_F4);
			FND_DATA_PORT = fnd_font[(ms_count % 100) / 10];
			break;
		case 1: // 10 단위
			FND_DIGIT_PORT_F &= ~(1 << FND_DIGIT_F5);
			FND_DATA_PORT = fnd_font[ms_count / 100];
			break;
		case 2: // 100 단위
			FND_DIGIT_PORT_F &= ~(1 << FND_DIGIT_F6);
			FND_DATA_PORT = fnd_font[sec_count % 10] | fnd_font[10];
			break;
		case 3: // 1000 단위
			FND_DIGIT_PORT_F &= ~(1 << FND_DIGIT_F7);
			FND_DATA_PORT = fnd_font[(sec_count / 10) % 6];
			break;
	}
	
	digit_select = (digit_select + 1) % 4; // 다음 표시할 자리수
}

// 주행 중 진행 방향 및 애니메이션 표시 함수
void fnd_drive_direction(void)
{
	static int digit_select = 0;
	
	fnd_clear();
	
	fnd_select_digit(digit_select);
	
	// 주행 상태 그래픽 출력
	switch (cur_drive_state)
	{
		case FORWARD:  FND_DATA_PORT = fnd_circle_anim_right[anim_index]; break;
		case BACKWARD: FND_DATA_PORT = fnd_circle_anim_left[anim_index]; break;
		case LEFT:     FND_DATA_PORT = 0x0F; break; // [ [ [ [
		case RIGHT:    FND_DATA_PORT = 0x39; break; // ] ] ] ]
		default:       FND_DATA_PORT = 0x40; break; // - - - -
	}
	
	digit_select = (digit_select + 1) % 4;
}

// 주행 종료(STOP) 후 버튼 조작에 따른 기록 조회 함수
void fnd_record_display(void)
{
	static int digit_select = 0;
	
	fnd_clear();
	
	fnd_select_digit(digit_select);
	
	uint32_t target_count = 0;
	
	// 버튼 조회 모드 판별
	switch (current_fnd_mode)
	{
		case DISPLAY_BACK_COUNT:  target_count = back_count;  break;
		case DISPLAY_LEFT_COUNT:  target_count = left_count;  break;
		case DISPLAY_RIGHT_COUNT: target_count = right_count; break;
		case DISPLAY_DRIVE_STATE:
		default:                  target_count = 9999;        break;
	}
	
	if (target_count == 9999)
	{
		FND_DATA_PORT = 0x40; // 기본 정지 상태 표시 (- - - -)
	}
	else
	{
		// 10진수 자릿수 파싱 분할 출력
		switch (digit_select)
		{
			case 0: FND_DATA_PORT = fnd_font[(target_count / 1000) % 10]; break;
			case 1: FND_DATA_PORT = fnd_font[(target_count / 100) % 10];  break;
			case 2: FND_DATA_PORT = fnd_font[(target_count / 10) % 10];   break;
			case 3: FND_DATA_PORT = fnd_font[target_count % 10];          break;
		}
	}
	
	digit_select = (digit_select + 1) % 4;
}

void reset_count(void)
{
	ms_count = 0;
	sec_count = 0;
}
