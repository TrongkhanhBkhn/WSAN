/* 
 * File:   define.h
 * Author: TienDat
 *
 * Created on August 5, 2013, 6:19 PM
 */

#ifndef DS1307_H
#define	DS1307_H

/*-------------------------------include library-------------------------------*/
#include "generic.h"

/*----------------------------------definitions---------------------------------*/
//read from ds1307 or write to ds1307
#define READ    1
#define WRITE   0

//address of ds1307
#define DS1307_ADD 0x68

//modes display time
#define AM 0
#define PM 1
#define FULL 2

/*------------------------------function prototype------------------------------*/
//configure mode of MCU (master/slave, speed)
void i2c_config(void);

//read second, minute and hour from ds1307
unsigned char* get_ds1307_time(void);
//read day, date, month and year from ds1307
unsigned char* get_ds1307_day(void);

//write second, minute and hour to ds1307
void set_ds1307_time(unsigned char mode ,unsigned char sec,unsigned char min,unsigned char hour);
//write day, date, month and year to ds1307
void set_ds1307_day(unsigned char day,unsigned char date,unsigned char month,unsigned char year);

//transaction data between MCU and ds1307
void write_i2c_to_ds1307(unsigned char address1,unsigned char *data1,unsigned char val1);
void read_i2c_from_ds1307(unsigned char address,unsigned char *data,unsigned char val);

//convert number between bcd and decimal
unsigned char bcdToDec(unsigned char BCD);
unsigned char decToBcd(unsigned char DEC);


#endif	/* API_H */

