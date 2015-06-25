 #include "definition.h"

//variables
extern BYTE EnableProcessFlag;//Cho phep

extern BYTE Command;
extern BYTE status;
extern BYTE Control;//bien nhan lenh dieu khien bat/tat

extern WORD time1, time2, time3, time4, time5;//soft timer
void ProcessCommand()
{
    if(Control == OFF)
    {
        TurnOff();
    }
    else
    {
        TurnOn();
    }

//    //gui lenh xac nhan len node Zigbee thong qua module UART
//    ConfirmToZigbeeNode(Command);
}
void TurnOn()
{
    switch(Command)
    {
        case OnVan1:
        if(VAN1 == OFF)
        {
            VAN1 = ON;//bat van 1
            time1 = 1;//bat dau dem thoi gian mo van 1
        }
        break;

        case OnVan2:
        if(VAN2 == OFF)
        {
            VAN2 = ON;
            time2 = 1;
        }

        break;

        case OnVan3:
        if(VAN3 == OFF)
        {
            VAN3 = ON;
            time3 = 1;
        }

        break;

        case OnVan4:
        if(VAN4 == OFF)
        {
            VAN4 = ON;
            time4 = 1;
        }
        break;

        case OnAllVan:
        if(VAN1 == OFF)
        {
            VAN1 = ON;//bat van 1
            time1 = 1;//bat dau dem thoi gian mo van 1
        }
        if(VAN2 == OFF)
        {
            VAN2 = ON;
            time2 = 1;
        }
        if(VAN3 == OFF)
        {
            VAN3 = ON;
            time3 = 1;
        }
        if(VAN4 == OFF)
        {
            VAN4 = ON;
            time4 = 1;
        }
        if(VAN5 == OFF)
        {
            VAN5 = ON;
            time5 = 1;
        }
        break;

        default:
        break;
    }
    if(PUMP == OFF)
        PUMP = ON;
}

void TurnOff()
{
    //control turn off pump
    status = 0x3F & PORTA;
    switch(status)
    {
        //if has only a van is
        case 0b00000011:
        case 0b00000101:
        case 0b00001001:
        case 0b00010001:
        case 0b00100001:
            TurnOffPump();
            break;

        default:
            if((Command == OffAllVan)||(Command == OffAllVanAuto))
            {
                TurnOffPump();
            }
            break;
    }

    //control turn off van
    switch(Command)
    {
        case OffVan1:
        case OffVan1Auto:
            VAN1 = OFF;
            time1 = CLEAR;
            break;

        case OffVan2:
        case OffVan2Auto:
            VAN2 = OFF;
            time2 = CLEAR;
            break;

        case OffVan3:
        case OffVan3Auto:
            VAN3 = OFF;
            time3 = CLEAR;
            break;

        case OffVan4:
        case OffVan4Auto:
            VAN4 = OFF;
            time4 = CLEAR;
            break;

        case OffVan5:
        case OffVan5Auto:
            VAN5 = OFF;
            time5 = CLEAR;
            break;

        case OffAllVan:
        case OffAllVanAuto:
            VAN1 = OFF;
            VAN2 = OFF;
            VAN3 = OFF;
            VAN4 = OFF;
            VAN5 = OFF;
            time1 = CLEAR;
            time2 = CLEAR;
            time3 = CLEAR;
            time4 = CLEAR;
            time5 = CLEAR;
            break;

        default:
            break;
    }
}

void TurnOffPump()
{
    PUMP = OFF;//tat bom
    Delay10KTCYx(200);//delay 1s @ 8MHz
    Delay10KTCYx(200);//delay 1s @ 8MHz
}
void ConfirmToZigbeeNode(BYTE Message)
{
    while(!TXSTAbits.TRMT);
    TXREG = Message;
}

