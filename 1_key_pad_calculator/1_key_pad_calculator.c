/*
#1. 계산기 만들기

결선
lcd: GND,Vcc,V0는 메인보드 lcd모듈순서대로, RS,RW,E는 PORTC.2,PORTC.1,PORTC.0순서, DB0~DB7은 PORTA.0~PORTA.7, 나머지 2개 pin은 백라이트 전원이므로 연결X
7-segment(debug): PORTB는 JP18(num) A~DP, PORTD는 JP17(seg) SEG0~SEG03
4x4 keypad: PORTE는 JP3(row) PORTE.0~PORTE.3(output) , JP1(col) PORTE.4~PORTE.7(input)
*/


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <lcd.h>  //https://blog.naver.com/PostView.nhn?blogId=dahye0209&logNo=222061424178&categoryNo=90&parentCategoryNo=0&viewDate=&currentPage=1&postListTopCurrentPage=1&from=search 참고.

#define S1_DATA (PINE&0x01)  //PINE&0b00000001
#define S2_DATA (PINE&0x02)  //PINE&0b00000010
#define S3_DATA (PINE&0x04)  //PINE&0b00000100
#define S4_DATA (PINE&0x08)  //PINE&0b00001000
#define S5_DATA (PINE&0x10)  //PINE&0b00010000
#define S6_DATA (PINE&0x20)  //PINE&0b00100000
#define S7_DATA (PINE&0x40)  //PINE&0b01000000
#define S8_DATA (PINE&0x80)  //PINE&0b10000000

unsigned char seg_arr[4]={0b1110, 0b1101, 0b1011, 0b0111};
unsigned char num_arr[14]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000, 0x88, 0x83, 0xc6, 0xa1};
char key,prev_key;
char index=0,num_key=1,error_flag=0,prev_operator=0,oper_flag=0;
char operator[32];
signed int int_num_data[32],result_num[32];

char lcd_temp[10];
char KeySet(void);

//sprintf(lcd_temp,"key: %c",key);

int main(void)
{
	DDRD = 0xff; PORTD= 0xf0;   //seg_arr
	DDRB = 0xff; PORTB= 0xff;   //num_arr

	CLID_Initialize();
	CLCD_xy(0,0);
	while (1)
	{
		key = KeySet();
		//Clear_temp_for_key(index);

		if(num_key==17) CLCD_xy(1,0);
		else if(num_key==33)
		{
			key='='; error_flag=1;
		}
		if(key!='n' && key!=prev_key)
		{
			switch (key)
			{
				case '0': PORTB = num_arr[0]; CLCD_Char(key); break;
				case '1': PORTB = num_arr[1]; CLCD_Char(key); break;
				case '2': PORTB = num_arr[2]; CLCD_Char(key); break;
				case '3': PORTB = num_arr[3]; CLCD_Char(key); break;
				case '4': PORTB = num_arr[4]; CLCD_Char(key); break;
				case '5': PORTB = num_arr[5]; CLCD_Char(key); break;
				case '6': PORTB = num_arr[6]; CLCD_Char(key); break;
				case '7': PORTB = num_arr[7]; CLCD_Char(key); break;
				case '8': PORTB = num_arr[8]; CLCD_Char(key); break;
				case '9': PORTB = num_arr[9]; CLCD_Char(key); break;
				case '+': PORTB = num_arr[10]; CLCD_Char(key); operator[index] = key; break;
				case '-': PORTB = num_arr[11]; CLCD_Char(key); operator[index]= key; break;
				case '*': PORTB = num_arr[12]; CLCD_Char(key); operator[index] = key;  break;
				case 'C': PORTB = num_arr[13]; CLCD_Clear(); Clear_parameter(index); break;
				case '=':
				{
					if(error_flag==0)
					{
						CLCD_Result(result_num[index]);
					}
					else
					{
						CLCD_String("error");
					}
					Clear_parameter(index);
					break;
				}
				default: PORTB = num_arr[0]; break;
			}
			if('0'<=key && key<='9')   //숫자인 경우
			{
				prev_operator=0;
				int_num_data[index] = 10 * int_num_data[index] + (key-'0');   //str -> int, int_num_data 십진법으로 누적.
				TEMP_Oper(index);
			}
			if(key=='+' || key=='-' || key=='*')     //operator인 경우
			{
				if(num_key==1 || prev_operator!=0)	//첫문장이 숫자가 아니거나 operator가 연속2번이상입력됬을때 error.
				{
					error_flag=1;
				}
				prev_operator=operator[index];
				index++;
			}
			num_key++;
		}
		prev_key=key;
		_delay_us(10);
	}
}

char KeySet(void)
{
	char KeyBuff = 'n';
	
	DDRE = 0x00; PORTE=0xff;  // PORTE input으로 초기화
	DDRE = 0x01; PORTE &= ~0x01;    // 1행에 low 신호.
	_delay_us(10);
	if(!(PINE&0x10)) KeyBuff = 'C';		//1열
	if(!(PINE&0x20)) KeyBuff = '7';		//2열
	if(!(PINE&0x40)) KeyBuff = '4';		//3열
	if(!(PINE&0x80)) KeyBuff = '1';		//4열
	
	DDRE=0x00; PORTE = 0xff;
	DDRE = 0x02; PORTE &= ~0x02;	// 2행에 low 신호.
	_delay_us(10);
	if(!(PINE&0x10)) KeyBuff = '0';
	if(!(PINE&0x20)) KeyBuff = '8';
	if(!(PINE&0x40)) KeyBuff = '5';
	if(!(PINE&0x80)) KeyBuff = '2';
	
	DDRE = 0x00; PORTE = 0xff;
	DDRE = 0x04; PORTE &= ~0x04;	// 3행에 low 신호.
	_delay_us(10);
	if(!(PINE&0x10)) KeyBuff = 'n';
	if(!(PINE&0x20)) KeyBuff = '9';
	if(!(PINE&0x40)) KeyBuff = '6';
	if(!(PINE&0x80)) KeyBuff = '3';
	
	DDRE = 0x00; PORTE = 0xff;
	DDRE = 0x08; PORTE &= ~0x08;	// 4행에 low 신호.
	_delay_us(10);
	if(!(PINE&0x10)) KeyBuff = '=';
	if(!(PINE&0x20)) KeyBuff = '*';
	if(!(PINE&0x40)) KeyBuff = '-';
	if(!(PINE&0x80)) KeyBuff = '+';
	
	return KeyBuff;
}
void Clear_temp_for_key(char n)
{
	/**/
}
void Clear_parameter(char n)
{
	for(int i=0; i<=n; i++)
	{
		int_num_data[i]='\0';
		result_num[i]=0;
	}
	index=0;
	num_key=0;
	error_flag=0;
	prev_operator=0;
	oper_flag=0;
}

void TEMP_Oper(char n)
{
	if(n>0)
	{
		switch(operator[n-1])
		{
			case '+':
			{
				result_num[n]=result_num[n-1]+int_num_data[n];
				break;
			}
			case '-':
			{
				result_num[n]=result_num[n-1]-int_num_data[n];
				break;
			}
			case '*':
			{
				if(n==1)
				{
					result_num[n]=int_num_data[n-1]*int_num_data[n];
				}
				else
				{
					if(operator[n-2]=='+')
					{
						result_num[n]=(result_num[n-1]-int_num_data[n-1])+int_num_data[n-1]*int_num_data[n];
					}
					else if(operator[n-2]=='-')
					{
						result_num[n]=(result_num[n-1]+int_num_data[n-1])-int_num_data[n-1]*int_num_data[n];
					}
					else if(operator[n-2]=='*')
					{
						int j=n; signed int b_mul_part=int_num_data[j]; signed int s_mul_part=0;
						while(operator[j-1]=='*') //operator가 *가 아니게 될때까지
						{
							b_mul_part*=int_num_data[j-1];
							s_mul_part=b_mul_part/int_num_data[n];
							j--;
						}
						if(operator[j-1]=='+')
						{
							result_num[n]=result_num[n-1]+(b_mul_part)-(s_mul_part);
						}
						else  //operator[j]=='-'
						{
							result_num[n]=result_num[n-1]-(b_mul_part)+(s_mul_part);
						}
					}
				}
				break;
			}
			
			default: break;
		}
	}
	else
	{
		result_num[n]=int_num_data[n];
	}
}

