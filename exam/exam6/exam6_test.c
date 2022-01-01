
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned long timer0=0;
volatile unsigned char n1, n10, n100, n1000, i;
unsigned long cnt_count=0;
unsigned char cycle_time,fixed_cycle_time=0;
//@ int가 아니고 char이여야하는 이유는? int라도 작은 자료형(char, INT0)에서 큰 자료형(int cycle_time)으로 대입하는 것이므로 비트 손실없다.
unsigned char prev,now=0;
unsigned char flag_10ms,flag_50ms,flag_1s=0;

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[11]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0b11111111};


int main(void)
{	
// 7-degment
	DDRC = 0xFF;
	PORTC = 0xFF;	
	DDRD = 0xFF;
	PORTD = 0xFF;
// LED	
	DDRE = 0xFF;
	PORTE = 0xFF;
	
	TCCR0A = 0x03;		  //prescale x64로 counter0 계수 시작.
	//TCNT0: counter0에서 파형발생모드에 따라 계수한 결과.
	//TIMSK0 = 0x00;      //timer overflow interrupt enable: TOIE0=0, output compare match interrupt enable: OCIE0A=0
	
    while (1) 
    {
		cycle_time = TCNT0;
		//for(i=0; i<0xFF; i++);
		
		PORTE ^= 0b00000001;
		//digit_for_write(n1,n10,n100,n1000);

		check_loop();  //1.0ms   
		
    }
}

void check_loop()
{
	fixed_cycle_time = cycle_time + (unsigned char)250;
	
	if(fixed_cycle_time> cycle_time)
	{
		while((int)(fixed_cycle_time - TCNT0)>=0);
	}
	else
	{
		while((int)TCNT0!=0);
		while((int)(fixed_cycle_time - TCNT0)>=0);
	}
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
}

void CNT_CLK()  //0.004ms
{
	TCNT0=0;
	now=TCNT0;
	prev=now;
	
	while(prev==now)
	{
		prev=now;
		now=TCNT0;
		//PORTE^=0b00000001; 
	}
	//for(i=0; i<0xFF; i++);
	cnt_count++;
	//cnt_count==250000일때 1s.
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