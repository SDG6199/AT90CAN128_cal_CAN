/*
 * lcd.h
 *
 * Created: 2021-12-13 오후 10:54:24
 *  Author: sdg
 */ 


#ifndef LCD_H_
#define LCD_H_


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

unsigned int adc_0;
unsigned int adc_1;

char lcd_1[10];
char lcd_2[10];
char flag=0;

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
void CLID_Initialize()
{
	DDRC=0xFF;					// control output
	delay_ms(50);
	CLCD_Command(0x38);  //0b00111000. function set
	delay_ms(1);
	CLCD_Command(0x0C);  //0b00001100. display ON/OFF control
	delay_ms(1);
	CLCD_Command(0x01);  //0b00000001. display clear
	delay_ms(2);
	CLCD_Command(0x06);  //0b00000110. entry mode set
	delay_ms(2);
}
void CLCD_Command(unsigned char command)		//lcd datasheet의 timing chart참고.
{
	CLCD_RS_0;			//instruction code
	CLCD_E_0;			//disable
	CLCD_RW_0;			//write
	CLCD_DATA_DIR=0xFF;	//output
	CLCD_DATA=command;
	CLCD_E_1;			//enable
	delay_us(1);
	CLCD_E_0;			//disable
	delay_us(50);
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
	//else if(x==2)  {i=0x80		+y+20;}
	//else           {i=0x80+0x40 +y+20;}
	
	CLCD_Command(i);
}
void CLCD_Char(char ch)
{
	if(ch !='\0')
	{
		CLCD_ASCII(ch);
	}
}
void CLCD_String(char *string)
{
	CLCD_Clear();
	while(*string !='\0')
	{
		CLCD_ASCII(*string);
		string++;
	}
}
void CLCD_Result(signed int n)
{
	CLCD_Clear(); 
	if(n>0)
	{
		if(n >= 10){
			CLCD_Result(n / 10);  // Recursive Operation
			n = n % 10;
		}
		if(flag==1)
		{
			CLCD_Char('-');
			flag=0;
		}
		CLCD_Char(n + '0');  
	}
	else
	{
		flag=1;
		if(abs(n) >= 10){
			CLCD_Result(abs(n) / 10);  // Recursive Operation
		}
		if(flag==1)
		{
			CLCD_Char('-');
			flag=0;
		}
		n = abs(n) % 10;
		CLCD_Char(n + '0');
	}
}
void CLCD_Clear(void)
{
	unsigned char i;
	CLCD_Command(0x80);
	for(i=0;i<=15; i++){CLCD_Data(' ');}
	CLCD_Command(0x80+0x40);
	for(i=0;i<=15; i++){CLCD_Data(' ');}
	CLCD_xy(0,0);
}



#endif /* LCD_H_ */