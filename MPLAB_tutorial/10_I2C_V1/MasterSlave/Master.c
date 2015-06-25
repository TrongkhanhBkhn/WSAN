#include "generic.h"

//configure special register
#pragma config FOSC = HS
#pragma config FCMEN = OFF, BOREN = OFF
#pragma config WDTEN = OFF, MCLRE = ON, LVP = OFF

//main function
void main(void)
{
    //string that send to slave
    char str1 = "NGUYEN TIEN DAT";
    char str2 = "KSTN-DTVT-K54"

    //initialize led
    TRISB = 0x00;
    LATB = 0x00;

    //initialize MSSP, mode I2C
    SSPADD = Fosc/(4*Fscl) - 1;//set speed transmission
    OpenI2C(MASTER ,SLEW_OFF);

    //

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
