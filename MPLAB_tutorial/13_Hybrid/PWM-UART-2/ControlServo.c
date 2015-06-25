/* ****************************************************************************************************
**  File Name    	: control_2_servo.c
**  Version			: 1.0
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**	Company			: Lab411
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.83
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 24 Feb 2012
**	Describe     	: In this my program, I completely use my material. You can enter any
**					  value from -90 degree to +90 degree through vitural terminal.
**					  After that, MCU will convert string to number form. This result will
**					  use set duty cycle of PWM mode. PWM signal will out at CCP2 and P1A
**					  to rotate servo motors. I have built a algorithm for coding this my program.
**					  If you want to explore my program,I advise you should overview the algorithm.
**					  Have you enjoy it!
**	Copyright		: This program is written by Nguyen Tien Dat_KSTN_K54.
**					  All right reserve. No part of this program may be reproduced or transmitted in
**					  any form or by any mean without my permission. Only allowed by me, you may develope
**					  other verison.
**					  Please to be a polite person!
** ****************************************************************************************************/

//include library
#include <p18f46k20.h>

//config bits
#pragma config FOSC = INTIO67
#pragma config LVP = OFF, WDTEN = OFF

//define period value for PWM signal and original angle
#define PERIOD 249	//16ms @ FOSC = 1MHz
#define ZERO 94		//1.5 ms @ FOSC = 1MHz, corresponding with angle of servo 0 degree aproximately


/************************************declare some necessary variable***********************************/
//declare union PWMDC to store duty cycle value
union PWMDC
{
    unsigned int lpwm;
    char bpwm[2];
};

//declare variable i to store index of element in array
char i=0, flag=0;

//declare array data[] to store data receive from RCREG
char data[];

//declare arrays for display
char note[]="SYSTEM IS READY! Please get values between -90 and +90 degree for controlling servos!";
char confirm[]="Servos have rotated", renew[]="Start new process", getback[]="Error! Get back value";
char introduction[] = "This program is written by Nguyen Tien Dat - KSTN - DTVT - K54. Have you enjoy it!";
/********************************************************************************************************/


/***************************************************UART*************************************************/
//initialize for transmit UART
void init_uart_trans(void)
{
	//BAUDRATE = 2400 @ FOSC = 1MHz
	SPBRG = 25;
	TXSTAbits.BRGH =1;
	BAUDCONbits.BRG16 =0;

	//TX output
	TRISCbits.TRISC6=1;

	//
	RCSTAbits.SPEN=1;
	TXSTAbits.SYNC =0;

	//TRANG THAI IDLE O MUC CAO
	BAUDCONbits.CKTXP=0;

	//TRANSMISSION
	TXSTAbits.TXEN =1;

	//RESET
	TXREG = 0x00;
}

//initialize for receive UART
void init_uart_recep(void)
{
	//TOC DO BAUD = 2400 @ FOSC = 1MHz
	SPBRG = 25;
	TXSTAbits.BRGH =1;
	BAUDCONbits.BRG16 =0;

	//RX output
	TRISCbits.TRISC7=1;

	//CHO PHEP TRUYEN THONG NOI TIEP KHONG DONG BO
	RCSTAbits.SPEN=1;
	TXSTAbits.SYNC=0;

	/*****Interrupt for receive uart*****/
	// Enable interrupt priority
	RCONbits.IPEN = 1;
	// Enable receive UART interrupt
	PIE1bits.RCIE=1;
	// Make receive interrupt high priority
	IPR1bits.RCIP = 1;
	// Enable all high priority interrupts
	INTCONbits.GIEH = 1;
	
	//RECEIVE enable
	RCSTAbits.CREN =1;
}

//put a string through TXREG to stransmit
void putsUART(char *string)
{
	do
	{
		while(!PIR1bits.TXIF);//waiting until TXREG is empty
		TXREG = *string;
	}
	while(*string++);//loop until meet null
}

//isr for interrupt receive UART
void interrupt_uart(void);
/*****************************************************************************************************/


/**********************************************PWM module*********************************************/
/******************initialize PWM mode******************/
//Disable CCP2 and P1A.
//configure PSTRCON to P1A is PWM output, set period for
//PWM signal by assigned period value for PR2 register.
//set up PWM mode by configure CCPxCON.
void init_PWM(unsigned char period)
{
	TRISC = 0xFF;
	PSTRCON = 0x11;
	PR2 = period;
	CCP1CON = 0x0C;
	CCP2CON = 0x0F;
}

//set duty cycle for PWM signal from P1A
void set_duty_1(unsigned int duty_cycle)
{
	union PWMDC DCycle1;
	// Save the dutycycle value in the union
	DCycle1.lpwm = duty_cycle << 6;
	// Write the high byte into CCPR1L
	CCPR1L = DCycle1.bpwm[1];
	// Write the low byte into CCP1CON5:4
	CCP1CON = (CCP1CON & 0xCF) | ((DCycle1.bpwm[0] >> 2) & 0x30);
}

//set duty cycle for PWM signal from CCP2B
void set_duty_2(unsigned int duty_cycle)
{
	union PWMDC DCycle2;
	// Save the dutycycle value in the union
	DCycle2.lpwm = duty_cycle << 6;
	// Write the high byte into CCPR2L
	CCPR2L = DCycle2.bpwm[1];
	// Write the low byte into CCP2CON5:4
	CCP2CON = (CCP2CON & 0xCF) | ((DCycle2.bpwm[0] >> 2) & 0x30);
}

//configure for Timer2
void config_Timer2(void)
{
	PIR1bits.TMR2IF = 0;
	T2CON = 0x02;
	T2CONbits.TMR2ON = 1;
}
/******************************************************************************************************/


/********************************************other functions*******************************************/
/*****************************convert from string form to number form******************************/
char get_decimal_value(char *pointer)
{
	char a,b;
	//if string include 2 character (sign and 1 digit), convert to number include signed number with 1 digit
	if(i==2)
	{
		a=*(pointer+1)-'0';
		if(*pointer=='-') return(0-a);
		else return(a);
	}
	//if string include 3 character (sign and 2 digit), convert to number include signed number with 2 digit
	else
	{
		a=*(pointer+1)-'0';
		b=*(pointer+2)-'0';
		if(*pointer=='-') return (0-a*10-b);
		else return(a*10+b);
	}
}

/***************************************declare some function***************************************/
char get_Value_angle(char parameter)
{
	char arr_temp[4]={13, parameter,'>','>'};
	char angle=0;
	while(1)
	{
		putsUART(arr_temp);
		while(!flag);//waiting for until flag = 1;
		if(i<2|i>3)	//because of desired values from -90 to +90, so it include 2 or 3 digit
		{
			//display request get back value
			putsUART(getback);
			//need to refresh i
			i=0;
			//refresh flag for next information
			flag = 0;
			continue;//renew loop
		}
		else
		{
			//only string start with sign (- or +) may be allowed
			if((data[0]=='-')|(data[0]=='+'))
			{
				//all trings include characters are not digit is not allowed
				if((i==2)&(data[1]<'0'|data[1]>'9'))
				{
					putsUART(getback);
					i=0;
					flag=0;
					continue;
				}
				else if((i==3)&(data[1]<'0'|data[1]>'9'|data[2]<'0')|data[2]>'9')
				{
					putsUART(getback);
					i=0;
					flag=0;
					continue;
				}
				
				//if string is allowed (include 2 or 3 character, start with - or +, other characters are all digit)
				else
				{
					angle = get_decimal_value(data);
					//if value is not between -90 and +90 degree, request new value!
					if(angle>90|angle<-90)
					{
						angle=0;
						putsUART(getback);
						i=0;
						flag = 0;
						continue;
					}
					//if OK, we need to refresh i and flag to follow get others (for x or y)
					else
					{
						i=0;
						flag = 0;
						break;
					}
				}
			}
			//other strings don't start with - or + is not allowed
			else
			{
				putsUART(getback);
				i=0;
				flag=0;
				continue;
			}
		}
	}
	return angle;
}

unsigned int get_Value_duty(char angle)
{
	unsigned int a;
	float duty;
	
	//if we default FOSC = 1MHz and TMR2 prescale is 16-bit, duty will caculate is as:
	a=(270+angle)*25;
	duty=a/72;
	
	return (unsigned int)duty;
}
/******************************************************************************************************/


void main(void)
{
	char angle_1, angle_2;
	//declare 2 variables to store for values of x and y
	unsigned int duty_cycle_1,duty_cycle_2;
	//start UART
	init_uart_trans();
	init_uart_recep();
	
	//start PWM mode
	init_PWM(PERIOD);
	set_duty_1(ZERO);
	set_duty_2(ZERO);
	config_Timer2();
	
	//wating until TMR2IF = 1. After that, enable RC1 and RC2
	while(!PIR1bits.TMR2IF);
	TRISC = 0xF9;
	
	//put to UART my introduction
	putsUART(introduction);
	//new line
	TXREG=13;
	//put to UART status of system
	putsUART(note);
	
	while(1)
	{
		angle_1 = get_Value_angle('x');	//get duty cycle for servo attach to RC2 (PA1)
		angle_2 = get_Value_angle('y');	//get duty cycle for servo attach to RC1 (CCP2B)
		
		duty_cycle_1 = get_Value_duty(angle_1);
		duty_cycle_2 = get_Value_duty(angle_2);
		
		set_duty_1(duty_cycle_1);		//control servo attach to RC2
		set_duty_2(duty_cycle_2);		//control servo attach to RC1
		
		putsUART(confirm);
		
		//enter 2 lines
		TXREG=13;
		while(!PIR1bits.TXIF);
		TXREG=13;
		
		putsUART(renew);
	}
}
/******************************************************************************************************/

//vector interrupt
#pragma code interrupt_vector = 0x08
void interrupt_vector(void)
{
	_asm
		goto interrupt_uart
	_endasm
}

//isr for interrupt
#pragma interrupt interrupt_uart
void interrupt_uart(void)
{
	data[i] = RCREG;
	while(!PIR1bits.TXIF);
	TXREG = data[i];
	if((i>0)&(data[i]==13)) flag = 1;
	else if((i==0)&(data[0]==13));
	else i++;
	PIR1bits.RCIF = 0;
}
