/* 
 * File:   definition.h
 * Author: TienDat
 *
 * Created on July 8, 2013, 11:26 AM
 */

#ifndef DEFINITION_H
#define	DEFINITION_H

#include<p18f46k20.h>
#include <usart.h>
#include <pwm.h>
#include <timers.h>
#include <delays.h>

#define VITRI_1	68      // set vi tri cho servo qua tinh toan trang 162 datasheet
#define VITRI_2	80      // gia tri nay chinh la duty cycle
#define VITRI_3	96
// vd  can xung 1.5ms de quay goc 90 do, chu ki dao dong 16ms => duty cycle = 1.5/16*4(PR2+1)=96
#define VITRI_4	112
#define VITRI_5	125

void Init_UART(void);
//void Init_Tran_UART(void);
//void Init_Rec_UART(void);
void Tran_UART(unsigned char ch);
void Pwm_init(void);
void Processing(void);

#endif	/* DEFINITION_H */

