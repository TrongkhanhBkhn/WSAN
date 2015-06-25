//include library
#include <p18f26k20.h>
#include <Delays.h>

//define type data
typedef unsigned char BYTE;
typedef unsigned short int WORD;

//define something useful
//pins control van
#define PUMP PORTAbits.RA0
#define VAN1 PORTAbits.RA1
#define VAN2 PORTAbits.RA2
#define VAN3 PORTAbits.RA3
#define VAN4 PORTAbits.RA4
#define VAN5 PORTAbits.RA5

//constants
#define SET 1
#define ON 1
#define CLEAR 0
#define OFF 0

#define DEBUG

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
    #define OffVan1Auto     0x41
	#define OffVan2Auto     0x42
	#define OffVan3Auto     0x43
	#define OffVan4Auto     0x44
	#define OffVan5Auto     0x45
	#define OffAllVanAuto   0x4F
#else
//commands may be receive from Zigbee Node
#define OnVan1          0x81//
#define OnVan2          0x82
#define OnVan3          0x83
#define OnVan4          0x84
#define OnVan5          0x85
#define OnAllVan	0x8F

#define OffVan1         0x01
#define OffVan2         0x02
#define OffVan3         0x03
#define OffVan4         0x04
#define OffVan5         0x05
#define OffAllVan	0x0F

#define OffVan1Auto     0x41
#define OffVan2Auto     0x42
#define OffVan3Auto     0x43
#define OffVan4Auto     0x44
#define OffVan5Auto     0x45
#define OffAllVanAuto   0x4F
#endif

#define OneMinute 60
#define TimeOut 1*OneMinute


//functions
void InitUART(void);
void InitTimer0(void);
void TurnOff(void);
void TurnOn(void);
void TurnOffPump(void);
void ProcessCommand(void);//ham xu ly lenh
void ConfirmToZigbeeNode(BYTE Message);
void InterruptService(void);
