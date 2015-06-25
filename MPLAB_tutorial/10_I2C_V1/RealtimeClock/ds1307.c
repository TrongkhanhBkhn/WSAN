#include "ds1307.h"

//variable
extern unsigned char i, temp;
extern unsigned char udata[4];

void i2c_config(void)
{
    SSPADD = Fosc/(4*Fscl) - 1;//set speed transmission
    OpenI2C(MASTER ,SLEW_OFF);
}

void set_ds1307_time(unsigned char mode ,unsigned char sec,unsigned char min,unsigned char hour)
{
    udata[0]=decToBcd(sec);
    udata[1]=decToBcd(min);
    udata[2]=decToBcd(hour);

    switch(mode)
    {
        case AM: udata[2]|=0x40; break;
        case PM: udata[2]|=0x60; break;
        default: break;
    }
    write_i2c_to_ds1307(0x00,udata,3);
}

void set_ds1307_day(unsigned char day,unsigned char date,unsigned char month,unsigned char year)
{
    udata[0]=decToBcd(day);
    udata[1]=decToBcd(date);
    udata[2]=decToBcd(month);
    udata[3]=decToBcd(year);

    write_i2c_to_ds1307(0x03,udata,4);
}

unsigned char* get_ds1307_time(void)
{
    read_i2c_from_ds1307(0x00,udata,3);

    temp = udata[0];
    udata[0] = bcdToDec(temp);

    temp = udata[1];
    udata[1] = bcdToDec(temp);

    if(udata[2]&0x40)//mode 12 hour
    {
        if(udata[2]&0x20)
            udata[3] = PM;
        else
            udata[3] = AM;

        temp = udata[2];
        udata[2] = bcdToDec(temp&0x1F);
    }
    else
    {
        udata[3] = FULL;
        temp = udata[2];
        udata[2] = bcdToDec(temp&0x3F);
    }
    return udata;
}

unsigned char* get_ds1307_day(void)
{
    read_i2c_from_ds1307(0x03,udata,4);

    temp = udata[0];
    udata[0] = bcdToDec(temp);

    temp = udata[1];
    udata[1] = bcdToDec(temp);

    temp = udata[2];
    udata[2] = bcdToDec(temp);

    temp = udata[3];
    udata[3] = bcdToDec(temp);

    return udata;
}

unsigned char decToBcd(unsigned char DEC)
{
    unsigned char L,H;
    H = (DEC/10)<<4;
    L = (DEC%10);
    return (H|L);
}
unsigned char bcdToDec(unsigned char BCD)
{
    unsigned char L, H;
    L = (BCD & 0x0F);
    H = (BCD>>4)*10;
    return(H+L);
}

void write_i2c_to_ds1307(unsigned char address1,unsigned char *data1,unsigned char val1)
{
    //start condition
    StartI2C();
    IdleI2C();

    //select slave (ds1307) to write data
    WriteI2C((DS1307_ADD << 1)| WRITE);
    IdleI2C();

    //Ghi <val1> bytes vao DS1307 bat dau tu thanh ghi co dia chi <address1>
    WriteI2C(address1);
    for(i=0;i<val1;i++)
    {
        IdleI2C();
        WriteI2C(data1[i]);
    }

    //stop condition
    StopI2C();
}

void read_i2c_from_ds1307(unsigned char address,unsigned char *data,unsigned char val)
{
    //start condition
    StartI2C();
    IdleI2C();

    //select slave (ds1307) to read data
    WriteI2C((DS1307_ADD << 1)| WRITE);
    IdleI2C();

    //select register of slave to read data
    WriteI2C(address);
    IdleI2C();

    //restart condition
    RestartI2C();
    IdleI2C();

    //request read data from slave (ds1307)
    WriteI2C((DS1307_ADD << 1)| READ);
    IdleI2C();

    //start read data from selection address
    data[0] = ReadI2C();
    for(i = 1; i < val; i++)
    {
        IdleI2C();
        AckI2C();
        data[i] = ReadI2C();
    }

    //Not ACK condition to slave stop send data
    NotAckI2C();
    IdleI2C();

    //Stop conditon
    StopI2C();
}
