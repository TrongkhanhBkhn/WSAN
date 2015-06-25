#include "definition.h"

//*************************************configure bits*************************************
#pragma romdata CONFIG1H = 0x300001
const rom unsigned char config1H = 0b00000010;// HS , disabled Fail-Safe Clock Monitor

#pragma romdata CONFIG2L = 0x300002
const rom unsigned char config2L = 0b00001110;// Brown-out Reset Enabled in hardware @ 2.7V, PWRTEN enabled

#pragma romdata CONFIG2H = 0x300003
const rom unsigned char config2H = 0b00010010;// WDT 1:512 postscale, WDT is controlled by SWDTEN bit

#pragma romdata CONFIG3H = 0x300005
const rom unsigned char config3H = 0b10000100;// PORTB digital on RESET, MCLR pin enabled
                                     // The system lock is held off until the HFINTOSC is stable

#pragma romdata CONFIG4L = 0x300006
const rom unsigned char config4L = 0b10000001;// Stack full will cause reset, LVP off
                                     // Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins

#pragma romdata

//variables
BYTE status;//bien luu tru trang thai cua cac van
BYTE Command;//bien nhan lenh tu Zigbee
BYTE Control;//bien nhan lenh dieu khien bat/tat
BYTE EnableProcessFlag = 0;//Cho phep xu ly lenh nhan duoc tu Zigbee

WORD time1 = 0, time2 = 0, time3 = 0, time4 = 0, time5 = 0;//soft timer

void main(void)
{
    WDTCONbits.SWDTEN = 0;//tat wdt timer

    //initialize UART module and timer0 module of PIC
    InitTimer0();
    InitUART();
    
    // Enable all high priority interrupts
    INTCONbits.GIEH = 1;
    T0CONbits.TMR0ON = 1;

    //initialize port control pump and van
    //cac chan A0 -> A5 o che do digital
    ANSEL = 0x00;
    TRISA = 0b11000000;//RA0 -> RA5 is output
    PORTA = 0b00000000;//RA0 -> RA5 is low level
    
    while(1)
    {
        WDTCONbits.SWDTEN = 0;//tat wdt timer
        if(EnableProcessFlag)
        {
            //neu nhan duoc lenh tu module UART thi xu ly lenh nay
            ProcessCommand();
            //gui lenh xac nhan len node Zigbee thong qua module UART
            ConfirmToZigbeeNode(Command);
            EnableProcessFlag = CLEAR;
        }
    }
}



//this interrupt is not priority interrupt. It is in Mid-Range Compatibility mode, which
//have address of interrupt vector at 0x08
#pragma code InterruptVector = 0x08
void InterruptVector()
{
    _asm
	goto InterruptService
    _endasm
}

#pragma interrupt InterruptService
void InterruptService(void)
{
    if(PIR1bits.RCIF)
    {
        Command = RCREG;//get command from buffer UART receiver
        Control = Command & 0x80;//On or Off?
        EnableProcessFlag = SET;//Enable to process the command has just get
        PIR1bits.RCIF = CLEAR;//Clear to reuse
    }
    
    if(INTCONbits.TMR0IF)
    {
        if((time1 == TimeOut)&&(time2 == TimeOut)&&(time3 == TimeOut)&&(time4 == TimeOut)&&(time5 == TimeOut))
        {
            Control = OFF;
            Command = OffAllVanAuto;
            EnableProcessFlag = SET;
        }
        else
        {
            //check among timex
            if(time1)
            {
                if(time1 == TimeOut)
                {
                    Control = OFF;
                    Command = OffVan1Auto;
                    EnableProcessFlag = SET;
                }
                else
                ++time1;
            }

            if(time2)
            {
                if(time2 == TimeOut)
                {
                    Control = OFF;
                    Command = OffVan2Auto;
                    EnableProcessFlag = SET;
                }
                else
                ++time2;
            }

            if(time3)
            {
                if(time3 == TimeOut)
                {
                    Control = OFF;
                    Command = OffVan3Auto;
                    EnableProcessFlag = SET;
                }
                else
                ++time3;
            }

            if(time4)
            {
                if(time4 == TimeOut)
                {
                    Control = OFF;
                    Command = OffVan4Auto;
                    EnableProcessFlag = SET;
                }
                else
                ++time4;
            }

            if(time5)
            {
                if(time5 == TimeOut)
                {
                    Control = OFF;
                    Command = OffVan5Auto;
                    EnableProcessFlag = SET;
                }
                else
                ++time5;
            }

            TMR0L = 0xDC;//3036 -> 1s
            TMR0H = 0x0B;
            INTCONbits.TMR0IF = CLEAR;
        }
    }
}


