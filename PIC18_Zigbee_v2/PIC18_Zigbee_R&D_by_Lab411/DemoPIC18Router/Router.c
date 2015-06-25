/*******************************************************************************
 * Microchip ZigBee2006 Residential Stack
 *
 * Zigbee Router
 *
 * Day la ma nguon de cau hinh cho mot node tro thanh Router theo chuan giao thuc
 * Zigbee. Ma nguon chay tren phan cung duoc thiet ke boi nhom WSAN - lab411.
 * Trong ma nguon cua nhom, co su dung kien truc Microchip Stack de xay dung cac
 * ung dung theo chuan giao tiep khong day Zigbee. De hieu duoc hoat dong cua he
 * thong, hay doc tai lieu WSAN Specification
 *
 *******************************************************************************
 * FileName:        Router.c
 * Project:         DemoPIC18Router
 * Version:         2.0
 *
 * Controller:      PIC18F26K20
 * Editor:          MPLAB X IDE v1.41
 * Complier:        MCC18 v3.20 or higher
 *
 * Company support: Microchip Technology, Inc.
 *
 * Developer:       Nguyen Tien Dat - KSTN - DTVT - K54
 * Group:           WSAN group - Lab411
 * Edition:         12/12/2012
 * 
 *******************************************************************************/

//******************************************************************************
// Header Files
//******************************************************************************

// Include the main ZigBee header file.
//#include "zAPL.h"

#ifdef I_SUPPORT_SECURITY
    #include "zSecurity.h"
#endif

// If you are going to send data to a terminal, include this file.
#if defined(USE_USART) || defined(USE_CONTROL_PUMP) || defined(USE_MicroWaveS)
    #include "Console.h"
#endif

// If you are going to get temperature and humidity, include this file.
#if defined(USE_SHT10)
    #include "delay.h"
    #include "SHT1x.h"
#endif

//******************************************************************************
// Configuration Bits
//******************************************************************************
#pragma romdata CONFIG1H = 0x300001
const rom BYTE config1H = 0b01001000;// INTOSC with RA6 & RA7 are function port, enabled Fail-Safe Clock Monitor

#pragma romdata CONFIG2L = 0x300002
const rom BYTE config2L = 0b00011110;// Brown-out Reset Enabled in hardware @ 1.8V, PWRTEN enabled

#pragma romdata CONFIG2H = 0x300003
const rom BYTE config2H = 0b00010010;// WDT 1:512 postscale, WDT is controlled by SWDTEN bit

#pragma romdata CONFIG3H = 0x300005
const rom BYTE config3H = 0b10000100;// PORTB digital on RESET, MCLR pin enabled
                                     // The system lock is held off until the HFINTOSC is stable

#pragma romdata CONFIG4L = 0x300006
const rom BYTE config4L = 0b10000001;// Stack full will cause reset, LVP off
                                     // Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins

#pragma romdata

//******************************************************************************
// Constants: defined application service
//******************************************************************************
// Gia tri dung de thiet lap cho thanh ghi OSCCON
#define INTOSC_16MHz    0x70

#define ON              1
#define SET             1
#define Enable		1
#define OFF             0
#define CLEAR           0
#define Disable		0

// Gia tri cho biet bo ADC dang do tin hieu tu cam bien khoi hay tu nguon cap
#define MQ6_Mode_ADC    0b00
#define Power_Mode_ADC  0b01
#define Finish_Convert  0b10

// Cac trang thai cua node mang
// 0x02 --> phat hien co khoi
// 0x03 --> muc nang luong co dau hieu can kiet
// 0x04 --> phat hien co doi tuong dang xam nhap
#define HasSmoke        0x02
#define LowPower        0x03
#define StrangeObject   0x04

// Cac gia tri nay dung de thiet lap cho thanh ghi ADCON1 dieu khien do tin hieu tu dau
#define Measure_MQ6     0x10
#define Measure_Voltage 0x00

// Gia tri nguong cua cac tin hieu tu cam bien khoi va nguon nang luong
// cho phep node gui tin hieu canh bao ve Router-EMB
#define ThresholdMQ6	900
#define ThresholdPower	1350

//******************************************************************************
// Function Prototypes
//******************************************************************************

extern void RemoveAllGroups(void);

#if defined(I_SUPPORT_BINDINGS)
    extern void RemoveAllBindings(SHORT_ADDR);
#endif
extern BOOL LookupAPSAddress(LONG_ADDR *);

// Functions created and modified by lab411
void HardwareInit(void);
void ProcessZigBeePrimitives(void);
void ProcessNONZigBeeTasks(void);

void SendOneByte(BYTE data, WORD ClusterID);

#if defined(USE_SHT10)
    void Send_HTE_ToRouterEmboard(void);
    void LoadSHT10(void);
#endif

#if defined(USE_DEBUG_MAIN)
    void PrintRoutingTable(void);
#endif

// End by lab411


//******************************************************************************
// Application Variables                                @modified by dat_a3cbq91
//******************************************************************************

// Variables created by lab411
#if defined(ENERGY_TRACKING)
    WORD Energy_Level;//Luu tru ket qua do dien ap nguon 
#endif

#if defined(USE_MQ6)
    WORD Mq6Signal;//Luu tru ket qua do tin hieu tu cam bien khoi MQ6
#endif

#if defined(ENERGY_TRACKING) && defined(USE_MQ6)
    WORD ADC_result;//Luu tru gia tri lay mau tu ADC module @dat_a3cbq91
#endif

#if defined(USE_SHT10)
    WORD humidity, temperature;//Luu tru gia tri do am, nhiet do lay tu SHT chua qua xu ly @dat_a3cbq91
#endif

#if defined(USE_CONTROL_PUMP)
    volatile BYTE CmdPump;
#endif
static union
{
    struct
    {
                WORD MQ6Warning             : 1;//when has smoke, set this flag
                WORD LowPowerWarning        : 1;//when voltage drop-out, set this flag
                WORD EnableGetDataHTE       : 1;//when need to print humi-temp data on terminal, set this flag
                WORD HTE_Data_Ready         : 1;
                WORD PrintNeighborTable     : 1;
                WORD MicrowaveDetecting     : 1;
                WORD PumpAckReceive         : 1;

        #if defined(USE_MQ6)||defined(ENERGY_TRACKING)
            #if defined(USE_MQ6) && defined(ENERGY_TRACKING)
                WORD MQ6orVoltage           : 2;
                WORD Reserve                : 7;
            #else
                WORD CompleteADC            : 1;
                WORD Reserve                : 8;
            #endif
        #else
                WORD Reserve                : 9;
        #endif
    } bits;
    WORD Val;
} WSANFlags;

#define STATUS_FLAGS_INIT       0x00
// End by lab411

ZIGBEE_PRIMITIVE currentPrimitive  = NO_PRIMITIVE;
NETWORK_DESCRIPTOR *currentNetworkDescriptor;
NETWORK_DESCRIPTOR *NetworkDescriptor;

BYTE orphanTries;
BYTE AllowJoin = 1;
BYTE i;

#ifdef I_SUPPORT_SECURITY
    extern KEY_VAL KeyVal;
    #ifdef USE_EXTERNAL_NVM
        extern WORD trustCenterLongAddr;
        extern NETWORK_KEY_INFO plainSecurityKey[2];
    #else
        extern ROM LONG_ADDR trustCenterLongAddr;
    #endif
#endif

extern NWK_STATUS nwkStatus;

//******************************************************************************
//******************************************************************************
//******************************************************************************
void main(void)
{
    /* Initialize both Hardware and Stack */
    CLRWDT();
    ENABLE_WDT();
    
    OSCCON = INTOSC_16MHz;

    //currentPrimitive = NO_PRIMITIVE;
    NetworkDescriptor = NULL;
    orphanTries = 3;

    /* Initialize the UART such that data can be sent and recieved on terminal */
    #if defined(USE_USART) || defined(USE_CONTROL_PUMP) || defined(USE_MicroWaveS)
	ConsoleInit();
    #endif

    /* Initialize the hardware before initializing the ZigBee Stack */
    HardwareInit();
    #if defined(USE_USART)
        printf("Init Hardware\r\n");
    #endif

    /* Initialize the ZigBee Stack */
    ZigBeeInit();
    #if defined(USE_USART)
        printf("Init Zigbee\r\n");
    #endif

    // *************************************************************************
    // Perform any other initialization here
    // *************************************************************************
    #if defined(USE_USART)
	/********************DEFINE SONSOR NODE***********************************/
        #if defined(SENSOR1)
            printf("R&D ZigBee-Router SENSOR1\r\n");
        #elif defined(SENSOR2)
            printf("R&D ZigBee-Router SENSOR2\r\n");
        #elif defined(SENSOR3)
            printf("R&D ZigBee-Router SENSOR3\r\n");
        #elif defined(SENSOR4)
            printf("R&D ZigBee Router Sensor4\r\n");
        #elif defined(SENSOR5)
            printf("R&D ZigBee Router Sensor5\r\n");
        #elif defined(SENSOR6)
            printf("R&D ZigBee Router Sensor6\r\n");
        #elif defined(SENSOR7)
            printf("R&D ZigBee Router Sensor7\r\n");
        #elif defined(SENSOR8)
            printf("R&D ZigBee Router Sensor8\r\n");
        #elif defined(SENSOR9)
            printf("R&D ZigBee Router Sensor9\r\n");
        #elif defined(SENSOR10)
            printf("R&D ZigBee Router Sensor10\r\n");

	/************************DEFINE ROUTER_EMBOARD********************/
        #elif defined(ROUTER_EMB)
            printf("R&D ZigBee Router_Emb\r\n");
	/************************DEFINE ACTOR NODE************************/
        #elif defined(ACOTR1)
            printf("R&D ZigBee Router Actor1\r\n");
        #elif defined(ACOTR2)
            printf("R&D ZigBee Router Actor2\r\n");
        #else
            #error "Router is not supported."
        #endif

        #if (RF_CHIP == MRF24J40)
            printf("Transceiver-MRF24J40\r\n");
        #else
            printf("Transceiver-Unknown\r\n");
        #endif
    #endif

    /* Enable interrupts to get the stack operational */
    RCONbits.IPEN = Enable; // Enable interrupt priority
    INTCONbits.GIEH = Enable; // Enables all high priority interrupts
    INTCONbits.GIEL = Enable; // Enables all low priority interrupts

    /* Initialize my status flags*/
    WSANFlags.Val = STATUS_FLAGS_INIT;

    /* Start the network anew each time Node is booted up */
    NWKClearNeighborTable();
    #if defined(I_SUPPORT_BINDINGS)
        ClearBindingTable();
    #endif

    /* Clear the Group Table */
    RemoveAllGroups();
        
    #if defined(I_SUPPORT_BINDINGS)
        RemoveAllBindings(macPIB.macShortAddress);
    #endif

    /* Clearing nwk status flags */
    nwkStatus.flags.Val = 0x00;

    while (1)
    {
        CLRWDT();

        /* Determine which is the next ZigBee Primitive to operate on */
        ZigBeeTasks( &currentPrimitive );

        /* Process the next ZigBee Primitive */
        ProcessZigBeePrimitives();

        /* do any non ZigBee related tasks and then go back to ZigBee tasks */
        ProcessNONZigBeeTasks();
    }
}

void ProcessZigBeePrimitives(void)
{
    switch (currentPrimitive)
    {
        case NLME_ROUTE_DISCOVERY_confirm:
            #if defined(USE_USART) || defined(ROUTER_EMB)
                if (!params.NLME_ROUTE_DISCOVERY_confirm.Status)
                {
                    printf("Route Reply OK\r\n" );
                }
                else
                {
                    PrintChar( params.NLME_PERMIT_JOINING_confirm.Status );
                    printf(" Route Reply Failed\r\n" );
                }
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case NLME_NETWORK_DISCOVERY_confirm:
            currentPrimitive = NO_PRIMITIVE;
            if (!params.NLME_NETWORK_DISCOVERY_confirm.Status)
            {
                if (!params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount)//neu khong tim duoc mang Zigbee nao @dat_a3cbq91
                {
                    #if defined(USE_USART)
                        printf("No networks found. Trying again!\r\n");
                    #endif
                }
                else
                {
                    // Save the descriptor list pointer so we can destroy it later.
                    NetworkDescriptor = params.NLME_NETWORK_DISCOVERY_confirm.NetworkDescriptor;

                    // Select a network to try to join. We're not going to be picky right now...
                    currentNetworkDescriptor = NetworkDescriptor;

SubmitJoinRequest:
                    params.NLME_JOIN_request.PANId = currentNetworkDescriptor->PanID;
                    params.NLME_JOIN_request.JoinAsRouter = TRUE;
                    params.NLME_JOIN_request.RejoinNetwork = FALSE;
                    params.NLME_JOIN_request.PowerSource = NOT_MAINS_POWERED;
                    params.NLME_JOIN_request.RxOnWhenIdle = TRUE;
                    params.NLME_JOIN_request.MACSecurity = FALSE;
                    params.NLME_JOIN_request.ExtendedPANID = currentNetworkDescriptor->ExtendedPANID;
                    currentPrimitive = NLME_JOIN_request;//gui don xin gia nhap mang

                    /* In ra PAN ID cua mang se tien hanh xin gia nhap */
                    #if defined(USE_USART)
                        printf("Network(s) found. Trying to join " );
                        PrintWord(params.NLME_JOIN_request.PANId.Val);
                        printf(" | ");
                        for(i=7;i^0xFF;i--)
                        {
                            PrintChar(currentNetworkDescriptor->ExtendedPANID.v[i]);
                        }
                        printf("\r\n");
                    #endif
                }
            }
            else
            {
                #if defined(USE_USART)
                    printf("Error finding network: " );
                    PrintChar( params.NLME_NETWORK_DISCOVERY_confirm.Status );
                    printf(". Trying again!\r\n");
                #endif
            }
            break;

        case NLME_JOIN_confirm:
            if (!params.NLME_JOIN_confirm.Status)
            {
                #if defined(USE_USART)
                    printf("Join OK!\r\n");
                #endif

                // Free the network descriptor list, if it exists. If we joined as an orphan, it will be NULL.
                while (NetworkDescriptor)
                {
                    currentNetworkDescriptor = NetworkDescriptor->next;
                    nfree(NetworkDescriptor);//giai phong bo nho @dat_a3cbq91
                    NetworkDescriptor = currentNetworkDescriptor;
                }

                // Start routing capability.
                params.NLME_START_ROUTER_request.BeaconOrder = MAC_PIB_macBeaconOrder;
                params.NLME_START_ROUTER_request.SuperframeOrder = MAC_PIB_macSuperframeOrder;
                params.NLME_START_ROUTER_request.BatteryLifeExtension = FALSE;
                currentPrimitive = NLME_START_ROUTER_request;
                
                //T1CONbits.TMR1ON = ON;//enable timer1 @dat_a3cbq91
            }
            else
            {
                currentPrimitive = NO_PRIMITIVE;
                #if defined(USE_USART)
                    PrintChar(params.NLME_JOIN_confirm.Status);
                    printf(" Status. ");
                #endif

                // If we were trying as an orphan, see if we have some more orphan attempts.
                if (ZigBeeStatus.flags.bits.bTryOrphanJoin)// Phan nay dam nhan viec ReJoin vs trang thai Orphan
                {
                    // If we tried to join as an orphan, we do not have NetworkDescriptor, so we do
                    // not have to free it.
                    #if defined(USE_USART)
                        printf("Could not join as orphan. ");
                    #endif
                    
                    orphanTries--;
                    if (orphanTries)
                    {
                        #if defined(USE_USART)
                            printf("Trying again!\r\n" );
                        #endif

                    }
                    else
                    {
                        orphanTries = 3;
                        #if defined(USE_USART)
                            printf("Must try as new node!\r\n" );
                        #endif
                        ZigBeeStatus.flags.bits.bTryOrphanJoin = CLEAR;
                    }
                }
                else
                {
                    #if defined(USE_USART)
                    	printf("Could not join selected network. ");
                    #endif
                    
                    currentNetworkDescriptor = currentNetworkDescriptor->next;
                    if (currentNetworkDescriptor)
                    {
                        #if defined(USE_USART)
                            printf("Trying next discovered network\r\n");
                        #endif
                        goto SubmitJoinRequest;
                    }
                    else
                    {
                        //Ran out of descriptors.  Free the network descriptor list, and fall
                        // through to try discovery again.
                        #if defined(USE_USART)
                            printf("Cleaning up and retrying discovery\r\n");
                        #endif
                        while (NetworkDescriptor)
                        {
                            currentNetworkDescriptor = NetworkDescriptor->next;
                            nfree(NetworkDescriptor);
                            NetworkDescriptor = currentNetworkDescriptor;
                        }
                    }
                }
            }
            break;

        case NLME_START_ROUTER_confirm:
            if (!params.NLME_START_ROUTER_confirm.Status)
            {
                #if defined(USE_USART)
                    printf("Router Started! Enabling joins\r\n" );
                #endif
                params.NLME_PERMIT_JOINING_request.PermitDuration = 0xFF;
                params.NLME_PERMIT_JOINING_request._updatePayload = TRUE;

                currentPrimitive = NLME_PERMIT_JOINING_request;
            }
            else
            {
                #if defined(USE_USART)
                    printf("Router start failed: ");
                    PrintChar( params.NLME_JOIN_confirm.Status);
                    printf(". We cannot route frames\r\n");
                #endif
                
                currentPrimitive = NO_PRIMITIVE;
            }
            break;

        case NLME_PERMIT_JOINING_confirm:
            // If Join Completed --> Send now infomation to Router EMB
            if (!params.NLME_PERMIT_JOINING_confirm.Status)
            {
                #if defined(USE_USART)
                    printf("Has joined!\r\n");
                #endif

                // Gui thong bao ve Gateway da gia nhap mang
                SendOneByte(MAC_LONG_ADDR_BYTE0, CONFIRM_JOIN_NETWORK_CLUSTER);
                #if defined(USE_LED)
                    LATBbits.LATB6 = 1;
                #endif
                #if defined(ENERGY_TRACKING) || defined(USE_MQ6)
                    ADCON0bits.GO = ON;//bat dau lay mau tin hieu tu cam bien khoi hoac do dien ap
                #endif
            }
            else
            {
                #if defined(USE_USART)
                    PrintChar( params.NLME_PERMIT_JOINING_confirm.Status );
                    printf(" Join permission failed\r\n" );
                #endif

                currentPrimitive = NO_PRIMITIVE;
            }
            break;

        case NLME_JOIN_indication:
            #if defined(USE_USART)
            {
                printf("Node ");
                /* In ra dia chi mang cua node vua join */
                PrintWord(params.NLME_JOIN_indication.ShortAddress.Val);
                /* In ra dia chi MAC  cua node vua join */
                printf(" with MAC Address ");
                for(i = 7; i ^ 0xFF; --i)
                    PrintChar(params.NLME_JOIN_indication.ExtendedAddress.v[i]);
                printf(" just joined\r\n");
            }
            #endif

            /* For Zigbee 2006: If a new device with the same old longAddress address
            * joins the PAN, then make sure the old short address is no longer used and is
            * overwritten with the new shortAddress & longAddress combination
            */
            /* Neu mot thiet bi da tung la thanh vien cua mang, can phai dam bao
             * la dia chi mang khong duoc dung nua va cap (dia chi mang & dia chi MAC)
             * se duoc ghi lai trong bang @dat_a3cbq91
             */
            {  /* same long address check block */
                APS_ADDRESS_MAP currentAPSAddress1;
                currentAPSAddress1.shortAddr   =   params.NLME_JOIN_indication.ShortAddress;
                currentAPSAddress1.longAddr    =   params.NLME_JOIN_indication.ExtendedAddress;

                if(LookupAPSAddress(&params.NLME_JOIN_indication.ExtendedAddress) )//neu node vua gia nhap co dia chi MAC giong voi mot dia chi MAC
                                                                                   //cua mot node da tung gia nhap mang truoc day
                {
                    for( i = 0; i < apscMaxAddrMapEntries; i++)
                    {
                        GetAPSAddress( &currentAPSAddress,  &apsAddressMap[i] );

                        if (!memcmp( (void *)&currentAPSAddress.longAddr, (void *)&currentAPSAddress1.longAddr, 8 ))
                        {
                            /* overwrite the old with the new short/long address combo  */
                            PutAPSAddress( &apsAddressMap[i], &currentAPSAddress1 );// ghi dia chi MAC va dia chi mang moi cua node moi gia nhap vao bo nho chuong trinh
                        }
                    }   /* end for loop */
                }       /* end if */
            }           /* end address check block */

            #ifdef I_SUPPORT_SECURITY
                #ifdef I_AM_TRUST_CENTER
                {
                    BOOL AllowJoin = TRUE;
                    // decide if you allow this device to join
                    if (!AllowJoin)
                    {
                        // no need to set deviceAddress, since it is overlap with NLME_JOIN_indication
                        //params.NLME_LEAVE_request.DeviceAddress = params.NLME_JOIN_indication.ExtendedAddress;
                        params.NLME_LEAVE_request.RemoveChildren = TRUE;
                        currentPrimitive = NLME_LEAVE_request;
                        break;
                    }

                    #ifdef I_SUPPORT_SECURITY_SPEC
                        if (params.NLME_JOIN_indication.secureJoin)
                        {
                            BYTE i;
                            for (i = 0; i < 16; i++)
                                KeyVal.v[i] = 0;
                            params.APSME_TRANSPORT_KEY_request.Key = &KeyVal;
                            params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = 0;

                        }
                        else
                        {
                            BYTE i;
                            GetNwkActiveKeyNumber(&i);
                            #ifdef USE_EXTERNAL_NVM
                                currentNetworkKeyInfo = plainSecurityKey[i - 1];
                            #else
                                GetNwkKeyInfo(&currentNetworkKeyInfo, (ROM void *) &(NetworkKeyInfo[i - 1]));
                            #endif
                            params.APSME_TRANSPORT_KEY_request.Key = &(currentNetworkKeyInfo.NetKey);
                            params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = currentNetworkKeyInfo.SeqNumber.v[0];

                        }
                    #else
                        #ifdef PRECONFIGURE_KEY
                            {
                                BYTE i;
                                for (i = 0; i < 16; i++)
                                    KeyVal.v[i] = 0;
                                params.APSME_TRANSPORT_KEY_request.Key = &KeyVal;
                                params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = 0;
                                params.APSME_TRANSPORT_KEY_request._UseSecurity = TRUE;
                            }
                        #else
                            if (params.NLME_JOIN_indication.secureJoin)
                            {
                                BYTE i;
                                for (i = 0; i < 16; i++)
                                    KeyVal.v[i] = 0;
                                params.APSME_TRANSPORT_KEY_request.Key = &KeyVal;
                                params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = 0;
                                params.APSME_TRANSPORT_KEY_request._UseSecurity = TRUE;
                            }
                            else
                            {
                                BYTE i;
                                GetNwkActiveKeyNumber(&i);
                                #ifdef USE_EXTERNAL_NVM
                                    currentNetworkKeyInfo = plainSecurityKey[i - 1];
                                #else
                                    GetNwkKeyInfo(&currentNetworkKeyInfo, (ROM void *) &(NetworkKeyInfo[i - 1]));
                                #endif
                                params.APSME_TRANSPORT_KEY_request.Key = &(currentNetworkKeyInfo.NetKey);
                                params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = currentNetworkKeyInfo.SeqNumber.v[0];
                                params.APSME_TRANSPORT_KEY_request._UseSecurity = FALSE;
                            }
                        #endif

                    #endif
                        params.APSME_TRANSPORT_KEY_request.KeyType = ID_NetworkKey;
                        params.APSME_TRANSPORT_KEY_request.DestinationAddress = params.NLME_JOIN_indication.ExtendedAddress;
                        params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.UseParent = FALSE;
                        currentPrimitive = APSME_TRANSPORT_KEY_request;
                }
                #else
                    #ifdef I_SUPPORT_SECURITY_SPEC
                        params.APSME_UPDATE_DEVICE_request.Status = (params.NLME_JOIN_indication.secureJoin) ? 0x00 : 0x01;
                    #else
                        #ifdef PRECONFIGURE_KEY
                            params.APSME_UPDATE_DEVICE_request.Status = 0x00;
                        #else
                            params.APSME_UPDATE_DEVICE_request.Status = 0x01;
                        #endif
                    #endif
                    params.APSME_UPDATE_DEVICE_request.DeviceShortAddress = params.NLME_JOIN_indication.ShortAddress;
                    params.APSME_UPDATE_DEVICE_request.DeviceAddress = params.NLME_JOIN_indication.ExtendedAddress;
                    GetTrustCenterAddress(&params.APSME_UPDATE_DEVICE_request.DestAddress);
                    for (i = 0; i < 8; i++)
                        params.APSME_UPDATE_DEVICE_request.DestAddress.v[i] = 0xaa;

                    currentPrimitive = APSME_UPDATE_DEVICE_request;
                #endif
            #else
                currentPrimitive = NO_PRIMITIVE;
            #endif
            break;

        case NLME_LEAVE_indication:
            {
                if (!memcmppgm2ram(&params.NLME_LEAVE_indication.DeviceAddress, (ROM void *) &macLongAddr, 8))
                {
                    #if defined(USE_USART)
                        printf("We have left the network\r\n");
                    #endif
                }
                else
                {
                    #if defined(USE_USART)
                        printf("Another node has left the network\r\n");
                    #endif
                }
            }
            #ifdef I_SUPPORT_SECURITY
            {
                SHORT_ADDR LeavingChildShortAddress;
                if (!APSFromLongToShort(&params.NLME_LEAVE_indication.DeviceAddress))
                {
                    currentPrimitive = NO_PRIMITIVE;
                    break;
                }
                LeavingChildShortAddress = currentAPSAddress.shortAddr;

                #ifdef I_AM_TRUST_CENTER
                    params.APSME_UPDATE_DEVICE_indication.Status = 0x02;
                    params.APSME_UPDATE_DEVICE_indication.DeviceAddress = params.NLME_LEAVE_indication.DeviceAddress;
                    GetMACAddress(&params.APSME_UPDATE_DEVICE_indication.SrcAddress);
                    params.APSME_UPDATE_DEVICE_indication.DeviceShortAddress = LeavingChildShortAddress;
                    currentPrimitive = APSME_UPDATE_DEVICE_indication;
                    break;
                #else
                    params.APSME_UPDATE_DEVICE_request.Status = 0x02;
                    GetTrustCenterAddress(&params.APSME_UPDATE_DEVICE_request.DestAddress);
                    params.APSME_UPDATE_DEVICE_request.DeviceShortAddress = LeavingChildShortAddress;
                    currentPrimitive = APSME_UPDATE_DEVICE_request;
                    break;
                #endif
            }

            #else
                currentPrimitive = NO_PRIMITIVE;
            #endif
            break;

        case NLME_RESET_confirm:
            #if defined(USE_USART)
                printf("Reset ZigBee Stack\r\n");
            #endif

            
            /* For Zigbee 2006 The Specs require that node needs to
             * try joining as an orphan first after each reset,
             * see Mandatory test 3.9
             */
            ZigBeeStatus.flags.bits.bTryOrphanJoin = SET;

            currentPrimitive = NO_PRIMITIVE;
            break;

        case NLME_LEAVE_confirm:
            #if defined(USE_USART)
                PrintChar(params.NLME_LEAVE_confirm.Status);
                printf(" Leaving the Zigbee network\r\n");
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSDE_DATA_indication:
        {
            BYTE data;
            currentPrimitive = NO_PRIMITIVE;
            switch (params.APSDE_DATA_indication.DstEndpoint)
            {
                // ************************************************************************
                // Place a case for each user defined endpoint.
                // ************************************************************************
                case WSAN_Dst:
                {
                    

                    switch (params.APSDE_DATA_indication.ClusterId.Val)
                    {
                        #if defined(USE_CONTROL_PUMP)
                        case ACTOR_REQUEST_CLUSTER:
                        {
                            //cau truc ban tin yeu cau bat bom tu router-emboard
                            //------------------------------------------------------------------------------------
                            // 0bSVVVVVVV
                            // S = 0 --> tat, S = 1 --> bat
                            // VVVVVVV --> van nao duoc bat/tat, dac biet, neu VVVVVVV = 0b1111111 thi tat ca ca van duoc bat/tat
                            //------------------------------------------------------------------------------------
                            data = APLGet();// lay lenh bat/tat bom tuoi
                            ConsolePut(data);//dua lenh nay xuong mach dieu khien bom tuoi
                            #if defined(USE_USART) && defined(USE_DEBUG)
                                printf("Just send cmd\r\n");
                            #endif
                        }
                            break;
                        #endif

                        #if defined(USE_SHT10)
                        case HTE_DATA_REQUEST_CLUSTER://gui du lieu nhiet do do am ma emboard yeu cau
                        {
                            //ban tin yeu cau lay du lieu nhiet do - do am tu bo nhung co dang: 0x00
                            data = APLGet();
                            LoadSHT10();//lay du lieu nhiet do - do am
                            Send_HTE_ToRouterEmboard();
                            #if defined(USE_USART) && defined(USE_DEBUG)
                                printf("Just send data to Router_EMB\r\n");
                            #endif
                        }
                            break;
                        #endif

                        default:
                            /* Catch all place for all none ZDO msgs not processed above */
                            #if defined(USE_USART)
                                printf("Got message...\r\n");
                            #endif
                            break;
                    }

                    if (currentPrimitive != APSDE_DATA_request)
                        TxData = TX_DATA_START;//reset lai chi so TxData
                }
                    break;

                // Other Endpoint here
                default:
                    break;
            }
            APLDiscardRx();
        }
            break;

        case APSDE_DATA_confirm:
            if (params.APSDE_DATA_confirm.Status)
            {
                #if defined(USE_USART)
                    PrintChar(params.APSDE_DATA_confirm.Status);
                    printf("Error sending message\r\n");
                #endif
            }
            else
            {
                #if defined(USE_USART)
                    printf("Send OK\r\n");
                #endif
            }
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSME_ADD_GROUP_confirm:
        case APSME_REMOVE_GROUP_confirm:
        case APSME_REMOVE_ALL_GROUPS_confirm:
            #if defined(USE_USART)
                printf("Perform Group Operation\r\n");
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case NO_PRIMITIVE:
            if (AllowJoin && !ZigBeeStatus.flags.bits.bNetworkJoined)//neu chua gia nhap mang @dat_a3cbq91
            {
                if (!ZigBeeStatus.flags.bits.bTryingToJoinNetwork)//neu chua tung thu gia nhap mang @dat_a3cbq91
                {
                    if (ZigBeeStatus.flags.bits.bTryOrphanJoin)//neu muon gia nhap nhu mot thanh vien cu @dat_a3cbq91
                    {
                        #if defined(USE_USART)
                            printf("Trying to join network as an orphan\r\n");
                        #endif
                        
                            params.NLME_JOIN_request.ScanDuration = 8;
                            params.NLME_JOIN_request.ScanChannels.Val = ALLOWED_CHANNELS;
                            params.NLME_JOIN_request.JoinAsRouter = TRUE;
                            params.NLME_JOIN_request.RejoinNetwork = TRUE;
                            params.NLME_JOIN_request.PowerSource = NOT_MAINS_POWERED;
                            params.NLME_JOIN_request.RxOnWhenIdle = TRUE;
                            params.NLME_JOIN_request.MACSecurity = FALSE;

                            params.NLME_JOIN_request.ExtendedPANID = currentNetworkDescriptor->ExtendedPANID;
                            currentPrimitive = NLME_JOIN_request;                        
                    }
                    else
                    {
                        #if defined(USE_USART)
                            printf("Trying to join network as a new device\r\n" );
                        #endif
                        params.NLME_NETWORK_DISCOVERY_request.ScanDuration = 8;
                        params.NLME_NETWORK_DISCOVERY_request.ScanChannels.Val = ALLOWED_CHANNELS;
                        currentPrimitive = NLME_NETWORK_DISCOVERY_request;
                    }
                }
            }
            else
            {
                if(!ZigBeeStatus.flags.bits.bHasBackgroundTasks)
                {
                    #if defined(USE_MQ6)
                        //gui canh bao co khoi ve router-emboard
                        if(ZigBeeReady() & WSANFlags.bits.MQ6Warning)
                        {
                            WSANFlags.bits.MQ6Warning = CLEAR;
                            SendOneByte(HasSmoke,NOTICE_STATE_NODE_CLUSTER);//thong bao co khoi ve router-emboard
                        }
                    #endif
                    #if defined(ENERGY_TRACKING)
                        //gui canh bao sap het nang luong ve router-emboard
                        if(ZigBeeReady() & WSANFlags.bits.LowPowerWarning)
                        {
                            WSANFlags.bits.LowPowerWarning = CLEAR;
                            SendOneByte(LowPower,NOTICE_STATE_NODE_CLUSTER);
                        }
                    #endif
                    #if defined(USE_SHT10)
                        //gui thong tin nhiet do-do am dinh ky ve router-emboard
                        if(ZigBeeReady() && WSANFlags.bits.HTE_Data_Ready)
                        {
                            WSANFlags.bits.HTE_Data_Ready = CLEAR;
                            Send_HTE_ToRouterEmboard();
                            #if defined(USE_LED)
                                LATBbits.LATB7 = ~LATBbits.LATB7;
                            #endif
                        }
                    #endif

                    #if defined(USE_CONTROL_PUMP)
                        if(ZigBeeReady() && WSANFlags.bits.PumpAckReceive)
                        {
                            WSANFlags.bits.PumpAckReceive = CLEAR;
                            SendOneByte(CmdPump,ACTOR_RESPONSE_CLUSTER);//bao ve Gateway da ra lenh dieu khien bat/tat van tuoi

                            #if defined(USE_USART) && defined(USE_DEBUG)
                                printf("Confirm to Gateway\r\n");
                            #endif
                        }
                    #endif
                    #if defined (USE_MicroWaveS)
                        //gui canh bao phat hien doi tuong xam nhap vao khu vuc cam ve router-emboard
                        if(ZigBeeReady() & WSANFlags.bits.MicrowaveDetecting & WSANFlags.bits.EnableSendMicrowave)
                        {
                            WSANFlags.bits.MicrowaveDetecting = CLEAR;
                            WSANFlags.bits.EnableSendMicrowave = CLEAR;
                            SendOneByte(StrangeObject,NOTICE_STATE_NODE_CLUSTER);
                        }
                    #endif
                }
                APLDiscardRx();
            }
            break;

        default:
            #if defined(USE_USART)
            	PrintChar( currentPrimitive );
                printf("Unhandled primitive\r\n" );
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;
    }
}

/* Prototype: void ProcessNONZigBeeTasks(void)
 * Input: None
 * Output: None
 * Discribe: Kich hoat cac flag duoc dinh nghia trong WSANFlags.
 */
void ProcessNONZigBeeTasks(void)
{
    #if defined(USE_SHT10)
        //Lay du lieu nhiet do-do am
        if(WSANFlags.bits.EnableGetDataHTE)
        {
            WSANFlags.bits.EnableGetDataHTE = CLEAR;
            LoadSHT10();

            #if defined(USE_USART) && defined(USE_DEBUG)
                printf("Temperature (hex): ");
                PrintWord(temperature);

                printf(". Humidity (hex): ");
                PrintWord(humidity);
                printf("\r\n");
            #endif

            WSANFlags.bits.HTE_Data_Ready = SET;//cho phep gui du lieu dinh ki ve gateway
        }

    #endif

    #if defined(ENERGY_TRACKING) && defined(USE_MQ6)
    if (WSANFlags.bits.MQ6orVoltage == Finish_Convert)
    {
        #if defined(USE_USART) && defined(USE_DEBUG)
            /* Hien thi ket qua tu cam bien khoi */
            printf("MQ6 signal (hex): ");
            PrintWord(Mq6Signal);

            /* Hien thi ket qua do dien ap nguon */
            printf(". Voltage (hex): ");
            PrintWord(Energy_Level);
            printf("\r\n");
        #endif

        if (Mq6Signal < ThresholdMQ6)
        {
            WSANFlags.bits.MQ6Warning = SET;//kich hoat viec gui tin hieu ve router emboard
        }

        if (Energy_Level < ThresholdPower)
        {
            WSANFlags.bits.LowPowerWarning = SET;//kich hoat viec gui tin hieu ve router emboard
        }
        WSANFlags.bits.MQ6orVoltage = MQ6_Mode_ADC;
    }
    #endif

    #if defined(ENERGY_TRACKING) && !defined(USE_MQ6)
    if(WSANFlags.bits.CompleteADC)
    {
        #if defined(USE_USART) && defined(USE_DEBUG)
            printf("Voltage (hex): ");
            PrintWord(Energy_Level);
            printf("\r\n");
        #endif
        if (Energy_Level > ThresholdPower)
        {
            WSANFlags.bits.LowPowerWarning = SET;
        }
        WSANFlags.bits.CompleteADC = CLEAR;
    }
    #endif

    #if !defined(ENERGY_TRACKING) && defined(USE_MQ6)
    if(WSANFlags.bits.CompleteADC)
    {
        #if defined(USE_USART) && defined(USE_DEBUG)
            printf("MQ6 signal (hex): ");
            PrintWord(Mq6Signal);
            printf("\r\n");
        #endif
        if (Mq6Signal<ThresholdMQ6)
        {
            WSANFlags.bits.MQ6Warning = SET;
        }
        WSANFlags.bits.CompleteADC = CLEAR;
    }
    #endif
}
/*******************************************************************************
HardwareInit
All port directioning and SPI must be initialized before calling ZigBeeInit().
 *******************************************************************************/
void HardwareInit(void)
{
    #ifdef USE_EXTERNAL_NVM
        EEPROM_nCS      = 1;
        EEPROM_nCS_TRIS = 0;
    #endif

    #if defined(USE_EXTERNAL_NVM) && !defined(EE_AND_RF_SHARE_SPI)
        RF_SPIInit();
        EE_SPIInit();
    #else
        SPIInit();//PIR1bits.SSPIF = 1
    #endif

    #if (RF_CHIP == MRF24J40)
        // Start with MRF24J40 disabled and not selected
        PHY_CS = 1;
        PHY_RESETn = 1;

        // Set the directioning for the MRF24J40 pin connections.
        PHY_CS_TRIS = 0;
        PHY_RESETn_TRIS = 0;

        // Initialize the interrupt.
        INTCON2bits.INTEDG0 = 0;
    #else
        #error Unknown transceiver selected
    #endif

    #if defined(USE_EXTERNAL_NVM) && !defined(EE_AND_RF_SHARE_SPI)
        // Initialize the SPI1 pins and directions
        LATCbits.LATC3      = 0;    // SCK
        LATCbits.LATC5      = 1;    // SDO
        TRISCbits.TRISC3    = 0;    // SCK
        TRISCbits.TRISC4    = 1;    // SDI
        TRISCbits.TRISC5    = 0;    // SDO

        // Initialize the SPI2 pins and directions
        LATDbits.LATD6      = 0;    // SCK
        LATDbits.LATD4      = 1;    // SDO
        TRISDbits.TRISD6    = 0;    // SCK
        TRISDbits.TRISD5    = 1;    // SDI
        TRISDbits.TRISD4    = 0;    // SDO

        RF_SSPSTAT_REG = 0x40;
        RF_SSPCON1_REG = 0x21;
        EE_SSPSTAT_REG = 0x40;
        EE_SSPCON1_REG = 0x21;
    #else
        // Initialize the SPI pins and directions
        LATCbits.LATC3      = 0; // SCK
        LATCbits.LATC5      = 1; // SDO
        TRISCbits.TRISC3    = 0; // SCK
        TRISCbits.TRISC4    = 1; // SDI
        TRISCbits.TRISC5    = 0; // SDO

        SSPSTAT_REG = 0x40; //Input data sampled at middle of data output time
                            //Output data changes on clock transition from active to idle
        #if (CLOCK_FREQ <= 16000000)
            SSPCON1_REG = 0x20;// Enables serial port and configures SCK, SDO, SDI and SS as serial port pins
                               // SPI Master mode, clock = FOSC/4
        #else
            SSPCON1_REG = 0x21;
        #endif

    #endif

    //-------------------------------------------------------------------------
    // This section is required for application-specific hardware
    // initialization.
    //-------------------------------------------------------------------------
    #if defined(ENERGY_TRACKING) || defined(USE_MQ6)
        // ----------------- Init ADC -----------------
        /* Configure port - Set PORT RA0 is analog input */
	TRISAbits.TRISA0 = SET;
	ANSEL = 0x01;
	ANSELH = 0x00;

        /* Configure ADC module */
	ADCON2 = 0xA1;	//ADCS<2:0> = 001 => Conversion clock = Fosc/8
                        //Right justified
                        //ACQT<2:0> = 010 => Acquisition Time = 8 Tad

	/* Configure voltage reference */
	#if defined(USE_MQ6)
            ADCON1 = Measure_MQ6;//dien ap tham chieu lay tu dien ap ra cua cam bien khoi. tin hieu cua cam bien khoi duoc do truoc.
        #else
            ADCON1 = Measure_Voltage;//do dien ap nguon bang cach su dung dien ap tham chieu lay tu dien ap nguon
        #endif

	//Select ADC input channel
	ADCON0 = 0x01;	//CHS<3:0> = 0000 => select channel AN0
                        //enable ADC module

        //Configure ADC interrupt (optional)
	//Clear ADC interrupt flag
	PIR1bits.ADIF = CLEAR;

	//Enable ADC interrupt
	PIE1bits.ADIE = Enable;
    #endif

    // ----------------- Init SHT -----------------
    #if defined(USE_SHT10)
        // Make RA1, RA2 outputs.
        TRISAbits.TRISA1 = CLEAR;
        TRISAbits.TRISA2 = CLEAR;
    #endif

    // ---------- Init RX interrupt UART ----------
    #if defined(USE_MicroWaveS)
        IPR1bits.RCIP = Enable;
        PIR1bits.RCIF = CLEAR;
        PIE1bits.RCIE = Enable;
    #endif
    // ---------- Init PIR ------------
    #if defined(USE_PIR)
        INTCON3bits.INT1IE=1;
        INTCON3bits.INT1IF=0;

        INTCON2bits.INTEDG1=1;
        TRISBbits.TRISB1 = 1;
    #endif
    #if defined (USE_LED)
        TRISBbits.TRISB6 = 0;
        LATBbits.LATB6 = 0;
        TRISBbits.TRISB7 = 0;
        LATBbits.LATB7 = 1;
    #endif
}

/*******************************************************************************
User Interrupt Handler

The stack uses some interrupts for its internal processing. Once it is done
checking for its interrupts, the stack calls this function to allow for any
additional interrupt processing.
 *******************************************************************************/
void UserInterruptHandler(void)
{
    if(INTCONbits.TMR0IF & INTCONbits.TMR0IE)
    {
        /* there was a timer overflow */
        INTCONbits.TMR0IF = 0;
        timerExtension1++;

        #if defined(USE_MicroWaveS)
            /* Chi cho phep gui canh bao co doi tuong xam nhap vao cac thoi diem le (ie: 1,3,5,...) */
            if(timerExtension1 & 0x01)
                WSANFlags.bits.EnableSendMicrowave = 1;
            else
                WSANFlags.bits.EnableSendMicrowave = 0;
        #endif
        if(timerExtension1 == 0)
        {
            timerExtension2++;

            /* Every 268 seconds, do the following*/

            #if defined(USE_SHT10)
                WSANFlags.bits.EnableGetDataHTE = SET;//cho phep in du lieu nhiet do-do am
            #endif
            #if defined(ENERGY_TRACKING) || defined(USE_MQ6)
                ADCON0bits.GO = ON;//bat dau lay mau tin hieu tu cam bien khoi
            #endif
        }
    }

    #if defined(ENERGY_TRACKING) || defined(USE_MQ6)
        if(PIR1bits.ADIF)
        {
            PIR1bits.ADIF = CLEAR;
            //Lay gia tri chuyen doi ADC
            #if defined(USE_MQ6) && defined(ENERGY_TRACKING)
                //lay ket qua chuyen doi ADC
                ADC_result = ADRESH;
                ADC_result = (ADC_result<<8)|ADRESL;
                switch(WSANFlags.bits.MQ6orVoltage)
                {
                    case MQ6_Mode_ADC:
                        Mq6Signal = ADC_result;
                        ADCON1 = Measure_Voltage;//chuyen sang do dien ap cua nguon
                        break;

                    case Power_Mode_ADC:
                        Energy_Level = ADC_result << 2;
                        ADCON1 = Measure_MQ6;//chuyen sang do tin hieu tu MQ6
                        break;

                    default:
                        break;
                }
                ++(WSANFlags.bits.MQ6orVoltage);
            #endif

            #if defined(USE_MQ6) && !defined(ENERGY_TRACKING)
                Mq6Signal = ADRESH;
                Mq6Signal = (Mq6Signal<<8)|ADRESL;
                WSANFlags.bits.CompleteADC = SET;
            #endif

            #if defined(ENERGY_TRACKING) && !defined(USE_MQ6)
                Energy_Level = ADRESH;
                Energy_Level = (Energy_Level<<8)|ADRESL;
                Energy_Level = Energy_Level << 2;//x4 = ADC 12 bit
                WSANFlags.bits.CompleteADC = SET;
            #endif
        }
    #endif

    #if defined(USE_MicroWaveS)
        if (PIR1bits.RCIF)
        {
            PIR1bits.RCIF = CLEAR;
            if (RCSTAbits.OERR)
            {
                RCSTAbits.CREN = 0; // Disable UART receiver
                RCSTAbits.CREN = 1; // Enable UART receiver
            }
            if(RCREG == 'W')
                WSANFlags.bits.MicrowaveDetecting = SET;
        }
    #endif
    
    #if defined(USE_PIR)
        if(INTCON3bits.INT1IF)
        {
            #if defined(USE_USART)
                printf("co su xam nhap");
            #endif
            INTCON3bits.INT1IF=0;
        }
    #endif

    #if defined(USE_CONTROL_PUMP)
        if (PIR1bits.RCIF)
        {
            PIR1bits.RCIF = CLEAR;
            CmdPump = RCREG;
            WSANFlags.bits.PumpAckReceive = SET;
        }

    #endif
}
/*********************************************************************
 * Function:        void SendOneByte(BYTE data, WORD ClusterID)
 *
 * PreCondition:    Init OK
 *
 * Input:           state
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Overview:        Gui mot thong tin ve router-EMB. Thong tin nay co the la cac
 *                  trang thai chay rung, canh bao muc nang luong hoac la thong
 *                  xac nhan co mot node mang nao do vua gia nhap mang
 *
 * Note:
 ********************************************************************/
void SendOneByte(BYTE data, WORD ClusterID)
{
    //cau truc ban tin: SS
    TxBuffer[TxData++] = data;//SS
    ZigBeeBlockTx();

    /* load parameters for APSDE_DATA_request primitive */
    params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;

    /* load network address of router-emboard */
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = RouterEmboardAddrMSB;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = RouterEmboardAddrLSB;

    params.APSDE_DATA_request.SrcEndpoint = WSAN_Src;
    params.APSDE_DATA_request.DstEndpoint = WSAN_Dst;
    params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;

    //params.APSDE_DATA_request.asduLength; TxData
    params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;

//    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_FORCE;
//    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_SUPPRESS;
    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;

    #ifdef I_SUPPORT_SECURITY
        params.APSDE_DATA_request.TxOptions.Val = Enable;
    #else
        params.APSDE_DATA_request.TxOptions.Val = Disable;
    #endif
    params.APSDE_DATA_request.TxOptions.bits.acknowledged = Enable;
    params.APSDE_DATA_request.ClusterId.Val = ClusterID;

    currentPrimitive = APSDE_DATA_request;
}
#if defined(USE_SHT10)
/*********************************************************************
 * Function:        void Send_HTE_ToRouterEmboard()
 *
 * PreCondition:
 *
 * Input:           clusterID
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Overview:        Gui du lieu nhiet do-do am-nang luong (HTE) ve
 *                  router-EMB.
 *
 * Note:
 ********************************************************************/
void Send_HTE_ToRouterEmboard()
{
    //nap du lieu nhiet do - do am - nang luong chua qua xu ly vao buffer TX
    TxBuffer[TxData++] = (BYTE) (temperature >> 8);
    TxBuffer[TxData++] = (BYTE) (temperature);
    TxBuffer[TxData++] = (BYTE) (humidity >> 8);
    TxBuffer[TxData++] = (BYTE) (humidity);
    #if defined(ENERGY_TRACKING)
        TxBuffer[TxData++] = (BYTE) (Energy_Level >> 8);
        TxBuffer[TxData++] = (BYTE) (Energy_Level);
    #else
        TxBuffer[TxData++] = 0x00;
        TxBuffer[TxData++] = 0x00;
    #endif

    ZigBeeBlockTx();
    params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = RouterEmboardAddrMSB;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = RouterEmboardAddrLSB;

    params.APSDE_DATA_request.SrcEndpoint = WSAN_Src;
    params.APSDE_DATA_request.DstEndpoint = WSAN_Dst;
    params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;//MY_PROFILE_ID = 0x0103

    //params.APSDE_DATA_request.asduLength; TxData
    params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;//gioi han so hop ma du lieu duoc phep truyen qua, o day la 10 hop

    //params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_FORCE;
    //params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_SUPPRESS;
    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;

    #ifdef I_SUPPORT_SECURITY
        params.APSDE_DATA_request.TxOptions.Val = Enable;
    #else
        params.APSDE_DATA_request.TxOptions.Val = Disable;//khong ho tro bao mat
    #endif
    params.APSDE_DATA_request.TxOptions.bits.acknowledged = Enable;// Yeu cau ACK tu thiet bi thu
    params.APSDE_DATA_request.ClusterId.Val = HTE_DATA_RESPONSE_CLUSTER;

    currentPrimitive = APSDE_DATA_request;
}

void LoadSHT10()
{
    BYTE error, checksum;

    error = 0;
    error += s_measure((BYTE*) &humidity, &checksum, HUMI); //measure humidity
    error += s_measure((BYTE*) &temperature, &checksum, TEMP); //measure temperature
    while (error != 0)
    {
        s_connectionreset(); //in case of an error: connection reset
        error += s_measure((BYTE*) &humidity, &checksum, HUMI); //measure humidity
        error += s_measure((BYTE*) &temperature, &checksum, TEMP); //measure temperature
    }
}
#endif
