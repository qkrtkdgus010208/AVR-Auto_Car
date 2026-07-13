/*
 * uart0.c
 *
 * Created: 2026-06-16 오전 9:57:56
 *  Author: kccistc
 */ 

#include "uart0.h"
#include "led.h"

volatile int rear = 0;  // interrupt에서 queue에 data를 저장하는 위치값
volatile int front = 0;  // pc_command_processing에서 가져가는 위치값
volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

extern int led_func_state;

extern void (*fp[]) ();

// 278p 표12-3
// PC로부터 1byte가 들어오면 자동적으로 이곳으로 진입한다.
// 예) led_all_on\n 이면 11번 이것으로 진입한다.
ISR(USART0_RX_vect)
{
	volatile uint8_t data;
	static volatile int i = 0;
	
	data = UDR0; // UDR0의 내용이 data에 복사된 후 UDR0내용은 빈 상태로 된다.
	
	// queue full 상태
	if ((rear + 1) % QUEUE_SIZE == front % QUEUE_SIZE) return;
	
	if (data == '\n' || data == '\r')
	{
		rx_buff[rear][i] = '\0'; // 문장의 끝인 NULL을 넣는다.
		rear = (rear + 1) % QUEUE_SIZE; // 0 ~ 9
		i = 0; // 다음 string을 저장하기 위해서 i를 0으로 만든다.
	}
	else
	{
		rx_buff[rear][i++] = data;
	}
}

/*
1. 전송속도: 9600bps
2. start/stop 설정
3. RX(수신): interrupt로 설정
4. TX(송신): polling으로 설정
*/
void init_uart0(void)
{
	// 1. 전송속도: 9600bps
	UBRR0H = 0x00;
	UBRR0L = 207; // 9600bps
	UCSR0A |= 1 << U2X0; // 2배속 설정 (sampling 8)
	// UART0를 송신, 수신이 다 가능하고 RX가 INT가 가능하도록 설정한다.
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
}

// UART0로 1byte를 전송하는 함수
void UART0_transmit(uint8_t data)
{
	while (!(UCSR0A & 1 << UDRE0)); // data가 송신 중이면 끝날 때까지 기다림
	
	UDR0 = data; // HW 전송 register에 data를 송신한다.
}

void pc_command_processing(void)
{
	if (front != rear) // data가 rx_buff에 존재하는지 
	{
		printf("%s", rx_buff[front]);
		if (strncmp((char *)rx_buff[front], "led_shift_left_on", strlen("led_shift_left_on")) == 0)
		{
			led_func_state = 0;
		}
		else if (strncmp((char *)rx_buff[front], "led_shift_right_on", strlen("led_shift_right_on")) == 0)
		{
			led_func_state = 1;
		}
		else if (strncmp((char *)rx_buff[front], "led_shift_left_keep_on", strlen("led_shift_left_keep_on")) == 0)
		{
			led_func_state = 2;
		}
		else if (strncmp((char *)rx_buff[front], "led_shift_right_keep_on", strlen("led_shift_right_keep_on")) == 0)
		{
			led_func_state = 3;
		}
		else if (strncmp((char *)rx_buff[front], "led_flower_on", strlen("led_flower_on")) == 0)
		{
			led_func_state = 4;
		}
		else if (strncmp((char *)rx_buff[front], "led_flower_off", strlen("led_flower_off")) == 0)
		{
			led_func_state = 5;
		}
		
		front = (front + 1) % QUEUE_SIZE;
	}
	
	fp[led_func_state] ();
}