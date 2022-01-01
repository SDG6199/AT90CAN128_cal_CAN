/*
 * exam3.c
 *
 * Created: 2021-10-07 오후 1:08:55
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define REQUEST 1
#define NOT_REQUEST 0
#define REPLY 2
#define NUM_EVENT 5

volatile int req_INT0=REQUEST ,req_INT1=NOT_REQUEST;   //interrupt service routine 에 사용되는 변수의 자료형은 volatile int로 해야하는 것 같다.
//int req_INT0=REQUEST ,req_INT1=NOT_REQUEST;

ISR(INT0_vect)
{
	static long event=0;
	if(req_INT0==REQUEST)
	{
		event++;
		if(event>=NUM_EVENT)
		{
			event=0;
			req_INT0=REPLY;
		}
	}
}


int main(void)
{
	DDRB = 0xFF;
	PORTB = 0xFF;	

	EIMSK=0x01;    //PORTD의 D0(INT0)를 enable.
	EICRA=0x02;		//interrupt request generates at falling edge.
	//PORTD=0x01;  interrupt는 PORTD에서만 사용할 수 있다. pull up으로 사용하고자 할 때 PORTD=0x01;을 한다.(pull up/pull down으로 설정가능.) 하지만 회로상으로 pull up처리되었기 때문에 할 필요없다.
	
	sei();    //global interrupt enable bit활성화.
	req_INT0=REQUEST;
	
    while (1) 
    {
		if(req_INT0==REPLY)
		{
			PORTB ^= 0xFF; 
			req_INT0=REQUEST;
		}
    }
}

