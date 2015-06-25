/* ***************************************************************************
**  File Name    	: RotateLed.c
**  Version			: 1.0
**  Description		: rotate led using timer0. There are two option to do when
**					  interrupt occur. To use one of two, comment or uncomment
**					  '#define USE_POLLING'. I also use lookup table to display
**					  led. You can use shift bit for the work.
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

/*if you want use 16 bit-mode, uncomment this following*/
#define USE_MODE_16BITS

/*if you want use polling technique, uncomment '#define USE_POLLING'
 *if you want use interrupt technique, comment '#define USE_POLLING'
 */
#define USE_POLLING

#ifndef USE_POLLING
	void InterruptServiceLow(void);
#endif

unsigned char table[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
char index = 1, k = 0;//k variable to select case

//funtion init_timer0 is declared to initialize timer0 module
void init_timer0(char mode)
{
	T0CON = mode;
	
	TMR0L = 0;		   //refresh register that store values for timer0 module
	#ifdef USE_MODE_16BITS
		TMR0H = 0;
	#endif
	
	//establish interrupt timer0
	#ifndef USE_POLLING
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
	LATD = 0x01;

	mode = 	STOP_TMR0
			& MODE_16_BITS
			& TIMER_MODE
			& USE_PRESCALE
			& PRES_2;
	init_timer0(mode);
	//triger timer0
	T0CONbits.TMR0ON = 1;
	
	while(1)
	{
		#ifdef USE_POLLING
			if(INTCONbits.TMR0IF)
			{
				switch (k)
				{
					//case 0 will show led turn on top - down
					case 0:
						LATD = table[index];
						index = index + 1;
						if(index == 8)
						{
							k=1;//set k = 1 to exchange case when led 8th is turned on
							index=6;//the next led is turned on is led 7th
						}
						break;
									
					//case 1 will show led turn on botom - up
					case 1:
						LATD = table[index];
						index = index - 1;
						if (index == -1)
						{
							k = 0;//set k = 0 to exchange case when led 1st is turned on
							index=1;//the next led is turned on is led 2nd
						}
						break;
				}
				INTCONbits.TMR0IF = 0;
			}
		#endif
	}
}

//-------------------------------------------------------------------------------------
#ifndef USE_POLLING
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
		switch (k)
		{
			//case 0 will show led turn on top - down
			case 0:
			LATD = table[index];
			index = index + 1;
			if(index == 8)
			{
				k=1;//set k = 1 to exchange case when led 8th is turned on
				index=6;//the next led is turned on is led 7th
			}
			break;
					
			//case 1 will show led turn on botom - up
			case 1:
			LATD = table[index];
			index = index - 1;
			if (index == -1)
			{
				k = 0;//set k = 0 to exchange case when led 1st is turned on
				index=1;//the next led is turned on is led 2nd
			}
			break;
		}
		INTCONbits.TMR0IF = 0;
	}
#endif
