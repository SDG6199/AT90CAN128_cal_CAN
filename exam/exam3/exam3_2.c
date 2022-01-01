#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[11]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0b11111111};
unsigned int count=0;

#define DELAY 100

void digit_for_write(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
	PORTD = seg_arr[0];
	PORTB = num_arr[a];
	_delay_ms(1);
	
	PORTD = seg_arr[1];
	PORTB = num_arr[b];
	_delay_ms(1);
	
	PORTD = seg_arr[2];
	PORTB = num_arr[c];
	_delay_ms(1);
	
	PORTD = seg_arr[3];
	PORTB = num_arr[d];
	_delay_ms(1);
	
	PORTD = 0b1111;
	_delay_ms(1);
}

int main(void)
{
	unsigned char n1, n10, n100, n1000;
	
	DDRD=0xFF;
	DDRB=0xFF;
	
	PORTD = 0xFF;
	PORTB = 0xFF;

	while (1)
	{
		count++;
		
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
		
		_delay_ms(DELAY);
		digit_for_write(n1,n10,n100,n1000);
	}
}

