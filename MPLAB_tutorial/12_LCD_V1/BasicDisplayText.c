/* *****************************************************************************
**  File Name    	: BasicDisplayText.c
**  Version			: 1.0
**  Description		:
**  Author			: NguyenTienDat
**  Class			: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.42 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.85
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 11 July 2012
** *****************************************************************************/
//Header file
#include <p18f46k20.h>
#include <xlcd.h>
#include <delays.h>

//My definition
#define FIRST_LINE 0
#define SECOND_LINE 64

//Delay functions for control LCD
void DelayFor18TCY(void)
{
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
}
void DelayPORXLCD(void)
{
    Delay1KTCYx(60);// Delay of 15ms
                    // Cycles = (TimeDelay * Fosc) / 4
                    // Cycles = (15ms * 16MHz) / 4
                    // Cycles = 60,000
    return;
}
void DelayXLCD(void)
{
    Delay1KTCYx(20);// Delay of 5ms
                    // Cycles = (TimeDelay * Fosc) / 4
                    // Cycles = (5ms * 16MHz) / 4
                    // Cycles = 20,000
    return;
}

//Main function
void main( void )
{
    //declare array to display on LCD
    char data1[] = "NGUYEN TIEN DAT";
    char data2[] = "KSTN-DTVT-K54";

    // configure external LCD
    OpenXLCD(FOUR_BIT & LINES_5X7);
    
    Delay1KTCYx(2);//wait to ready
    
    WriteCmdXLCD(BLINK_OFF & CURSOR_OFF);//no blink cursor and no display cursor
    
    putsXLCD(data1);
    
    //set cursor at header of second line.
    while(BusyXLCD());
    SetDDRamAddr(SECOND_LINE);
    
    putsXLCD(data2);
    
    while(1);
}
