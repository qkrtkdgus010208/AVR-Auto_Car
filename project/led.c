/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"

int led_func_state = 0;

void init_led(void)
{
	DDRG |= (1 << 3); // PORTG.3 출력 모드로 설정
	PORTG &= ~(1 << 3); // PORTG.3 led를 off
}

void led_auto_mode(TypeMode mode)
{
	if (mode == MANAUL_MODE) // manual mode
		PORTG &= ~(1 << 3);
	else
		PORTG |= (1 << 3);
}