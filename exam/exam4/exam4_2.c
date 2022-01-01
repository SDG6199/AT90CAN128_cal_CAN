/*
 * exam3.c
 *
 * Created: 2021-10-07 오후 1:08:55
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned long timer0;

ISR(TIMER0_OVF_vect)
{
	timer0++;
	
	if(timer0%100==0) PORTB^=0xFF;
}


int main(void)
{
	DDRB = 0xFF;
	PORTB = 0xFF;

	DDRD = 0xFF;
	PORTD = 0x00;

	//TCNT0: 파형발생모드에 따라 프리스케일된 클록의 값이 변경.
	TCCR0A = 0b00000101;   //일반모드. bit6(WGMn0),bit3(WGMn1)=(0,0), prescalex1: bit0~2=(1,0,1)
	TIMSK0 = 0x01;         //인터럽트 활성비트 TOIE0=1
		
	sei();
	
    while (1) 
    {

    }
}

