/* ***************************************************************************
**  File Name    	: Autocounter.c
**  Version			: 1.0
**  Description		: count number on led 7-seg using timer mode of TMR0 to count
**					  from 0 to 9 and again start at 0 automatically.
**					  Interrupt in Mid-Range Compatibility mode, not priority.
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.83
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 29 April 2012
** ***************************************************************************/

#include<p18f46k20.h>

#pragma config FOSC=HS,WDTEN=OFF,LVP=OFF,PBADEN=OFF,MCLRE=ON

//prototype function
void TMR0_isr(void);				//declare program service of interrupt
void InitTimerMode(void);			//declare configuration of TMR0 at counter mode

//init array to display in 7-seg led
unsigned char LookupTable[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned char index = 0;

void main()
{
	InitTimerMode();
	
	TRISD = 0x00;				//7-seg led connect with PORT D
	LATD = LookupTable[0];		//7-seg led display 0
	while(1)
	{
		//other code
	}
}

//-------------------------------------------------------------------------------------
//this interrupt is not priority interrupt. It is in Mid-Range Compatibility mode, which
//have address of interrupt vector at 0x08
#pragma code InterruptVector = 0x08
void InterruptVector()
{
	_asm
	goto TMR0_isr
	_endasm
}

#pragma interrupt TMR0_isr
void TMR0_isr(void)
{
	++index;
	LATD = LookupTable[index];
	if(index == 9) index = -1;
	
	INTCONbits.TMR0IF = 0;
	TMR0L = 255;		//press button only a time, 7-seg led will display next digit
} 

void InitTimerMode(void)
{
    INTCON = 0xE0;				//enable global, peripheral and TMR0 interrupt
	
	//@ Fosc = 20 MHz
    T0CON = 0x85;				//To find out more meaning of this statement, please study in datasheet
    TMR0L = 0;					//clear timer
}
