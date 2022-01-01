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

unsigned int adc_0;
unsigned int adc_1;

char lcd_1[10];
char lcd_2[10];

int main(void)
{
	DDRC=0xFF;
	
	CLID_Initialize();
	
    while (1) 
    {
		ADMUX = 0x00;
		ADCSRA=0b11000111;
		while((ADCSRA&0x10)!=0x10){};
		adc_0=ADC;   //10bit
		sprintf(lcd_1,"ADC0: %4d",adc_0);  
		CLCD_String(0,0,lcd_1);
	
		ADMUX = 0x01;
		ADCSRA=0b11000111;
		while((ADCSRA&0x10)!=0x10){};    //ADC Interrupt Flag가 set될 때까지 대기.
		adc_1=ADC;   //10bit
		sprintf(lcd_2,"ADC1: %4d",adc_1);
		CLCD_String(1,0,lcd_2);
			
		//CLCD_Clear();
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
void CLID_Initialize()
{
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
void CLCD_String(unsigned char x, unsigned char y, char *string)
{
	CLCD_xy(x,y);
	
	while(*string !='\0')
	{
		CLCD_ASCII(*string);
		string++;
	}
}
void CLCD_Clear(void)
{
	unsigned char i;
	CLCD_Command(0x80);
	for(i=0;i<=15; i++){CLCD_Data(' ');}
	CLCD_Command(0x80+0x40);
	for(i=0;i<=15; i++){CLCD_Data(' ');}
}