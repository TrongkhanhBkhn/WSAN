/* *****************************************************************************
**  File Name    	: Master_SPI.c
**  Version		: 1.0
**  Hardware Description: This processor is master. It uses RC3 -> SCK (clock line)
**                        RC4 -> SDI pin (make master input - slave output line)
**                        RC5 -> SDO pin (make master output - slave input line)
**                        RD0 -> connect to SS (RA5) of slaver.
**
**  Software Description: Master send data about led 7 seg table through SPI bus,
**                        every 0,5s. When Slave receive this data, it display on
**                        led 7 seg.
**  Author		: NguyenTienDat
**  Class		: KSTN - DTVT - K54
**  Microcontroller	: PIC18F46K20
**  Compiler     	: Microchip C18 v3.42 C Compiler
**  IDE          	: Microchip MPLAB IDE v8.85
**  Programmer   	: Microchip PICKit2 Programmer
**  Last Updated 	: 8 July 2012
** *****************************************************************************/
#include <p18f46k20.h>
#include <delays.h>

//if you want to use available functions in spi.h file, uncomment the following
#define USE_SPI_H

#ifdef USE_SPI_H
    #include <spi.h>
#endif

//custom definition
#define SS_pin_tris     TRISDbits.TRISD0
#define SS_pin_lat      LATDbits.LATD0

//function prototype
#ifndef USE_SPI_H
    void InitSPIMaster(void);
    void TransmitData(unsigned char Data);
#endif

void main(void)
{
    /* Declare Variable */
    unsigned char LookupTable[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
    char index = 0;
    
    SS_pin_tris = 0;//slave select pin -> output
    SS_pin_lat = 1;//normally, SS line at high level

    /* Initialize for MSSP module in SPI mode */
    #ifdef USE_SPI_H
    	OpenSPI(SPI_FOSC_16, MODE_10, SMPMID);
    #else
    	InitSPIMaster();
    #endif

    while(1)
    {
        SS_pin_lat = 0;//When want to communicate with slave, keep SS line at low level
        #ifdef USE_SPI_H
            putcSPI(LookupTable[index++]);
        #else
            TransmitData(LookupTable[index++]);
        #endif
        SS_pin_lat = 1;

        Delay10KTCYx(200);  //@16MHz --> 0,5s

        if(index == 10)
            index = 0;
    }
}

#ifndef USE_SPI_H
    void InitSPIMaster(void)
    {
        TRISC = 0x10;//TRISCbits.TRISC3 = 0 -> define clock pin as input
                     //TRISCbits.TRISC4 = 1 -> define SDI pin as input
                     //TRISCbits.TRISC5 = 0 -> define SDO pin as output
                     
        /* To find out more about 2 command the following, please review datasheet */
        SSPSTAT = 0x40;
        SSPCON1 = 0x31;
    }
    void TransmitData(unsigned char Data)
    {
        unsigned char ReadOldData = SSPBUF;// Clears BF
        PIR1bits.SSPIF = 0;//Clear interrupt flag for next check
        SSPCON1bits.WCOL = 0;//Clear any previous write collision
        
        SSPBUF = Data;//write a byte data to SSPBUF register
        while( !PIR1bits.SSPIF );//wait until write data completly
    }
#endif
