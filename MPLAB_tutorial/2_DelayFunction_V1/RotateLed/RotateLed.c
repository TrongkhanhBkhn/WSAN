/* ***************************************************************************
**  File Name    	: RotateLed.c
**  Version			: 1.0
**  Description		: This program basically works by displaying LEDs
**					  attached to PORT D. Microcontroller will control turn on
**					  led from D0, delay 400ms and then it is turned off and D1
**					  is turned on. The process will be go on to D7. After that,
**					  process again start at D0.
**					  I use preprocessor to select one of two method to make rotate led
**					  these are: use lookup table and shift bit
**					  
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.83
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 29 April 2012
** ***************************************************************************/
#include "p18f46k20.h"
#include "delays.h"

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

//if you want to use one of two method, uncomment one of things.
//#define USE_LOOKUP_TABLE
#define USE_SHIFT_BIT

void main(void)
{
	#ifdef USE_LOOKUP_TABLE
		unsigned char table[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		char index = 0;
		while(1)
		{
			TRISD=0x00;
			LATD = table[index];
			index = index + 1;
			Delay1KTCYx(100);
			if (index == 8)
			index = 0;
		}
	
	#else
		TRISD = 0x00;
		LATD = 0x01;
		while(1)
		{
			Delay1KTCYx(100);
			LATD = LATD << 1;
			if(LATD == 0x00) LATD = 0x01;
		}
	#endif
}
