/* 
 * File:   definition.h
 * Author: TienDat
 *
 * Created on June 28, 2013, 2:15 PM
 */

#ifndef DEFINITION_H
#define	DEFINITION_H

#include <p18f26k20.h>
#include <usart.h>
#include <Delays.h>

typedef char byte;

//when program to target, must comment this define
//#define DEBUG
#ifdef DEBUG
    #define OnVan1  '1'
    #define OnVan2  '2'
    #define OnVan3  '3'
    #define OnVan4  '4'
    #define OnVan5  '5'
    #define OnAllVan    '6'
    #define OffVan1 'a'
    #define OffVan2 'b'
    #define OffVan3 'c'
    #define OffVan4 'd'
    #define OffVan5 'e'
    #define OffAllVan 'f'
#else
    #define OnVan1  0x81
    #define OnVan2  0x82
    #define OnVan3  0x83
    #define OnVan4  0x84
    #define OnVan5  0x85
    #define OnAllVan    0x8F
    #define OffVan1 0x01
    #define OffVan2 0x02
    #define OffVan3 0x03
    #define OffVan4 0x04
    #define OffVan5 0x05
    #define OffAllVan 0x0F
#endif

// Declare subfunctions
void port_init(void);//initialize port
void UART_tran_char(byte ch);//transmit a character

void mode_on1(void);// turn on valve 1
void mode_off1(void);// turn off valve 1
void mode_on2(void);// turn on valve 2
void mode_off2(void);// turn off valve 2
void mode_on3(void);// turn on valve 3
void mode_off3(void);// turn off valve 3
void mode_on4(void);// turn on valve 4
void mode_off4(void);// turn off valve 4
void mode_on5(void);// turn on valve 5
void mode_off5(void);// turn off valve 5
void mode_on(void);// turn on valves
void mode_off(void);// turn off valves
void mode_error(void);// command error

#endif	/* DEFINITION_H */

