/* File name        : lcd.h
 * Version          : 1.0
 * Author           : Nguyen Tien Dat
 * Class            : KSTN - DTVT - K54 - DHBKHN
 * Description      : This library is written to drive text LCD 16x2.
 *                    LCD interface type 4-bits.
 */

#ifndef LCD_H
#define	LCD_H

/*-------------------------------include library-------------------------------*/
#include "generic.h"

/*----------------------------------definitions---------------------------------*/
//pins of MCU that drive LCD
#define LCD_RS          LATBbits.LATB0
#define LCD_RW          LATBbits.LATB1
#define LCD_E           LATBbits.LATB2

#define	LCD_D4          LATBbits.LATB3
#define	LCD_D5          LATBbits.LATB4
#define	LCD_D6          LATBbits.LATB5
#define	LCD_D7          LATBbits.LATB6

//popular command to drive LCD
#define	LCD_clear	0x01	//Xoa toan bo noi dung DDRAM cua LCD
#define	LCD_home	0x02	//Dua con tro ve dau dong
#define	LCD_normal	0x06	//Che do ghi du lieu binh thuong vao LCD
#define	LCD_off         0x08    //Tat man hinh
#define	LCD_on          0x0C    //Bat man hinh va khong hien thi con tro
#define	LCD_on_crsr     0x0E    //Bat man hinh va con tro nhung khong nhap nhay con tro
#define	LCD_on_blink    0x0F    //Bat man hinh va con tro, dong thoi nhap nhay con tro
#define	LCD_4b2l        0x28    //Giao tiep 4 bit, 2 dong, font 5x8
#define	LCD_homeL1      0x80    //Di chuyen con tro ve dau dong 1
#define	LCD_homeL2      0xC0    //Di chuyen con tro ve dau dong 2

#define lcd_line_one    0x00    //Dia chi dau tien cua dong 1
#define lcd_line_two    0x40    //Dia chi dau tien cua dong 2

//delays parameter
#define Delay40us()     Delay10TCYx(Fosc_MHz)      //@Fosc_MHz = 4MHz --> 1Tcy = 4/Fosc_MHz (us)
                                                    //--> 1us = Fosc_MHz/4 (Tcy) --> 40us = 10 * Fosc_MHz
#define Delay2ms()      Delay1KTCYx(2)      //@Fosc_MHz = 4

/*------------------------------function prototype------------------------------*/
//initialize LCD
void Init_LCD(void);

//create pulse at LCD_E pin
void LCD_OK(void);

//write a command to LCD
void LCD_CMD(unsigned char cmd);

//write a data to LCD
void LCD_DATA(unsigned char data);

//write a string to LCD
void lcd_putstr(unsigned char *string);

//write a number decima (up to 4-digit) to LCD
void LCD_IN_INTER(unsigned char d);

//write a float to LCD
void LCD_IN_FLOAT( float d);

//bring cursor to point has location (x;y). (x;y) may be between (1;1) and (16;2)
void lcd_gotoxy(int x, int y);

#endif	/* LCD_H */
