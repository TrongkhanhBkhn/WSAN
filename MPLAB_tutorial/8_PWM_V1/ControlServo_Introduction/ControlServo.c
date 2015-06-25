/* ***************************************************************************
**  File Name    		: ControlServo.c
**  Version				: 1.0
**  Author				: NguyenTienDat
**	Class				: KSTN - DTVT - K54
**  Microcontroller		: PIC18F46K20
**  Compiler			: Microchip C18 v3.40 C Compiler
**  IDE					: Microchip MPLAB IDE v8.83
**  Programmer			: Microchip PICKit2 Programmer
**  Last Updated		: 15 Feb 2012
**	Detail description	: I have use 5 pin to output PWM signal. Those are CCP2B,
**						  P1A through P1D. Although I use CCP1CON, please don't think
**						  that I use PWM (enhanced mode). I use completely standard mode.
**						  If you don't believe, strongly recommend you reference datasheet.
**						  This is basic program to learn to control servo or other motor.
**						  So I think this is helpful for you. Hope you enjoy.
**	Copyright			: This program is written by Nguyen Tien Dat_KSTN_K54.
**						  All right reserve. No part of this program may be reproduced or
**						  transmitted in any form or by any mean without my permission. Only
**						  allowed by me, you may develope other. Please to be a polite person!
** ***************************************************************************/
//include library for program
#include <p18f46k20.h>
#include <delays.h>

//configure
#pragma config FOSC = INTIO67	//source ocsilator is internal. Default value is 1 MHz
#pragma config WDTEN = OFF, LVP = OFF	//disable watchdog timer

//define period and duty cycles for generating PWM signal
#define duty_cycle_1 0x18	//you can assign any value if you want.
#define duty_cycle_2 0x19
#define period 249			//period 16 ms @Fosc = 1 MHz

//following function will out PWM signal
void enable_PWM_pin(void)
{
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISDbits.TRISD5 = 0;
	TRISDbits.TRISD6 = 0;
	TRISDbits.TRISD7 = 0;
}

void main(void)
{
	//Disable the all PWM pin (CCP2B and P1A through P1D outputs) by setting the associated TRIS bit.
	TRISC = 0x06;
	TRISD = 0xE0;
	
	//For generating PWM at P1A through P1D, must set bits of PSTRCON register.
	//If we only want to out at one or two or three, we will set only associated bits.
	PSTRCON = 0x1F;
	
	//Set the PWM period by loading the PR2 register.
	PR2 = period;
	
	//Configure the CCP module for the PWM mode by loading the CCPxCON register with the appropriate values.
	CCP1CON = 0x0C;//standard mode
	CCP2CON = 0x0F;
	
	//Set the PWM duty cycle by loading the CCPRxL register and DCxBy bits of the CCPxCON register.
	CCPR2L = duty_cycle_2;
	CCP2CONbits.DC2B1 = 1;
	CCP2CONbits.DC2B0 = 1;
	CCPR1L = duty_cycle_1;
	CCP1CONbits.DC1B1 = 1;
	
	//Configure and start Timer2
	PIR1bits.TMR2IF = 0;
	T2CON = 0x02;
	T2CONbits.TMR2ON=1;
	
	//Enable PWM output after a new PWM cycle has started
	while(!PIR1bits.TMR2IF);
	enable_PWM_pin();
	
	//forever loop
	while(1);
}
