/* ***************************************************************************
**  File Name    	: BlinkLed_V1.c
**  Version			: 1.0
**  Description		: Blink leds at PORTD after fixed time by using delay function
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
//to know more, Click Help menu, select Topic\Language Tools\PIC18 config setting
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

//define duration for delay
#define delay 50

//main function
void main(void)
{
	TRISD = 0x00;
	while(1)
	{
		LATD = 0b10101010;
		Delay1KTCYx(delay);	// Delay 50 x 1000 = 50,000 cycles instruction; 200ms @ 1MHz
							// Note that, because of FOSC = INTIO67, default frequency is 1MHz
		
		LATD = ~LATD;//inverse LATD => 0b01010101, and then, so on.
		Delay1KTCYx(delay);
	}
}
