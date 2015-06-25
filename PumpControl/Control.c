#include "definition.h"

// Valve1
void mode_on1(void)
{
    LATAbits.LATA1 = 1;//turn on van 1
    UART_tran_char(OnVan1);//confirm to Zigbee node

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;//turn on pump
}
void mode_off1(void)
{
    //check status of other vans. If other vans are off, turn off pump
    if(!(LATAbits.LATA2|LATAbits.LATA3|LATAbits.LATA4|LATAbits.LATA5))
    {
        LATAbits.LATA0 = 0;//turn off pump
        Delay10KTCYx(250); //Delay 2,5s
    }
    LATAbits.LATA1 =0;//turn off van 1
    UART_tran_char(OffVan1);//confirm to Zigbee node
}

// Valve2
void mode_on2(void)
{
    LATAbits.LATA2 = 1;
    UART_tran_char(OnVan2);

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;

}
void mode_off2(void)
{
    if(!(LATAbits.LATA1|LATAbits.LATA3|LATAbits.LATA4|LATAbits.LATA5))
    {
        LATAbits.LATA0 = 0;
        Delay10KTCYx(250); //Delay 2,5s
    }
    LATAbits.LATA2 =0;
    UART_tran_char(OffVan2);
}

// Valve3
void mode_on3(void)
{
    LATAbits.LATA3 = 1;
    UART_tran_char(OnVan3);

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;
}
void mode_off3(void)
{
    if(!(LATAbits.LATA1|LATAbits.LATA2|LATAbits.LATA4|LATAbits.LATA5))
    {
        LATAbits.LATA0 = 0;
        Delay10KTCYx(250); //Delay 2,5s
    }
    LATAbits.LATA3 =0;
    UART_tran_char(OffVan3);
}

// Valve4
void mode_on4(void)
{
    LATAbits.LATA4 = 1;
    UART_tran_char(OnVan4);

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;
}
void mode_off4(void)
{
    if(!(LATAbits.LATA1|LATAbits.LATA2|LATAbits.LATA3|LATAbits.LATA5))
    {
        LATAbits.LATA0 = 0;
        Delay10KTCYx(250); //Delay 2,5s
    }
    LATAbits.LATA4 =0;
    UART_tran_char(OffVan4);
}

// Valve5
void mode_on5(void)
{
    LATAbits.LATA5 = 1;
    UART_tran_char(OnVan5);

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;
}
void mode_off5(void)
{
    if(!(LATAbits.LATA1|LATAbits.LATA2|LATAbits.LATA3|LATAbits.LATA4))
    {
        LATAbits.LATA0 = 0;
        Delay10KTCYx(250); //Delay 2,5s
    }
    LATAbits.LATA5 =0;
    UART_tran_char(OffVan5);
}

// Valves
void mode_on(void)
{
    LATAbits.LATA1 = 1;
    LATAbits.LATA2 = 1;
    LATAbits.LATA3 = 1;
    LATAbits.LATA4 = 1;
    LATAbits.LATA5 = 1;

    UART_tran_char(OnAllVan);

    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA0 = 1;
}
void mode_off(void)
{
    LATAbits.LATA0 = 0;
    Delay10KTCYx(250); //Delay 2,5s
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    LATAbits.LATA3 = 0;
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
    UART_tran_char(OffAllVan);
}

// Command error
void mode_error(void)
{
    UART_tran_char(0xAA);
}

//this function use to initialize pins connect with vans
void port_init(void)
{
    // Set ouput
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    // clear
    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    LATAbits.LATA3 = 0;
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
}

//this function use to transmit a character to another
void UART_tran_char(byte ch)
{
    while(!TXSTAbits.TRMT);
    TXREG = ch;
}
