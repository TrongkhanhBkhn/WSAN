/* ***************************************************************************
**  File Name    	: TimerPump.c
**  Version			: 1.0
**  Description		: This program used to control pump in WSAN project. It use
**					  Interrupt in Mid-Range Compatibility mode, not priority.
**  Author			: NguyenTienDat
**	Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F26K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.89
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 13 March 2013
** ***************************************************************************/
//use for debug
//#define DEBUG

//include file
#include<p18f26k20.h>
#ifdef DEBUG
#include<usart.h>
#endif

//******************************************************************************
// Configuration Bits									@modified by dat_a3cbq91
//******************************************************************************
//#pragma romdata CONFIG1H = 0x300001
//const rom unsigned char config1H = 0b01001000;// INTOSC with RA6 & RA7 are function port, enabled Fail-Safe Clock Monitor

#pragma romdata CONFIG2L = 0x300002
const rom unsigned char config2L = 0b00011110;// Brown-out Reset Enabled in hardware @ 1.8V, PWRTEN enabled

//#pragma romdata CONFIG2H = 0x300003
const rom unsigned char config2H = 0b00010010;// WDT 1:512 postscale, WDT is controlled by SWDTEN bit

#pragma romdata CONFIG3H = 0x300005
const rom unsigned char config3H = 0b10000100;// PORTB digital on RESET, MCLR pin enabled
                                              // The system lock is held off until the HFINTOSC is stable

#pragma romdata CONFIG4L = 0x300006
const rom unsigned char config4L = 0b10000001;// Stack full will cause reset, LVP off
                                     // Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins

#pragma romdata

//some definitions
//define 1 minute: value = 2^16 - (60s * 1e6(clock) : 256(pre-scale) : 4 (clock/Tcy))
#define OneMinute 6943
#define DURATION_WORK 15
#define ON 1
#define OFF 0

//prototype function
void TMR0_isr(void);
void InitTimer0(void);
void InitPortA(void);

//variables
volatile unsigned char minute = 59, hour = 9;
volatile unsigned char status = OFF;
unsigned int InitValue = OneMinute;
unsigned char LSB, MSB;

#ifdef DEBUG
//macro kiem tra da san sang truyen hay chua, neu da san sang, ConsolIsPutReady() = 1
#define ConsoleIsPutReady()     (TXSTAbits.TRMT)

//ham day mot ky tu len man hinh theo dinh dang ASCII
void ConsolePut(unsigned char c)
{
    while( !ConsoleIsPutReady() );
    TXREG = c;
}

//ham nay in ra mot BYTE hex len tren man hinh theo dinh dang 0x--.
rom unsigned char CharacterArray[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
// In mot unsigned char len terminal bang cach truyen 1 unsigned char trong 2 lan, truyen MSB truoc, LSB sau. @dat_a3cbq91
void PrintChar(unsigned char toPrint)
{
    unsigned char PRINT_VAR;
    PRINT_VAR = toPrint;
    toPrint = (toPrint >> 4)&0x0F;
    while (BusyUSART());
    TXREG = CharacterArray[toPrint];
    toPrint = (PRINT_VAR)&0x0F;
    while (BusyUSART());
    TXREG = CharacterArray[toPrint];
    return;
}
void PrintTime(void)
{
    PrintChar(hour);
    ConsolePut(':');
    PrintChar(minute);
    ConsolePut('\r\n');
}
#endif

void main()
{
    //khoi tao cac portA tren VDK xuat tin hieu dieu khien cac van
    InitPortA();

    #ifdef DEBUG
    //khoi tao module UART
    OpenUSART(USART_TX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_HIGH, 25);//2400 bsud
    #endif
    
    WDTCONbits.SWDTEN = 0;
    //khoi tao cac gia tri cho LSB va MSB, phuc vu cho module timer
    LSB = InitValue & 0x00FF;
    MSB = InitValue >> 8;

    //khoi tao module timer0 de dinh thoi gian tuoi
    InitTimer0();

    //bat dau kich hoat module timer0
    TMR0L = LSB;
    TMR0H = MSB;
    T0CONbits.TMR0ON = 1;
    

    #ifdef DEBUG
    PrintTime();
    #endif

    while(1)
    {
        //other code
        if(minute == 60)
        {
            minute = 0;
            ++hour;
            if(hour == 24)
            {
                hour = 0;
            }
        }

        //neu den gio tuoi, bat may bom
        //neu den gio tat, tat may bom
        if((hour == 10)||(hour == 11)||(hour == 12))
        {
            //neu dang bat
            if(status)
            {
                //neu luc 9h30 hoac 16h30
                if(minute == DURATION_WORK)
                {
                    //tat 5 van
                    LATA = 0x00;
                    //ket thuc viec tuoi
                    status = OFF;
                }
            }
            else
            {
                //neu 9h bat dau bat
                if(minute == 0)
                {
                    //bat 5 van
                    LATA = 0b00111111;
                    //trang thai cua cac van tuoi
                    status = ON;
                }
            }
        }
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
    //virtual real-time clock
    ++minute;

    #ifdef DEBUG
    PrintTime();
    #endif
    
    //refresh module timer0 de cho cac lan ngat tiep theo
    TMR0L = LSB;
    TMR0H = MSB;
    INTCONbits.TMR0IF = 0;
} 

void InitPortA(void)
{
    //cac chan A0 -> A5 o che do digital
    ANSEL = 0x00;

    //cac chan A0 -> A5 o che do output
    TRISA = 0b00000000;

    //ban dau, khi moi bat len, cac chan A0 -> A5 xuat tin hieu o muc logic thap: tat bom
    LATA = 0x00;
}
void InitTimer0(void)
{
    //che do timer 16 bit
    //prescale 1:256
    //nguon clock = Fosc/
    T0CON = 0b00000111;
    //bat ngat toan cuc, bat ngat ngoai vi, bat ngat timer0
    INTCON = 0xE0;
    TMR0L = LSB;
    TMR0H = MSB;
}
