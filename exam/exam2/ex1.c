
#include <avr/io.h>
//#include <util/delay.h>

//#define F_CPU 16000000UL

#define S1_DATA (PIND&0x01)  //PIND&0b00000001
#define S2_DATA (PIND&0x02)  //PIND&0b00000010
#define S3_DATA (PIND&0x04)  //PIND&0b00000100
#define S4_DATA (PIND&0x08)  //PIND&0b00001000
#define S5_DATA (PIND&0x10)  //PIND&0b00010000
#define S6_DATA (PIND&0x20)  //PIND&0b00100000
#define S7_DATA (PIND&0x40)  //PIND&0b01000000
#define S8_DATA (PIND&0x80)  //PIND&0b10000000

int main(void)
{
	unsigned int S1_time, S2_time, S3_time, S4_time, S5_time, S6_time, S7_time, S8_time;
	
	DDRD=0x00;  //set input
	DDRB=0xFF;  //set output
	PORTD=0xFF;   //³»ºÎ pull up
	PORTB=0xFF;
	
	while (1)
	{
		if(S1_DATA == 0x00)
		{
			S1_time++;
			
			if(S1_time>=10000)
			{
				S1_time=20000;
				PORTB&=0b11111110;
			}
		}
		else
		{
			S1_time=0;
			PORTB|=0b00000001;
		}
		
		//-----
		if(S2_DATA == 0x00)
		{
			S2_time++;
			
			if(S2_time>=10000)
			{
				S2_time=20000;
				PORTB&=0b11111101;
			}
		}
		else
		{
			S2_time=0;
			PORTB|=0b00000010;
		}
		//-----
		if(S3_DATA == 0x00)
		{
			S3_time++;
			
			if(S3_time>=10000)
			{
				S3_time=20000;
				PORTB&=0b11111011;
			}
		}
		else
		{
			S3_time=0;
			PORTB|=0b00000100;
		}
		
		
		//-----
		if(S4_DATA == 0x00)
		{
			S4_time++;
			
			if(S4_time>=10000)
			{
				S4_time=20000;
				PORTB&=0b11110111;
			}
		}
		else
		{
			S4_time=0;
			PORTB|=0b00001000;
		}
		//-----
		if(S5_DATA == 0x00)
		{
			S5_time++;
			
			if(S5_time>=10000)
			{
				S5_time=20000;
				PORTB&=0b11101111;
			}
		}
		else
		{
			S5_time=0;
			PORTB|=0b00010000;
		}
		//-----
		if(S6_DATA == 0x00)
		{
			S6_time++;
			
			if(S6_time>=10000)
			{
				S6_time=20000;
				PORTB&=0b11011111;
			}
		}
		else
		{
			S6_time=0;
			PORTB|=0b00100000;
		}
		//-----
		if(S7_DATA == 0x00)
		{
			S7_time++;
			
			if(S7_time>=10000)
			{
				S7_time=20000;
				PORTB&=0b10111111;
			}
		}
		else
		{
			S7_time=0;
			PORTB|=0b01000000;
		}
		//-----
		if(S8_DATA == 0x00)
		{
			S8_time++;
			
			if(S8_time>=10000)
			{
				S8_time=20000;
				PORTB&=0b01111111;
			}
		}
		else
		{
			S8_time=0;
			PORTB|=0b10000000;
		}
	}
}