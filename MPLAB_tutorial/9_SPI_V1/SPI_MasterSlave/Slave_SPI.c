/* *****************************************************************************
**  File Name    	: Slave_SPI.c
**  Version		: 1.0
**  Hardware Description: This processor is slave. It uses RC3 -> SCK (clock line)
**                        RC4 -> SDI pin (make master input - slave output line)
**                        RC5 -> SDO pin (make master output - slave input line)
**                        SS (RA5) -> connect to RD0 of slaver.
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
//#define USE_SPI_H

#ifdef USE_SPI_H
    #include <spi.h>
#endif

//custom definition
#define Led7_tris	TRISD
#define Led7_lat	LATD
#define RA5_SS		ANSELbits.ANS4
#define Digital		0

//function prototype
#ifndef USE_SPI_H
	void InitSPISlave(void);
	unsigned char ReceiveData(void);
#endif

void main(void)
{
	//initialize led 7 seg
    Led7_tris = 0x00;
    Led7_lat = 0x40;
    
	#ifdef USE_SPI_H
	    OpenSPI(SLV_SSON, MODE_10, SMPMID);
	#else
	    InitSPISlave();
	#endif
	
    while(1)
    {
	#ifdef USE_SPI_H
		Led7_lat = getcSPI();
	#else
		Led7_lat = ReceiveData();
	#endif
    }
}
#ifndef USE_SPI_H
void InitSPISlave(void)
{
    TRISC = 0x18;//slave -> clock pin is input
    
    RA5_SS = Digital;
    TRISAbits.TRISA5 = 1;//slave -> SS is input
    
    /* To find out more about 2 command the following, please review datasheet */
    SSPSTAT = 0x40;
    SSPCON1 = 0x74;
}
unsigned char ReceiveData(void)
{
    unsigned char ReadOldData = SSPBUF;// Clear SSPBUF
    PIR1bits.SSPIF = 0;      // Clear interrupt flag
    SSPBUF = 0x00;           // refresh SSPBUF
    while ( !SSPSTATbits.BF );// wait until have data in SSPBUF register
    return ( SSPBUF );       // return with a byte have just read
}
#endif
