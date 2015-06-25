/* ***************************************************************************
**  File Name    	: RTC.c
**  Version		: 1.0
**  Description		: 
**  Author		: Chu Trong Khanh
**  Class		: DT7 - K55
**  Cover by		: Nguyen Tien Dat
**  Class		: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.40 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.91
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 09 August 2013
** ***************************************************************************/
//header files
#include "ds1307.h"
#include "lcd.h"

//configure special register
#pragma config FOSC = HS
#pragma config FCMEN = OFF, BOREN = OFF
#pragma config WDTEN = OFF, MCLRE = ON, LVP = OFF

//function
void Display_time(unsigned char *pdata);
void Display_day(unsigned char *pdata);

//variable
unsigned char i, temp;
unsigned char udata[4];

//main function
void main(void)
{
    unsigned char st1[]="STARTING";
    unsigned char st2[]=".";

    //pins for drive LCD
    TRISB = 0x80;

    //Initialize modules
    Init_LCD();
    i2c_config();

    set_ds1307_time(AM,0,0,6);
    set_ds1307_day(8,11,8,13);

    lcd_putstr(st1);

    for(i=0;i<7;i++)
    {
        lcd_putstr(st2);
        Delay1KTCYx(200);
    }
    LCD_CMD(LCD_clear);
    
    while(1)
    {
        lcd_gotoxy(1,1);//hang 1, cot 1.
        Display_time(get_ds1307_time());

        lcd_gotoxy(1,2);//hang 2, cot 1
        Display_day(get_ds1307_day());

        Delay10KTCYx(70);
    };
}

void Display_time(unsigned char *pdata)
{
    unsigned char a[] = " AM";
    unsigned char b[] = " PM";
    unsigned char ch1[] = ":";
    unsigned char c[] = "H";

    LCD_IN_INTER(pdata[2]);//gio
    lcd_putstr(ch1);

    LCD_IN_INTER(pdata[1]);//phut
    lcd_putstr(ch1);

    LCD_IN_INTER(pdata[0]);//giay

    switch(udata[3])
    {
        case AM : lcd_putstr(a);break;
        case PM : lcd_putstr(b);break;
        default : lcd_putstr(c); break;
    }
}
void Display_day(unsigned char *pdata)
{
    unsigned char ch1[]=":";
    unsigned char hai[]="MON";
    unsigned char ba[]="TUE";
    unsigned char bon[]="WED";
    unsigned char nam[]="THUS";
    unsigned char sau[]="FRI";
    unsigned char bay[]="SAT";
    unsigned char cn[]="SUN";
    unsigned char er[]="ERROR";

    switch(pdata[0])
    {
        case 2: lcd_putstr(hai);break;
        case 3: lcd_putstr(ba);break;
        case 4: lcd_putstr(bon);break;
        case 5: lcd_putstr(nam);break;
        case 6: lcd_putstr(sau);break;
        case 7: lcd_putstr(bay);break;
        case 8: lcd_putstr(cn);break;
        default :lcd_putstr(er); break;
    }
    lcd_putstr(ch1);

    LCD_IN_INTER(pdata[1]);//ngay
    lcd_putstr(ch1);

    LCD_IN_INTER(pdata[2]);//thang
    lcd_putstr(ch1);

    LCD_IN_INTER(20);//nam
    LCD_IN_INTER(pdata[3]);
}
