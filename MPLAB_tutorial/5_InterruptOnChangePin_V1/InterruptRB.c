/* ***************************************************************************
**  File Name    	: InterruptRB.c
**  Version			: 1.0
**  Description		: this is a program that is perform for interrupt at RB4
**					: and RB7. There are two button to count increasingly and
**					  count decreasingly.
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.83
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 29 April 2012
** ***************************************************************************/

#include<p18f46k20.h>

#pragma config  FOSC=HS,WDTEN=OFF,PBADEN=OFF,MCLRE=ON,LVP=OFF

//predefine global LookupTableay for display on led 7 segment
unsigned char LookupTable[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
char index = 0;

//function prototype
void config_RBINT(void);//funtion use to configure intterrupt module
void RB_isr(void);//interrupt service program for RB interrupt

void main()
{
	config_RBINT();
	TRISC = 0;
	LATC = LookupTable[0];
	while(1)
	{
		//other application
	}
}

void config_RBINT(void)
{
	TRISB = 0xFF;
	INTCON = 0x88;	//INTCONbits.RBIE = 1;
					//INTCONbits.RBIF = 0;
					//INTCONbits.GIE = 1;
	
	IOCB = 0x90;	//IOCBbits.IOCB7 = 1;
					//IOCBbits.IOCB4 = 1;
}

#pragma code interrupt_vector=0x08
void interrupt_vector()
{
	_asm
	goto RB_isr
	_endasm
}

#pragma interrupt RB_isr
void  RB_isr(void)
{
	if(!PORTBbits.RB4)
	{
		++index;
		if(index == 10)
		index = 0;
	}
	if(!PORTBbits.RB7)
	{
		--index;
		if(index == -1)
		index = 9;
	}
	INTCONbits.RBIF = 0;
	LATC = LookupTable[index];
}
