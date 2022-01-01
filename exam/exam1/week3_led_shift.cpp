#include <avr/io.h>

unsigned char a;
signed char b;
unsigned long i;
unsigned long j;

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0xFF;
	while (1)
	{
		PORTB = 0x00;
		b = 0b10000000;   //-0x00
		for (j = 0; j < 8; j++)
		{
			for(i=0; i<0x20000; i++);  //delay
			PORTB = b;
			b = b >> 1;
		}
		a = 0b11111111;
		for (j = 0; j < 8; j++)
		{
			for(i=0; i<0x20000; i++);  //delay
			a = a << 1;
			PORTB = a;
		}
	}
	return 0;
}