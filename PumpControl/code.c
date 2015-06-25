/*******************************************************************************
 * Creater:	Du Van Tuan
 * Address:	DT10-K54
 * Tel:		01674530491
 * Date:	August 3, 2012
 * 
 * Developer:   Nguyen Tien Dat
 * Address:     KSTN - DTVT - K54
 * Date:        June 28, 2013
 * 
 * 
 * Note:	RA1(pin 03) -> Valve 1
 * 		RA2(pin 04) -> Valve 2
 * 		RA3(pin 05) -> Valve 3
 * 		RA4(pin 06) -> Valve 4
 * 		RA5(pin 07) -> Valve 5
 * 		RA0(pin 02) -> Moto
 *
 * 		Fcy = 8Mz
********************************************************************************/

// Library
#include "definition.h"

// Configuration Bits
//******************************************************************************
#pragma romdata CONFIG1H = 0x300001
const rom unsigned char config1H = 0b01000010;// HS , enabled Fail-Safe Clock Monitor

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

// declare some global variables
byte ProcessFlag = 0;//This flag use to enable process command
byte Command;

// Declare subfunctions
void interrupt_handler(void);//UART-receive interrupt service routine
void ProcessCommand(void);

// program main
void main(void)
{
    //initialize UART module
    OpenUSART(USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_ASYNCH_MODE &
            USART_EIGHT_BIT &
            USART_BRGH_HIGH,
            25);

    INTCONbits.PEIE = 1;//Peripheral interrupt enable
    INTCONbits.GIE = 1; // Golbal interrupt enable

    port_init();//init ports connect to vans
    
    while(1)
    {
        if(ProcessFlag)
        {
            ProcessCommand();//processing command receive from Zigbee device
            ProcessFlag = 0;//clear the flag
        }
    }
}

//this function use to process command that receive from UART RX module
void ProcessCommand(void)
{
    switch(Command)
    {
        case OnVan1:	mode_on1();break;
        case OffVan1:	mode_off1();break;
        case OnVan2:	mode_on2();break;
        case OffVan2:	mode_off2();break;
        case OnVan3:	mode_on3();break;
        case OffVan3:	mode_off3();break;
        case OnVan4:	mode_on4();break;
        case OffVan4:	mode_off4();break;
        case OnVan5:	mode_on5();break;
        case OffVan5:	mode_off5();break;

        case OnAllVan:	mode_on();break;
        case OffAllVan:	mode_off();break;
        default:	mode_error();break;
    }
}

// Define interrupt as receiver finish	
#pragma interrupt interrupt_handler
void interrupt_handler(void)
{
    Command = RCREG;//get character from buffer UART receive
    ProcessFlag = 1;//enable to process
    PIR1bits.RCIF = 0;//reset the flag
}

#pragma code interrupt_vector = 0x08
void interrupt_vector(void)
{
    _asm
        goto interrupt_handler
    _endasm
}
