/* ***************************************************************************
**  File Name    	: BlinkLed.c
**  Version			: 1.0
**  Description		: Blink led after fixed time by using timer0. This program
**					  use two technique, these are polling and interrupt.
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.83
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 29 April 2012
** ***************************************************************************/
#include "p18f46k20.h"
#include "MyDef.h"

//configuration bits
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF  				// CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30  					// CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768  								// CONFIG2H
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC  	// CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF  					// CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF  				// CONFIG5L
#pragma config CPB = OFF, CPD = OFF  									// CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF  			// CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF  						// CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF  		// CONFIG7L
#pragma config EBTRB = OFF  											// CONFIG7H

//if you want use 16 bit-mode, uncomment this following
//#define USE_MODE_16BITS

//if you want use polling technique
//comment '#define USE_INTERRUPT_PRIORITY'
#define USE_INTERRUPT_PRIORITY

#ifdef USE_INTERRUPT_PRIORITY
	void InterruptServiceLow(void);
#endif

//funtion init_timer0 is declared to initialize timer0 module
void init_timer0(char mode)
{
	T0CON = mode;
	
	TMR0L = 0;		   //refresh register that store values for timer0 module
	#ifdef USE_MODE_16BITS
		TMR0H = 0;
	#endif
	
	//establish interrupt timer0
	#ifdef USE_INTERRUPT_PRIORITY
		RCONbits.IPEN = 1;//Enable priority levels on interrupts
		INTCONbits.TMR0IE = 1;//Enable TMR0 interrupt
		INTCON2bits.TMR0IP = 0;//TMR0 interrupt is low level
		INTCONbits.PEIE = 1;
		INTCONbits.GIE = 1;
	#endif	
	//clear flag interrupt
	INTCONbits.TMR0IF = 0;
}

void main(void)
{
	char mode;
	TRISD=0x00;
	LATD = 0x55;
	mode = 	STOP_TMR0
			& MODE_8_BITS
			& TIMER_MODE
			& USE_PRESCALE
			& PRES_256;
	
	init_timer0(mode);	//is configure as 8 bits
					   	//1:256 prescale
					   	//to know more about T0CON register,
					   	//I strongly recommend you reference in datasheet
	//triger timer0
	T0CONbits.TMR0ON = 1;
	while(1)
	{
		#ifndef USE_INTERRUPT_PRIORITY
			if(INTCONbits.TMR0IF)
			{
				LATD = ~ LATD;
				INTCONbits.TMR0IF = 0;
			}
		#endif
		
		//if you use interrupt technique, you can do anything you want at here.
		/*
			//your work
		 */
	}
}

//-------------------------------------------------------------------------------------
#ifdef USE_INTERRUPT_PRIORITY
	// Low priority interrupt vector
	#pragma code InterruptVectorLow = 0x18
	void InterruptVectorLow (void)
	{
		_asm
			goto InterruptServiceLow 			//jump to interrupt routine
		_endasm
	}
	#pragma interruptlow InterruptServiceLow  	// "interruptlow" pragma for low priority
	void InterruptServiceLow(void)
	{
		LATD = ~ LATD;
		INTCONbits.TMR0IF = 0;
	}
#endif
