/*
 * exam3.c
 *
 * Created: 2021-10-07 오후 1:08:55
 * Author : user
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//volatile unsigned long timer0=0;
unsigned long timer0=0;

volatile unsigned char n1, n10, n100, n1000, i;
volatile unsigned int count=0;

volatile unsigned char flag_timerovf=0;
//volatile unsigned char flag_10ms=0;
unsigned char flag_10ms=0;
volatile unsigned char flag_50ms=0;
//volatile unsigned char flag_1s=0;
unsigned char flag_1s=0;
unsigned char duty=0;

unsigned int fixed_cycle_time=0;
unsigned int cycle_time=0;

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[11]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0b11111111};
 
//void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d);


/*ISR(TIMER0_OVF_vect)
{
	//sub routine.
}*/

int main(void)
{	
	DDRC = 0xFF;
	PORTC = 0xFF;	
	DDRD = 0xFF;
	PORTD = 0xFF;
	DDRE = 0xFF;
	PORTE = 0xFF;
	
	DDRB=0xFF;
	PORTB = 0xFF;
	//TCNT0: 파형발생모드에 따라 프리스케일된 클록의 값이 변경.
	TCCR0A = 0b01101011;		  //1.024ms
	TIMSK0 = 0b00000000;        //인터럽트 활성비트 TOIE1=1 
	TCCR1B = 0x03;
	//TIMSK1 = 0b00000001;        

	OCR0A=128;
	
	sei();

    while (1)  //main routine.
    {
		duty++;
		OCR0A=duty;
		
		check_loop();  //10ms
    }
}
void check_loop()
{
	fixed_cycle_time = cycle_time + (unsigned int)2500;
				
	while(((int)(fixed_cycle_time - TCNT1)>=0))
	{

	}
	TCNT1=0;
	cycle_time = TCNT1;

	timer0++;
}
void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
	PORTC = seg_arr[0];
	PORTD = num_arr[a];
	
	for(i=0; i<0x80; i++);
	//_delay_us(10);
	
	PORTC = seg_arr[1];
	PORTD = num_arr[b];
	
	for(i=0; i<0x80; i++);
	//_delay_us(10);
	
	PORTC = seg_arr[2];
	PORTD = num_arr[c];
	
	for(i=0; i<0x80; i++);
	//_delay_us(10);
	
	PORTC = seg_arr[3];
	PORTD = num_arr[d];
	for(i=0; i<0x80; i++);
	//_delay_us(10);
		
	PORTC = 0xFF;
}