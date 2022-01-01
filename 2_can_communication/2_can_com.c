/* #2. CAN 통신 실습

1. PC에서 전송되는 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800,0x150, 0x250의 10개 CAN Frame Data를 수신하고, 
LED에 수신될 때마다 Toggle 할 것 (D9-0x100, D10-0x200, … D16-0x800, D23-0x150, D24-0x250)
2. AT90CAN128 보드에서 3개의 Tx Data 송출할 것
• 송대근 : 0x110(10msec), 0x210(50msec), 0x310(100msec)

결선:
MCU의 CAN pheriphral측에서 PORTD.5는 TXCAN, PORTD.6는 RXCAN이다. 이는 transcevier에 연결되며(할 필요x),
CAN_H, CAN_L는 can_to_usb케이블(kvaser+RS232)에 연결된다.

AT90CAN128 feature:
15 Mob(message object)를 담을 수 있는 data buffer.
(MOb는 CAN 프레임 기술자이다. 메일박스에 송수신되는 메시지의 동작모드 결정.)
id tag, id mask는 11bit(rev 2.0A) or 29bit(rev 2.0B)
8byte data buffer 

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

//CAN 관련 매크로  ======================================
#define STD		0x00		//standard frame
#define EXT		0x01		//extended frame

#define _SEND_FAIL	0
#define _SEND_OK	1

#define _RECE_FAIL	0
#define _RECE_OK	1

// CAN 보레이트(baud rate)===============================
#define b1M		1
#define b500k 	2
#define b250k	3
#define b200k	4
#define	b125k	5
#define b100k	6

//MOb 구조체 선언  ======================================
struct MOb
{
	unsigned long id;		//id set.
	unsigned char rtr;		//RTRTAG bit. rtr(remote transmission request tag)설정.
	unsigned char ide;		//IDT bit. identifier extention(std or ext mode)설정.
	unsigned char dlc;		//data length (0~8)
	unsigned char data[8];
};

struct MOb msg1={0x110,0,STD,8,{0,1,2,3,4,5,6,7}};
struct MOb msg2={0x210,0,STD,8,{0,1,2,3,4,5,6,7}};
struct MOb msg3={0x310,0,STD,8,{0,1,2,3,4,5,6,7}};

struct MOb rxmsg;

void delay_us(unsigned char time_us);
void delay_ms(unsigned int time_ms);
void initPort();
void can_init (char baudRate);
char can_tx (unsigned char obj, struct MOb *msg, char rtr);
char can_rx(unsigned char obj, struct MOb *msg);
void can_rx_set(char obj, unsigned long id, char ide, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask);
void can_int_rx_set(char obj, unsigned long id, char rplvIde, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask);
void check_loop();

volatile unsigned long timer0=0;		//@ timer flag initialize.
volatile unsigned char flag_10ms=0;
volatile unsigned char flag_50ms=0;
volatile unsigned char flag_100ms=0;
unsigned int cycle_time,fixed_cycle_time=0;    //@

int main(void)
{
	TCCR0A = 0x03;		    //@ 1.024ms마다 overflow. prescale x64로 counter0 계수 시작.
	//TIMSK0 = 0x01;        //인터럽트 활성비트 TOIE0=1
	cycle_time = TCNT0;		//@
	char count1=0,count2=0,count3=0;

	initPort();
	can_init(b500k);//b500k
	
	//can_rx_set(2, 200, STD, 8, 0x3FF, 0);
	can_int_rx_set(3, 0x100, STD, 8, 0x3FF, 0x00);  //@ can interrupt set
	can_int_rx_set(4, 0x200, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(5, 0x300, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(6, 0x400, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(7, 0x500, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(8, 0x600, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(9, 0x700, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(10, 0x800, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(11, 0x150, STD, 8, 0x3FF, 0x00);
	can_int_rx_set(12, 0x250, STD, 8, 0x3FF, 0x00);
	
	rxmsg.rtr=0;
	sei();
	
	while(1)
	{
		
		/*
		for(int i=0; i<10; i++)
		{
			can_rx(i,&rxmsg);
		}*/
		
		if(flag_10ms==1)	//@ loop1 (10ms)
		{
			flag_10ms=0;
			count1++;
			/*
			txmsg.id = 0x110;		//@ id: 0x110
			txmsg.dlc = 8;
			txmsg.rtr = 0;
			txmsg.ide = STD;
		
			txmsg.data[0] = count;
			txmsg.data[1] = 10;
			txmsg.data[2] = 20;
			txmsg.data[3] = 30;
			txmsg.data[4] = rxmsg.data[0];
			txmsg.data[5] = rxmsg.data[1];
			txmsg.data[6] = rxmsg.data[2];
			txmsg.data[7] = rxmsg.data[3];*/
			if(count1==10) count1=0;
			msg1.data[0]=0+count1;
		
			PORTA^=0x04;
			//can_tx(0,&txmsg,0);   //@ Mob1
			can_tx(0,&msg1,0);   //@ Mob1, id: 0x110
		}
		
		if(flag_50ms==1)	//@ loop2 (50ms)
		{		
			flag_50ms=0;
			count2++;
			
			if(count2==10) count2=0;
			msg2.data[0]=0+2*count2;
				
			PORTA^=0x08;
			can_tx(1,&msg2,0);   //@ Mob2, id: 0x210
			
		}
			
		if(flag_100ms==1)	//@ loop3 (100ms))
		{
			flag_100ms=0;
			count3++;
			
			if(count3==10) count3=0;
			msg3.data[0]=0+3*count3;
						
			PORTA^=0x10;
			can_tx(2,&msg3,0);   //@ Mob3, id: 0x310
		}
		check_loop();  //@ 1.0ms 
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

void initPort()
{
	DDRC = 0xff;
	PORTC = 0x00;     //@ output to led. 
	DDRA = 0xff;
	PORTA = 0x00;     //@ output to led.
	
	DDRG = 0xff;
	PORTG = 0x00;
	DDRE = 0xff;
	PORTE = 0xff;
}

void can_init (char baudRate)	// CAN초기화
{
	unsigned char i, j;
	
	CANGCON |= (1<<SWRES);	// CAN 제어기 리셋
	// CAN General Control Register
	
	//보레이트 설정 ==============================================
	switch(baudRate)
	{
		case b1M:
		CANBT1= 0x00;
		CANBT2= 0x0c;
		CANBT3= 0x37;
		break;
		case b500k:
		CANBT1= 0x02;
		CANBT2= 0x0c;
		CANBT3= 0x37;
		break;
		case b250k:
		CANBT1= 0x06;	// CAN보레이트 설정
		CANBT2= 0x0c;	// bit timing: datasheet 264 (check table)
		CANBT3= 0x37;	// 250kbps, 16 MHz CPU Clock(0.250usec)
		break;
		case b200k:
		CANBT1= 0x08;
		CANBT2= 0x0c;
		CANBT3= 0x37;
		break;
		case b125k:
		CANBT1= 0x0E;
		CANBT2= 0x0c;
		CANBT3= 0x37;
		break;
		case b100k:
		CANBT1= 0x12;
		CANBT2= 0x0c;
		CANBT3= 0x37;
		break;
	}
	
	for(i=0; i<15; i++)		// Reset all MObs
	{
		CANPAGE = (i<<4);	// MOBNB3~0
		// MOb Number Select(0~14)
		CANCDMOB = 0;		// ALL Disable MOb
		CANSTMOB = 0;		// Clear status
		CANIDT1 = 0;		// Clear ID
		CANIDT2 = 0;		// Clear ID
		CANIDT3 = 0;		// Clear ID
		CANIDT4 = 0;		// Clear ID
		CANIDM1 = 0;		// Clear mask
		CANIDM2 = 0;		// Clear mask
		CANIDM3 = 0;		// Clear mask
		CANIDM4 = 0;		// Clear mask

		for(j=0; j<8; j++)
		CANMSG = 0;		// CAN Data Message Register
		// Clear data
	}
	
	// Clear CAN interrupt registers
	CANGIE = 0;				// CAN General Interrupt Enable Register
	// None Interrupts
	CANIE1 = 0;				// CAN Enable INT MOb Registers 1
	// None Interrupts on MObs
	CANIE2 = 0;				// CAN Enable INT MOb Registers 2
	// None Interrupts on MObs
	CANSIT1 = 0;			// CAN Status INT MOb Registers 1
	// None Interrupts on MObs
	CANSIT2 = 0;			// CAN Status INT MOb Registers 2
	// None Interrupts on MObs

	CANGCON = (1<<TTC );	// TTC mode *******************************************
	CANGCON |= (1<<ENASTB);	// CAN General Control Register
	// Enable Mode (11 Recessive Bits has Been read)
	// Start CAN interface

	while (!(CANGSTA & (1<<ENFG))); // CAN General Status Register (Enable Flag)
	// Wait until module ready
}

char can_tx (unsigned char obj, struct MOb *msg, char rtr)	// CAN transmission
{
	char send_result = _SEND_FAIL;
	unsigned char i;	
	unsigned long can_id= msg->id;
	
								// Enable MOb1, auto increment index, start with index = 0
	CANPAGE = (obj<<4);			// CAN Page MOb Register
								// MOb Number Select
	CANSTMOB = 0x00;		//CAN MOb Status Register
	CANCDMOB = 0x00;		//CAN MOb Control and DLC Register

	if(msg->ide== 0x00)	// standard
	{
		CANIDT1= (unsigned char)(can_id>>3);
		CANIDT2= (unsigned char)(can_id<<5);

		CANCDMOB &= ~0x10;		// Set IDE bit 2.0A 11bits
		//usart0_transmit_string("\rstandard\n");
	}
	else	// extended
	{
		CANIDT1= (unsigned char)(can_id>>21);
		CANIDT2= (unsigned char)(can_id>>13);
		CANIDT3= (unsigned char)(can_id>>5);
		CANIDT4= (unsigned char)(can_id<<3);

		CANCDMOB |= 0x10;		// Set IDE bit 2.0B 29bits
		//usart0_transmit_string("\rExtended\n");
	}

	CANCDMOB |= (msg->dlc<<DLC0);	// set data length

	CANIDT4 |= (rtr & 0x04);     // RTRTAG 설정;

	CANIDT4 &= ~0x02;		   // RB1TAG=0;
	CANIDT4 &= ~0x01;		   // RB0TAG=1;

	//put data in mailbox
	for(i=0; i<msg->dlc; i++)
		CANMSG = msg->data[i];	// full message 

	//enable transmission		
	CANCDMOB |= (1<<CONMOB0);


	//_delay_ms(10);
	//PORTE=0xAA;
	if(	(CANSTMOB & (1<<TXOK)) )
	{
		PORTE=0x00;
	}


//	PORTE=0x00;
//	while (!(CANSTMOB & (1<<TXOK)))
//	{
//		PORTE= (CANSTMOB & (1<<TXOK)) ? 0x00 : 0xff;
//	}	// check tx ok


/*
	int OnOff = 0;
	while(1)
	{	

		OnOff = OnOff ? 0 : 1;
		PORTE = OnOff ? 0xAA : 0x55;
		_delay_ms(1000);

	}
*/

	send_result= _SEND_OK;

	// monitoring with serial com
	// usart1_transmit_string("\rTXOK\n");

	//reset flag
	CANSTMOB &= ~(1<<TXOK);

	return(send_result);
}

char can_rx(unsigned char obj, struct MOb *msg)
{
	char rece_result = _RECE_FAIL;
	unsigned char i;
	unsigned long can_id= 0;
	
	CANPAGE = (obj<<4);				// 수신 MOb 선택

	//usart1_transmit_string("\rRX MOb #");
	//usart1_transmit(obj+0x30);
	//usart1_transmit_string("\r\n");

	// 수신 완료될 때까지 대기함
	
	//while(!(CANSTMOB&(1<<RXOK)));
	
	// 수신이 완료되면
	// CANIDT, CANCDMOB, CANMSG에 수신 메시지가 저장됨
	// get CANIDT and CANCDMOB and CANMSg
	//usart1_transmit_string("\rRXOK\n");
	rece_result = _RECE_OK;

	// 표준 혹은 확장 포맷에 맞추어 ID, IDE 결정
	if((CANCDMOB & 0x10) == 0x00){			// IDE standard ?
		msg->ide= STD;
		can_id  = ((unsigned long)CANIDT1)<<8;
		can_id |= ((unsigned long)CANIDT2);
		can_id>>=5;
		//usart1_transmit_string("\rRx Standard\n");
	}
	else{
		msg->ide= EXT;
		can_id  = ((unsigned long)CANIDT1)<<24;
		can_id |= ((unsigned long)CANIDT2)<<16;
		can_id |= ((unsigned long)CANIDT3)<<8;
		can_id |= ((unsigned long)CANIDT4);
		can_id>>=3;
		//usart1_transmit_string("\rRx Extended\n");
	}
	msg->id= can_id;			// 구조체 변수로 CANID 대입

	msg->rtr= CANIDT4 & 0x04;

	msg->dlc= CANCDMOB & 0x0f;	// 수신 메시지 길이 구조체 변수에 대입

	// get data
	for(i=0; i<(CANCDMOB&0xf); i++){
		msg->data[i] = CANMSG;	// 메시지 데이터 배열에 저장
	}

	// rx init
	CANSTMOB = 0x00;			// 상태 초기 화

	// enable reception mode and ide set
	CANCDMOB |= (1<<CONMOB1); 	// 수신 IDE 유지하고 수신 모드 설정

	// reset flag
	CANSTMOB &= ~(1<<RXOK);		// 수신대기

	return(rece_result);
}
void can_rx_set(char obj, unsigned long id, char ide, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask)
{
	CANPAGE = obj<<4;		// set MOb number

	CANSTMOB = 0x00;		// clear status

	if(ide== STD)			// standard
	{
		CANIDT1= (unsigned char)(id>>3);
		CANIDT2= (unsigned char)(id<<5);

		CANIDM1= (unsigned char)(idmask>>3);
		CANIDM2= (unsigned char)(idmask<<5);
		CANIDM4=0;

		CANCDMOB &= ~0x10;	// clear IDE =0, standard 11 bits

		//usart1_transmit_string("\rRx Standard Set\n");
	}
	else					// extended
	{
		CANIDT1= (unsigned char)(id>>21);
		CANIDT2= (unsigned char)(id>>13);
		CANIDT3= (unsigned char)(id>>5);
		CANIDT4= (unsigned char)(id<<3);

		CANIDM1= (unsigned char)(idmask>>21);
		CANIDM2= (unsigned char)(idmask>>13);
		CANIDM3= (unsigned char)(idmask>>5);
		CANIDM4= (unsigned char)(idmask<<3);

		CANCDMOB |= 0x10;	// set IDE =1, extended 29 bits

		//usart1_transmit_string("\rRx Extended Set\n");
	}
	CANCDMOB |= (dlc & 0x0f);		// set data length

	CANIDM4 |= (unsigned char)(rtrIdemask & 0x07);
	//	CANIDM4 &= ~0x04;		// RTRMSK= 1/0 enable comparison (Data receive)
	//	CANIDM4 &= ~0x02;		// recommended
	//	CANIDM4 &= ~0x01;		// IDEMSK= 1/0 enable comparison (IDE receive)

	CANCDMOB |= 0x80;		// receive enable
}

ISR(CANIT_vect)   //CAN interrupt service routine.
{
	char rece_result = _RECE_FAIL;
	unsigned char i;
	unsigned long can_id=0;
	rece_result = _RECE_OK;	
	
	if (( CANHPMOB & 0xF0 ) == 0xF0 ) 
	{
		 // No MOB has an interrupt flagged, simply clear the interrupt
		 CANGIT |= 0x7F ;
		 return ;
	}

	CANPAGE = CANHPMOB ;
	
//	CANPAGE = 0x20;				// 수신 MOb 선택
	
	if((CANCDMOB & 0x10) == 0x00)
	{
		rxmsg.ide = STD;
		can_id = ((unsigned long)CANIDT1)<<8;
		can_id |= ((unsigned long) CANIDT2);
		can_id >>=5;
		//		usart1_transmit_string("\rRx standart\n");
	}
	else
	{
		rxmsg.ide= EXT;
		can_id=((unsigned long)CANIDT1)<<24;
		can_id |= ((unsigned long)CANIDT2)<<16;
		can_id |= ((unsigned long)CANIDT3)<<8;
		can_id |= ((unsigned long)CANIDT4);
		can_id>>=3;
		//		usart1_transmit_string("\rRx Extended\n");
	}
	
	rxmsg.id=can_id;
	rxmsg.dlc = CANCDMOB & 0x0f;
	for(i=0;i<(CANCDMOB&0x0f);i++)
	{
		rxmsg.data[i]=CANMSG;
	}

	if (can_id==0x100)  //@ frame data에 따른 led toggle
	{
		PORTC^=0x01;
	}
	else if (can_id==0x200)
	{
		PORTC^=0x02;
	}
	else if (can_id==0x300)
	{
		PORTC^=0x04;
	}
	else if (can_id==0x400)
	{
		PORTC^=0x08;
	}
	else if (can_id==0x500)
	{
		PORTC^=0x10;
	}
	else if (can_id==0x600)
	{
		PORTC^=0x20;
	}
	else if (can_id==0x700)
	{
		PORTC^=0x40;
	}
	else if (can_id==0x800)
	{
		PORTC^=0x80;
	}
	else if (can_id==0x150)
	{
		PORTA^=0x01;
	}
	else if (can_id==0x250)
	{
		PORTA^=0x02;
	}
	
	CANSTMOB = 0;
	CANCDMOB |= (1<<CONMOB1);
	CANSTMOB &= ~(1<<RXOK);	
}

void can_int_rx_set(char obj, unsigned long id, char rplvIde, unsigned char dlc, unsigned long idmask, unsigned char rtrIdemask)
{
	CANPAGE = obj<<4;		// set MOb number

	CANSTMOB = 0x00;		// clear status

/*	if(rplvIde & 0x02)
		CANCDMOB |= 0x20;			// RPLV set, 자동 응답 모드 설정
	else
		CANCDMOB &= ~0x20;			// RPLV clear
*/		
		

	if(rplvIde == STD)			// standard
	{
		CANIDT1= (unsigned char)(id>>3);
		CANIDT2= (unsigned char)(id<<5);

		CANIDM1= (unsigned char)(idmask>>3);
		CANIDM2= (unsigned char)(idmask<<5);
		CANIDM4=0;

		CANCDMOB &= ~0x10;	// clear IDE =0, standard 11 bits

		//usart1_transmit_string("\rRx Standard Set\n");
	}
	else					// extended
	{
		CANIDT1= (unsigned char)(id>>21);
		CANIDT2= (unsigned char)(id>>13);
		CANIDT3= (unsigned char)(id>>5);
		CANIDT4= (unsigned char)(id<<3);

		CANIDM1= (unsigned char)(idmask>>21);
		CANIDM2= (unsigned char)(idmask>>13);
		CANIDM3= (unsigned char)(idmask>>5);
		CANIDM4= (unsigned char)(idmask<<3);

		CANCDMOB |= 0x10;	// set IDE =1, extended 29 bits

		//usart1_transmit_string("\rRx Extended Set\n");
	}
	CANCDMOB |= (dlc & 0x0f);		// set data length

	CANIDM4 |= (unsigned char)(rtrIdemask & 0x07);
	//	CANIDM4 &= ~0x04;		// RTRMSK= 1/0 enable comparison (Data receive)
	//	CANIDM4 &= ~0x02;		// recommended
	//	CANIDM4 &= ~0x01;		// IDEMSK= 1/0 enable comparison (IDE receive)


	//  인터럽트 인에이블(인터럽트 설정)
	CANGIE |= 0xa0; 		// Enable all interrupt and Enable Rx interrupt

	if(obj<8)
	CANIE2 = (1<<obj);		// 해당 MOb의 인터럽트를 인에이블 시킴
	else
	CANIE1 = (1<<(obj-8));	//

	CANCDMOB |= 0x80;			// 수신 인에이블
//	sei();
}

/*
ISR(TIMER0_OVF_vect)
{
	timer0++;
	
	if(timer0%10==0) //10ms
	{
		flag_10ms=1;
	}
	if(timer0%50==0) //50ms
	{
		flag_50ms=1;
	}
	if(timer0%100==0)  // 100ms
	{
		timer0=0;
		flag_100ms=1;
	}
}*/

void check_loop()
{
	fixed_cycle_time = cycle_time + (unsigned int)250;
				
	while(((int)(fixed_cycle_time - TCNT0)>=0))
	{

	}
	TCNT0=0;
	cycle_time = TCNT0;
	
	timer0++;
	if(timer0%10==0) //10ms
	{
		flag_10ms=1;
	}
	if(timer0%50==0) //50ms
	{
		flag_50ms=1;
	}
	if(timer0%100==0)  // 100ms
	{
		timer0=0;
		flag_100ms=1;
	}
}