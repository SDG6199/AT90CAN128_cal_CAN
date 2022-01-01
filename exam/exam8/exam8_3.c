/*
 * exam8.c
 *
 * Created: 2021-11-11 오전 11:56:36
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
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
unsigned int fixed_cycle_time=0;
unsigned int cycle_time=0;
unsigned long timer0=0;

char lcd_1[10];
char lcd_2[10];

int main(void)
{
	DDRB=0xFF;
	PORTB= 0xFF;
	CLID_Initialize();
	TCCR0A = 0b01101011;		//1.024ms 마다 interrupt. 8bit. pwm용.
	//TIMSK0 = 0b00000000;      //인터럽트 활성x
	TCCR1B = 0x03;              //1.024ms 마다 interrupt. 16bit. check_loop용.
	//TIMSK1 = 0b00000001;

	OCR0A=128;   //normalize하면 0.5
	
	//sei();
    while (1) 
    {
		ADCSRA=0b11000111;
		ADMUX = 0x00;

		while((ADCSRA&0x10)!=0x10){};
		adc_0=ADC;   //10bit
		
		sprintf(lcd_1,"ADC0: %04d",adc_0);   //왜 PF1을 convert?
		CLCD_String(0,0,lcd_1);
				
		ADCSRA=0b11000111;
		ADMUX = 0x01;

		while((ADCSRA&0x10)!=0x10){};	
		OCR0A= (unsigned char)(((unsigned long)ADC*255)/1023);							//duty에 해당.
		adc_1=(unsigned long)ADC*100/1023;   //10bit
		sprintf(lcd_2,"PWM: %04d",adc_1);   
		CLCD_String(1,0,lcd_2);
		
		check_loop();  //10ms
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
	DDRC=0xFF;
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
void CLCD_Clear(void)
{
	unsigned char i;
	CLCD_Command(0x80);
	for(i=0;i<=40; i++){CLCD_Data(' ');}
	CLCD_Command(0x80+0x40);
	for(i=0;i<=40; i++){CLCD_Data(' ');}
}
void check_loop()
{
	fixed_cycle_time = cycle_time + (unsigned int)2500;
	
	while(((int)(fixed_cycle_time - TCNT1)>=0))
	{

	}
	TCNT1=0;
	cycle_time = TCNT1;

	timer0++;
}