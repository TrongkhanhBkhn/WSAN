/* ***************************************************************************
**  File Name    	: ExternalInterrupt.c
**  Version			: 1.0
**  Description		: this is a program that is perform for external interrupt
**					: at INT0 & INT1. There are two button to count increasingly
**					  and count decreasingly.
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
char LookupTable[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
char index = -1;

//function prototype
void config_ExINT(void);//funtion use to configure intterrupt module
void ExINT_isr(void);//interrupt service program

void main()
{
	config_ExINT();
	TRISC = 0;
	while(1)
	{
		//other applications
	}
}

void config_ExINT(void)
{
	
	/** INTCONbits.GIE=1;			//Enables all unmasked interrupts
	 INTCONbits.PEIE=1;		//Enables all unmasked peripheral interrupts
	INTCONbits.INT0IE=1;		//Enables the INT0 external interrupt
	 INTCON3bits.INT1IE=1;		//Enables the INT1 external interrupt 
	 INTCON3bits.INT1IF=0;		//The INT1 external interrupt did not occur. It mean I refresh this flag
	 INTCONbits.INT0IF=0;		//The INT0 external interrupt did not occur. It is same as above, I also refresh this flag
	 INTCON2bits.INTEDG0=1;	//External Interrupt 0 Edge Select bit_interrupt on falling edge
	 INTCON2bits.INTEDG1=1;	//External Interrupt 1 Edge Select bit_Interrupt on falling edge
	 */
        INTCON2bits.INTEDG0=0;	//External Interrupt 0 Edge Select bit_interrupt on falling edge
	INTCON2bits.INTEDG1=0;	//External Interrupt 1 Edge Select bit_Interrupt on falling edge
	INTCON = 0xD2;
	INTCON3 = 0x08;
}

#pragma code interrupt_vector=0x08
void interrupt_vector()
{
	_asm
	goto ExINT_isr
	_endasm
}

#pragma interrupt ExINT_isr
void  ExINT_isr(void)
{
	if(INTCONbits.INT0IF)
	{
		index++;
		INTCONbits.INT0IF = 0;
		if(index == 10)
		index = 0;
	}
	if(INTCON3bits.INT1IF)
	{
		index--;
		INTCON3bits.INT1IF = 0;
		if(index == -1)
		index = 9;
	}
	LATC = LookupTable[index];
}
