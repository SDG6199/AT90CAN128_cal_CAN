#include <avr/io.h>

int main(void)
{
	DDRB=0xFF;
	PORTB=0x00;
	unsigned long i;
	unsigned long count = 0;
	
	while (1)
	{
		for(i=0; i<0x20000; i++);  //delay
		count++;

		if (count == 17)
		{
			count = 1;
		}
		switch(count)
		{
			case 1:
			PORTB = 0b00000000;
			break;
			case 2: case 16:
			PORTB = 0b10000000;
			break;
			case 3: case 15:
			PORTB = 0b11000000;
			break;
			case 4: case 14:
			PORTB = 0b11100000;
			break;
			case 5: case 13:
			PORTB = 0b11110000;
			break;
			case 6: case 12:
			PORTB = 0b11111000;
			break;
			case 7: case 11:
			PORTB = 0b11111100;
			break;
			case 8: case 10:
			PORTB = 0b11111110;
			break;
			case 9:
			PORTB = 0b11111111;
			break;
		}
	}
	return 0;
}