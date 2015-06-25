#include "definition.h"

void Init_UART(void)
{
    TXSTAbits.BRGH =1;
    BAUDCONbits.BRG16 =1;//16-bit Baud Rate Generator is used (SPBRGH:SPBRG)
    SPBRGH:SPBRG = 12;// tra bang 18-5 toc do baurate 19200

    TRISCbits.TRISC6=1;// RC6 la chan rx
    TRISCbits.TRISC7=1;// RC7 la chan tx

    TXSTAbits.SYNC =0;
    RCSTAbits.SPEN=1;  // cho phep noi tiep voi cac cong o che do uart

    //BAUDCONbits.CKTXP=0; // Idle state for transmit (TX) is high

    TXSTAbits.TXEN =1;   // cho phep truyen uart
    //TXREG = 0x00;        // lam rong thanh ghi truyen

    PIE1bits.RCIE=1;     // cho phep ngat khi nhan uart
    INTCONbits.GIE=1;    // cho phep ngat toan cuc khi IPEN =1 set boi thanh ghi RCON
    INTCONbits.PEIE=1;   // cho phep ngat ngoai vi
    RCSTAbits.CREN =1;   // cho phep nhan uart khong dong bo
}

//void Init_Tran_UART(void)
//{
//    BAUDCONbits.CKTXP=0; // Idle state for transmit (TX) is high
//    TXSTAbits.TXEN =1;   // cho phep truyen uart
//    TXREG = 0x00;        // lam rong thanh ghi truyen
//}
//
//void Init_Rec_UART(void)
//{
//    PIE1bits.RCIE=1;     // cho phep ngat khi nhan uart
//    INTCONbits.GIE=1;    // cho phep ngat toan cuc khi IPEN =1 set boi thanh ghi RCON
//    INTCONbits.PEIE=1;   // cho phep ngat ngoai vi
//    RCSTAbits.CREN =1;   // cho phep nhan uart khong dong bo
//}

void Tran_UART(unsigned char ch)
{
    while(!TXSTAbits.TRMT);
    TXREG = ch;
}

void Pwm_init(void)
{   RCONbits.IPEN = 1; // Enable priority levels on interrupts
    IPR1bits.RCIP = 1;
    INTCONbits.GIEH = 1;
    OpenTimer2(0x06);  // chon gia tri 16 prescale cho timer2
    OpenPWM2(0xff);	   // set gia tri thanh ghi PR2 la 0xff
}

