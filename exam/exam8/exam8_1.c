/*
 * exam8.c
 *
 * Created: 2021-11-11 오전 11:56:36
 * Author : user
 */ 

#include <avr/io.h>
#include <stdio.h>

#define SETBIT(PORTx, PinNum) (PORTx |= (1<<PinNum))
#define CLEARBIT(PORTx, PinNum) (PORTx &= ~(1<<PinNum))

#define CLCD_DATA			(PORTA)
#define CLCD_DATA_DIR		(DDRA)
#define CLCD_CONTROL_DIR	(DDRC)
 
#define CLCD_E_1	SETBIT	(PORTC,0)   // BIT1
#define CLCD_E_0	CLEARBIT(PORTC,0)   // BIT1
#define CLCD_RW_1	SETBIT	(PORTC,1)   // BIT2
#define CLCD_RW_0	CLEARBIT(PORTC,1)   // BIT2
#define CLCD_RS_1	SETBIT	(PORTC,2)   // BIT3
#define CLCD_RS_0	CLEARBIT(PORTC,2)   // BIT3

#define CLCD_ASCII CLCD_Data


int main(void)
{
	DDRC=0xFF;
	DDRD=0X00;
	ADCSRA=0x11011110;
	
	PORTD&00000001
	CLID_Initialize();
	CLCD_String(1,3,"hello");
	CLCD_String(0,0,"hi");
	
    while (1) 
    {
    }
}

void delay_us(unsigned char time_us)
{
	register unsigned char i;
	
	for(i=0; i<time_us; i++)    //4cycle  (4tic)
	{
		asm volatile("PUSH R0");   //2cycle  (2tic)
		asm volatile("POP R0");    //2cycle  (2tic)
		asm volatile("PUSH R0");   //2cycle  (2tic)
		asm volatile("POP R0");    //2cycle  (2tic)
		asm volatile("PUSH R0");   //2cycle  (2tic)
		asm volatile("POP R0");    //2cycle  (2tic)
	}							   //total: 16cycle  (1us for 16Mhz)
}

void delay_ms(unsigned int time_ms)
{
	register unsigned int i;
		
	for(i=0; i<time_ms; i++)    //4cycle  (4tic)
	{
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	}											
}
void CLCD_Command(unsigned char command)		//lcd datasheet의 timing chart참고.
{
	CLCD_RS_0;			//instruction    
	CLCD_E_0;			//disable
	CLCD_RW_0;			//write
	CLCD_DATA_DIR=0xFF;	//output
	CLCD_DATA=command;  
	CLCD_E_1;			//enable
	delay_us(1);
	CLCD_E_0;			//disable
	delay_us(50);
}
void CLID_Initialize()
{
	delay_ms(50);
	CLCD_Command(0x38);  //0b00111000
	delay_ms(1);
	CLCD_Command(0x0C);  //0b00001100
	delay_ms(1);
	CLCD_Command(0x01);  //0b00000001
	delay_ms(2);
	CLCD_Command(0x06);  //0b00000110
	delay_ms(2);
}
void CLCD_Data(unsigned char data)		//lcd datasheet의 timing chart참고.
{
	CLCD_RS_1;			//data
	CLCD_E_0;			//disable
	CLCD_RW_0;			//write
	CLCD_DATA_DIR=0xFF;	//output
	CLCD_DATA=data;  
	CLCD_E_1;			//enable
	delay_us(1);
	CLCD_E_0;			//disable
	delay_us(50);
}
void CLCD_xy(unsigned char x, unsigned char y)
{
	unsigned char i;
	if(x==0)       {i=0x80		+y+0;}
	else if(x==1)  {i=0x80+0x40 +y+0;}
	else if(x==2)  {i=0x80		+y+20;}
	else           {i=0x80+0x40 +y+20;}
	
	CLCD_Command(i);
}
void CLCD_String(unsigned char x, unsigned char y, char *string)
{
	CLCD_xy(x,y);
	
	while(*string !='\0')
	{
		CLCD_ASCII(*string);
		string++;
	}
}