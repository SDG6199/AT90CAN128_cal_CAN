/*
 * exam3.c
 *
 * Created: 2021-10-07 오후 1:08:55
 * Author : user
 */ 

#include <avr/io.h>

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[10]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000};
unsigned int i=0;
unsigned int j=0;
unsigned long k=0;

int main(void)
{
	DDRD=0xFF;
	DDRB=0xFF;
	PORTD= 0xF0;
	
    while (1) 
    {
		for(i=0; i<10; i++)
		{
			PORTB= num_arr[i];
			for(k=0; k<0x20000; k++);		
		}
    }
}

