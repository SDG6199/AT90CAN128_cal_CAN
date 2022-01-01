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

volatile unsigned long timer0=0;
volatile unsigned char n1, n10, n100, n1000, i;
unsigned int count=0;
unsigned int cnt_count=0;
unsigned int cycle_time=0;
unsigned int fixed_cycle_time=0;
unsigned char prev,now,a,b=0;

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[11]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0b11111111};

void check_loop();
 
//void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d);

/*
ISR(TIMER0_OVF_vect)
{
	timer0++;
	
	if(timer0%10==0) //10ms
	{
		flag_10ms=1; 
	}
	if(timer0%50==0) //50ms
	{
		flag_50ms=1;
	}
	if(timer0%1000==0)  // 1s
	{
		timer0=0;
		flag_1s=1;
	}
}*/

int main(void)
{	
	DDRC = 0xFF;
	PORTC = 0xFF;	
	DDRD = 0xFF;
	PORTD = 0xFF;
	DDRE = 0xFF;
	PORTE = 0xFF;
	
	//TCNT1: 파형발생모드에 따라 프리스케일된 클록의 값이 변경.
	TCCR1B = 0x03;		  //1.024ms
	//TIMSK0 = 0x00;        //인터럽트 활성비트 TOIE0=1
		
	
    while (1) 
    {
		/*
		
		if(TCNT0%10==0) //%10: 0.04ms 여야함.  //? %10: 5.8ms(주기:11.6ms), %100: 61.2ms(주기:122.4ms)
		{
			TCNT0=0;
			cnt_count++;	
			PORTE^=0b00000001;
		}
		
		a=TCNT0;
		
		
		for(i=0; i<0xFF; i++);
		
		//
		//main state
		//main state의 주기가 1ms보다 작아야 함. main state의 주기가 1.024ms를 넘어갈 경우(b overflow)?
		//b=TCNT0;
		while(TCNT0-a<250)   //256>b-a>250이여야 탈출가능.
		{	
		}
		PORTE^=0b00000001;
		*/
		
		PORTE ^= 0b00000001;
		
		check_loop();
		
		//digit_for_write(n1,n10,n100,n1000);
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
	/*
	while((int)TCNT0>=255)
	{
		TCNT0=0;
	}*/
}

void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
	PORTC = seg_arr[0];
	PORTD = num_arr[a];
	
	for(i=0; i<0xFF; i++);
	//_delay_us(50);
	
	PORTC = seg_arr[1];
	PORTD = num_arr[b];
	
	for(i=0; i<0xFF; i++);
	//_delay_us(50);
	
	PORTC = seg_arr[2];
	PORTD = num_arr[c];
	
	for(i=0; i<0xFF; i++);
	//_delay_us(50);
	
	PORTC = seg_arr[3];
	PORTD = num_arr[d];
	for(i=0; i<0xFF; i++);
	//_delay_us(10);
		
	PORTC = 0xFF;
}