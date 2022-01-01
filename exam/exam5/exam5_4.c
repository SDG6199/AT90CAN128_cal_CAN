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
volatile unsigned int count=0;
volatile unsigned char flag_10ms=0;
volatile unsigned char flag_50ms=0;
volatile unsigned char flag_1s=0;

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[11]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0b11111111};
 
//void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d);

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
}

int main(void)
{	
	DDRC = 0xFF;
	PORTC = 0xFF;	
	DDRD = 0xFF;
	PORTD = 0xFF;
	DDRE = 0xFF;
	PORTE = 0xFF;
	
	//TCNT0: 파형발생모드에 따라 프리스케일된 클록의 값이 변경.
	TCCR0A = 0x03;		  //1.024ms
	TIMSK0 = 0x01;        //인터럽트 활성비트 TOIE0=1
		
	sei();
	
    while (1) 
    {
		if(flag_10ms==1)
		{
			flag_10ms=0;
			PORTE^=0b00000001;    //led1
		}
		else if(flag_50ms==1)
		{
			flag_50ms=0;
			PORTE^=0b00000010;    //led2
		}
		else if(flag_1s==1)
		{
			flag_1s=0;
			count++;
			PORTE^=0b00000100;     //led3
			
			if(count<10)
			{
				n1000 = 0;
				n100 = 0;
				n10 = 0;
				n1 = count;
			}
			else if (count<100)
			{
				n1000 = 0;
				n100 = 0;
				n10 = count/10;
				n1 = count%10;
			}
			else if(count<1000)
			{
				n1000 = 0;
				n100 = count/100;
				n10 = (count%100)/10;
				n1 = count%10;
			}
			else if(count<10000)
			{
				n1000 = count/1000;
				n100 = (count%1000)/100;
				n10 = (count%100)/10;
				n1 = count%10;
			}
			else
			{
				count = 0;
			}	
		}
		digit_for_write(n1,n10,n100,n1000);
    }
}

void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
	PORTC = seg_arr[0];
	PORTD = num_arr[a];
	
	//for(i=0; i<0xFF; i++);
	_delay_us(10);
	
	PORTC = seg_arr[1];
	PORTD = num_arr[b];
	
	//for(i=0; i<0xFF; i++);
	_delay_us(10);
	
	PORTC = seg_arr[2];
	PORTD = num_arr[c];
	
	//for(i=0; i<0xFF; i++);
	_delay_us(10);
	
	PORTC = seg_arr[3];
	PORTD = num_arr[d];
	//for(i=0; i<0xFF; i++);
	_delay_us(10);
		
	PORTC = 0xFF;
}