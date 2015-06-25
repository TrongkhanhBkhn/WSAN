/* ***************************************************************************
**  File Name    : adc.c
**  Version      : 1.0
**  Description  : MCU will take analog signal from sensor LM35 through RA0 pin,
**				   so this pin must beconfigure analog input. It also use reference
**				   voltage is external supply (3.3V). The result will be processed
**				   to display temperature in 2 7-seg led.
**  Author       : NguyenTienDat
**	Class		 : KSTN - DTVT - K54
**  Compiler     : Microchip C18 v3.40 C Compiler
**  IDE          : Microchip MPLAB IDE v8.83
**  Programmer   : Microchip PICKit2 Programmer
**  Last Updated : 30 April 2012
** ***************************************************************************/
#include <p18f46k20.h>

/*
** PIC18F46K20 Configuration Bit:
**
** FOSC = HS			- High Frequency Oscillator
** FCMEN = OFF			- Fail-Safe Clock Monitor disabled
** BOREN = OFF			- Brown-out Reset disabled in hardware and software
** WDTEN = OFF			- WDT is controlled by SWDTEN bit of the WDTCON register
** MCLRE = ON			- MCLR pin enabled, RE3 input pin disabled
** LVP = OFF			- Single-Supply ICSP disabled
*/
#pragma config FOSC = HS
#pragma config FCMEN = OFF, BOREN = OFF
#pragma config WDTEN = OFF, MCLRE = ON, LVP = OFF

//declare globle variable
#define temperature result
unsigned char display_enable;

//prototype function
void init_ADC(void);
void ADC_isr(void);


//main function
void main(void)
{
	unsigned int result;
	unsigned char a,b;
	init_ADC();
	TRISD = 0;
	
	ADCON0bits.GO = 1;//Start conversion by setting the GO/~DONE bit

	while(1)
	{
		if(display_enable)
		{
			//get result from registers ADRESH & ADRESL
			result = ADRESH;
			result = result << 8;
			result = result | ADRESL;
			temperature = (result*10)/31;
			a = temperature%10;
			b = temperature/10;
			LATD = (a<<4)|b;
			display_enable = 0;
			ADCON0bits.GO = 1;//Start conversion by setting the GO/~DONE bit
		}
	}
}

//Initialize ADC module
void init_ADC(void)
{
	/***************************Configure port***************************/
	TRISAbits.TRISA0 = 1;
	ANSEL = 0x01;		// Set PORT RA0 is analog input
	ANSELH = 0;			// Set PORT AN8 to AN12 as Digital I/O
	
	/************************Configure ADC module************************/
	//Select ADC conversion clock
	//Select result format
	//Select acquisition delay
	ADCON2 = 0x91;	//ADCS<2:0> = 001 => Conversion clock = Fosc/8
					//Right justified
					//ACQT<2:0> = 010 => Acquisition Time = 4 Tad
	
	//Configure voltage reference
	ADCON1 = 0x10;	//negative voltage reference supplied by Vss
					//positive voltage reference supplied by VREF+ = 3.3V
	
	//Select ADC input channel
	ADCON0 = 0x00;	//CHS<3:0> = 0000 => select channel AN0
	
	//Turn on ADC module
	ADCON0bits.ADON = 1;
	
	/******************Configure ADC interrupt (optional)*****************/
	//Clear ADC interrupt flag
	PIR1bits.ADIF = 0;
	
	//Enable ADC interrupt
	PIE1bits.ADIE = 1;
	
	//Enable peripheral interrupt
	INTCONbits.PEIE = 1;
	
	//Enable global interrupt
	INTCONbits.GIE = 1;
}

#pragma code interrupt_vector=0x08
void interrupt_vector()
{
	_asm
	goto ADC_isr
	_endasm
}

#pragma interrupt ADC_isr
void  ADC_isr(void)
{
	//enable to display on led 7-seg
	display_enable = 1;
	
	//clear flag to re-enable interrupt
	PIR1bits.ADIF = 0;
}
