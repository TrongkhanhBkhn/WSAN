#include "lcd.h"

void Init_LCD(void)
{
    LCD_CMD(LCD_home);
    Delay2ms();//delay time greater 1,64ms @Fosc = 4MHz
    
    LCD_CMD(LCD_on);
    Delay40us();

    LCD_CMD(LCD_4b2l);
    Delay40us();

    LCD_CMD(LCD_normal);
    Delay40us();
}

void LCD_OK(void)
{
    unsigned char u;

    //Tao xung Enable
    LCD_E = 1;
    for (u = 0; u < 2; u++); //do rong xung toi thieu la 1 us @Fosc = 4 MHz
    LCD_E = 0;
}

void LCD_CMD(unsigned char cmd)
{
    //ghi lenh vao LCD
    LCD_RW = 0;
    LCD_RS = 0;

    //xuat 4 bit cao
    LCD_D4 = cmd >> 4;
    LCD_D5 = cmd >> 5;
    LCD_D6 = cmd >> 6;
    LCD_D7 = cmd >> 7;
    LCD_OK();

    //Xuat 4 bit thap
    LCD_D4 = cmd;
    LCD_D5 = cmd >> 1;
    LCD_D6 = cmd >> 2;
    LCD_D7 = cmd >> 3;
    LCD_OK();
}

void LCD_DATA (unsigned char data)
{
    //Ghi du lieu vao LCD
    LCD_RW = 0;
    LCD_RS = 1;

    //Xuat 4 bit cao
    LCD_D4 = data >> 4;
    LCD_D5 = data >> 5;
    LCD_D6 = data >> 6;
    LCD_D7 = data >> 7;
    LCD_OK();

    //Xuat 4 bit thap
    LCD_D4 = data;
    LCD_D5 = data >> 1;
    LCD_D6 = data >> 2;
    LCD_D7 = data >> 3;
    LCD_OK();
}

void lcd_putstr(unsigned char *string)
{
    while(*string != '\0')
    {
        LCD_DATA(*string);
        string++;
        Delay40us();
    };
}

void lcd_gotoxy(int x, int y)
{
    int address;

    //go to line y
    if(y != 1)
        address = lcd_line_two;
    else
        address=lcd_line_one;

    //go to column x
    address += x - 1;

    LCD_CMD(0x80|address);
    Delay40us();
}

void LCD_IN_INTER(unsigned char d)
{
    unsigned char i,j,k,l;
    l = d/1000;//nghin
    k = (d%1000)/100;//tram
    j = ((d%1000)%100)/10;//chuc
    i = ((d%1000)%100)%10;//don vi

    if(l >= 1)
    {
        LCD_DATA(0x30+l);
    }
    if(k >= 1)
    {
        LCD_DATA(0x30+k);
    }

    LCD_DATA(0x30+j);
    LCD_DATA(0x30+i);
}

void LCD_IN_FLOAT(float d)
{
    unsigned char ch[]=".";
    unsigned int e,i,j,k,l;
    
    e = (int)(d*10);
    l = e/1000;//nghin
    k=(e%1000)/100;//tram
    j=((e%1000)%100)/10;//chuc
    i=((e%1000)%100)%10;//donvi
    LCD_DATA(0x30+l);
    LCD_DATA(0x30+k);
    LCD_DATA(0x30+j);
    lcd_putstr(ch);
    LCD_DATA(0x30+i);
}