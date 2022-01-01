#include <avr/io.h>

int main(void)
{
	unsigned long i;
	unsigned char a;
	DDRB=0xFF;
	PORTB=0x00;
	a=0b11111111;

	while (1)
	{
		for(i=0; i<0x20000; i++);  //delay
		PORTB =a;
		a^=0xFF;
	}
}