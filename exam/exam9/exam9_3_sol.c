
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>



//CAN 관련 매크로  ======================================
#define STD		0x00
#define EXT		0x01

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
	unsigned long id;
	unsigned char rtr;
	unsigned char ide;
	unsigned char dlc;
	unsigned char data[8];
};

struct MOb txmsg;
struct MOb rxmsg;

void delay_us(unsigned char time_us)    // Time delay for 1us (1MHz)
{
	register unsigned char i;

	for(i=0; i<time_us; i++)            // 4 cycle +
	{
		asm volatile(" PUSH R0 ");      // 2 cycle +
		asm volatile(" POP R0 ");       // 2 cycle +
		asm volatile(" PUSH R0 ");      // 2 cycle +
		asm volatile(" POP R0 ");       // 2 cycle +
		asm volatile(" PUSH R0 ");      // 2 cycle +
		asm volatile(" POP R0 ");       // 2 cycle = 16cycle = 1us
	}                                   // for 16MHz
}

void delay_ms(unsigned int time_ms) // Time delay for 1ms
{
	register unsigned int i;

	for(i=0; i<time_ms; i++)
	{
		delay_us(250);                  // 250us +
		delay_us(250);                  // 250us +
		delay_us(250);                  // 250us +
		delay_us(250);                  // 250us = 1ms
	}
}

void initPort()
{
	DDRC = 0xff;
	PORTC = 0xff;
	DDRG = 0xff;
	PORTG = 0x00;
	DDRE = 0xff;
	PORTE = 0xff;
}

void testPoint(int i)
{
	if(i == 0)
		PORTE=0xff;
	else
		PORTE=0x00;
}

void breakPoint()
{
	while(1)
	{
		PORTE=0x00;
	}
}

void can_init (char baudRate)	// CAN초기화
{
	unsigned char i, j;
	
	CANGCON |= (1<<SWRES);	// CAN 제어기 리셋
	// CAN General Control Register
	
	//보레이트 설정 ==============================================
	switch(baudRate){
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
	CANSTMOB = 0x00;
	CANCDMOB = 0x00;

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

unsigned char flg_can;

ISR(CANIT_vect)
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
	flg_can=1;
		
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

void main()
{
	char count = 0;

	//struct MOb txmsg;
	//struct MOb rxmsg;


	initPort();
	can_init(b500k);//b500k
	
	//can_rx_set(2, 200, STD, 8, 0x3FF, 0);
	can_int_rx_set(2, 200, STD, 8, 0x3FF, 0x00);
	
	flg_can=0;
	rxmsg.rtr=0;
	
	sei();
	
		
	while(1)
	{
		count++;
		
		//can_rx(2,&rxmsg);
		
		txmsg.id = 100;
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
		txmsg.data[7] = rxmsg.data[3];
			
	
		delay_ms(10);
		can_tx(0,&txmsg,0);
		
	}
}
	



