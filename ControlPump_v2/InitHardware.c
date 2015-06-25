#include "definition.h"

/***************************************************UART*************************************************/
//initialize UART
void InitUART(void)
{
    //BAUDRATE = 19200 @ FOSC = 8MHz
    SPBRG = 25;
    TXSTAbits.BRGH =1;
    BAUDCONbits.BRG16 =0;

    //config pin
    TRISCbits.TRISC6=1;
    TRISCbits.TRISC7=1;

    //enable asychronous mode
    RCSTAbits.SPEN=1;
    TXSTAbits.SYNC =0;

    //enable TX
    TXSTAbits.TXEN =1;

    // Enable receive UART interrupt
    PIE1bits.RCIE=1;
    // Make receive interrupt high priority
    IPR1bits.RCIP = 1;

    //enable RX
    RCSTAbits.CREN =1;
}

void InitTimer0(void)
{
    //che do timer 16 bit
    //prescale 1:32
    //ngat 1s @8MHz
    T0CON = 0b00000100;
    //bat ngat toan cuc, bat ngat ngoai vi, bat ngat timer0
    INTCON = 0xE0;
    TMR0L = 0xDC;//3036 -> 1s
    TMR0H = 0x0B;
}
