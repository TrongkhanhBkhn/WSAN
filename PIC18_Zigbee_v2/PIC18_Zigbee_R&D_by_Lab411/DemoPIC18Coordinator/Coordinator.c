/*******************************************************************************
 * Microchip ZigBee2006 Residential Stack
 *
 * Zigbee Coordinator
 *
 * Day la ma nguon de cau hinh cho mot node tro thanh Coodinator theo chuan giao
 * thuc Zibee. Ma nguon chay tren phan cung duoc thiet ke boi nhom WSAN - lab411.
 * Trong ma nguon cua nhom, co su dung kien truc Microchip Stack de xay dung cac
 * ung dung theo chuan giao tiep khong day Zigbee. De hieu duoc hoat dong cua he
 * thong, hay doc tai lieu WSAN Specification
 *
 *******************************************************************************
 * FileName:        Coordinator.c
 * Project:         DemoPIC18Coordinator
 * Version:         2.0
 *
 * Processor:       PIC18F26K20
 * Editor:          MPLAB X IDE v1.41
 * Complier:        MCC18 v3.20 or higher
 *
 * Company support: Microchip Technology, Inc.
 *
 * Developer:       Nguyen Tien Dat - KSTN - DTVT - K54
 * Group:           WSAN group - Lab411
 * Edition:         13/10/2012
 * 
 *******************************************************************************/

//******************************************************************************
// Header Files
//******************************************************************************

// Include the main ZigBee header file.
#include "zAPL.h"

#ifdef I_SUPPORT_SECURITY
    #include "zSecurity.h"
    extern NETWORK_KEY_INFO currentNetworkKeyInfo;
    #ifdef USE_EXTERNAL_NVM
        extern NETWORK_KEY_INFO plainSecurityKey[2];
        extern BOOL SetSecurityKey(INPUT BYTE index, INPUT NETWORK_KEY_INFO newSecurityKey);
    #endif
#endif

// If you are going to send data to a terminal, include this file.
#if defined(USE_USART) || defined(USE_CONTROL_PUMP)
    #include "Console.h"
#endif

// If you are going to get temperature and humidity, include this file.
#if defined(USE_SHT10)
    #include "delay.h"
    #include "SHT1x.h"
#endif

//******************************************************************************
// Configuration Bits					@modified by dat_a3cbq91
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
// Compilation Configuration
//******************************************************************************

//******************************************************************************
// Constants 		defined application service		@added by dat_a3cbq91
//******************************************************************************
#define PAN_Identify    0x1AAA
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
#define Security        0x04

// Cac gia tri nay dung de thiet lap cho thanh ghi ADCON1 dieu khien do tin hieu tu dau
#define Measure_MQ6     0x10
#define Measure_Voltage 0x00

// Gia tri nguong cua cac tin hieu tu cam bien khoi va nguon nang luong
// cho phep node gui tin hieu canh bao ve Router-EMB
#define ThresholdMQ6	900
#define ThresholdPower	10

// dia chi mang cua Router-EMB la 0x0009
#define NetworkAddrMSB_EMB	0x00
#define NetworkAddrLSB_EMB	0x09
//******************************************************************************
// Function Prototypes					@modified by dat_a3cbq91
//******************************************************************************

extern void RemoveAllGroups(void);
extern BYTE LookupAPSAddress(LONG_ADDR *);
extern BOOL APSSaveAPSAddress(APS_ADDRESS_MAP *AddressMap);

#if defined(I_SUPPORT_BINDINGS)
    extern void RemoveAllBindings(SHORT_ADDR);
#endif

// Functions created or modified by lab411
void HardwareInit(void);
void ProcessZigBeePrimitives(void);
void ProcessNONZigBeeTasks(void);

void SendOneByte(BYTE data, WORD ClusterID);

#if defined(USE_SHT10)
    void Send_HTE_ToRouterEmboard(void);
    void LoadSHT10(void);
#endif
// End by lab411

//******************************************************************************
// Application Variables								@modified by dat_a3cbq91
//******************************************************************************

// Variables created by lab411
#if defined(ENERGY_TRACKING)
    WORD Energy_Level;//Luu tru ket qua do dien ap nguon
#endif

#if defined(USE_MQ6)
    WORD Mq6Signal;//Luu tru ket qua do tin hieu tu cam bien khoi MQ6
#endif

#if defined(ENERGY_TRACKING) && defined(USE_MQ6)
    WORD ADC_result;//Luu tru gia tri lay mau tu ADC module
#endif

#if defined(USE_SHT10)
    WORD humidity, temperature;//Luu tru gia tri do am, nhiet do lay tu SHT chua qua xu ly @dat_a3cbq91
#endif

signed char i;

static union
{
    struct
    {
                BYTE MQ6Warning             : 1;
                BYTE LowPowerWarning        : 1;
                BYTE EnablePrintResult      : 1;
        #if defined(USE_MQ6)||defined(ENERGY_TRACKING)
            #if defined(USE_MQ6)&&defined(ENERGY_TRACKING)
                BYTE MQ6orVoltage           : 2;//LSB
                BYTE Reserve                : 3;
            #else
                BYTE CompleteADC            : 1;
                BYTE Reserve                : 4;
            #endif
        #else
                BYTE Reserve                : 5;
        #endif
                
    } bits;
    BYTE Val;
} WSANFlags;
#define STATUS_FLAGS_INIT       0x00
// End by lab411

ZIGBEE_PRIMITIVE currentPrimitive = NO_PRIMITIVE;

SHORT_ADDR destinationAddress;
#ifdef I_SUPPORT_SECURITY
    extern KEY_VAL KeyVal;
    #ifdef USE_EXTERNAL_NVM
        extern WORD trustCenterLongAddr;
        extern NETWORK_KEY_INFO plainSecurityKey[2];
    #else
        extern ROM LONG_ADDR trustCenterLongAddr;
    #endif
#endif


//******************************************************************************
//******************************************************************************
//******************************************************************************
void main(void)
{
    CLRWDT();
    ENABLE_WDT();

    OSCCON = INTOSC_16MHz;

    //currentPrimitive = NO_PRIMITIVE;

    /* Initialize the UART such that data can be sent and recieved on terminal */
    #if defined(USE_USART) || defined(USE_CONTROL_PUMP)
	ConsoleInit();
    #endif

    /* Initialize the hardware before initializing the ZigBee Stack */
    HardwareInit();
    #if defined(USE_USART)
        printf("\r\nInit Hardware");
    #endif

    /* if using security force the use of key slot 0 in stack */
    #ifdef I_SUPPORT_SECURITY
        BYTE i;
        i = 0xff;
        PutNwkActiveKeyNumber(&i);
    #endif

    /* Initialize the ZigBee Stack */
    ZigBeeInit();//ham nay co khoi tao bo dem Timer0.
    #if defined(USE_USART)
        printf("\r\nInit Zigbee");
    #endif

    
    PHYSetShortRAMAddr(0x00, 0x0d);

    /* Refresh the tables anew each time the Node is booted up */
    #if defined(I_SUPPORT_BINDINGS)
        /* removes all the entries from the Neighbor Table that were stored in nonvolatile memory */
        NWKClearNeighborTable();
        /* removes all the entries from the binding table that were stored in nonvolatile memory */
        ClearBindingTable();
    #endif

    /* remove membership in all groups from an endpoint */
    RemoveAllGroups();

    #if defined(I_SUPPORT_BINDINGS)
        /* removes all the entries from the binding table from a particular source device */
        RemoveAllBindings(macPIB.macShortAddress);
    #endif

    // *************************************************************************
    // Perform any other initialization here
    // *************************************************************************
    #if defined(USE_USART)
        printf("\r\nR&D ZigBee-Coordinator by WSAN-Lab411");
        #if (RF_CHIP == MRF24J40)
            printf("\r\nTransceiver-MRF24J40");
        #else
            printf("\r\nTransceiver-Unknown" );
        #endif
    #endif

    /* Initialize my status flags*/
    WSANFlags.Val = STATUS_FLAGS_INIT;

    /* Enable interrupts to get the stack operational */
    RCONbits.IPEN = Enable; // Enable interrupt priority
    INTCONbits.GIEH = Enable; // Enables all high priority interrupts
    INTCONbits.GIEL = Enable; // Enables all low priority interrupts

    while (1)
    {
        /* Clear the watch dog timer to avoid reseting */
        CLRWDT();

        /* Process the current ZigBee Primitive */
        ZigBeeTasks(&currentPrimitive);

        /* Determine the next ZigBee Primitive */
        ProcessZigBeePrimitives();

        /* do any non ZigBee related tasks */
        ProcessNONZigBeeTasks();
    }
}

void ProcessZigBeePrimitives(void)
{
    switch (currentPrimitive)
    {
        //ZigBee protocol coordinator add a device as a child device.
	case NLME_DIRECT_JOIN_confirm:
            #if defined(USE_USART)
                if (params.NLME_DIRECT_JOIN_confirm.Status == NWK_TABLE_FULL)
                    printf("\r\nNeighbor table is full");
                else
                    printf("\r\nDirect join OK");
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        //ZigBee protocol coordinator start a network on one of the specified channels.
	case NLME_NETWORK_FORMATION_confirm:
            if (!params.NLME_NETWORK_FORMATION_confirm.Status)
            {
                #if defined(USE_USART)
                    printf("\r\nCreated PAN ");
                    PrintChar(macPIB.macPANId.byte.MSB);
                    PrintChar(macPIB.macPANId.byte.LSB);
                #endif

                /* nap cac gia tri cho primitive NLME_PERMIT_JOINING_request */
                params.NLME_PERMIT_JOINING_request.PermitDuration = 0xFF;//0xFF cho phep gia nhap mang, 0x00 khong cho phep gia nhap
                params.NLME_PERMIT_JOINING_request._updatePayload = TRUE;
                currentPrimitive = NLME_PERMIT_JOINING_request;// neu tao mang thanh cong, coordinator tiep tuc
                                                               // kich hoat stack cho phep cac node khac join vao mang
            }
            else
            {
                #if defined(USE_USART)
                    printf("\r\nError forming network:");
                    PrintChar(params.NLME_NETWORK_FORMATION_confirm.Status);
                    printf(". Trying again!");
                #endif
                currentPrimitive = NO_PRIMITIVE;// neu khong tao duoc mang, ZC gui lai yeu cau tao mang
            }
            break;

        //ZigBee protocol coordinator allow other nodes to join the network as our children.
	case NLME_PERMIT_JOINING_confirm:
            if (!params.NLME_PERMIT_JOINING_confirm.Status)
            {
                #if defined(USE_USART)
                    printf("\r\nAllow to join");
                #endif
                
                currentPrimitive = NO_PRIMITIVE;//khi viec join da thanh cong, tro ve trang thai binh thuong.

                #if defined(ENERGY_TRACKING) || defined(USE_MQ6)
                    ADCON0bits.GO = ON;//bat dau lay mau tin hieu tu cam bien khoi hoac do dien ap
                #endif
            }
            else
            {
                #if defined(USE_USART)
                    printf("\r\nJoin permission failed:");
                    PrintChar(params.NLME_PERMIT_JOINING_confirm.Status);
                #endif
                currentPrimitive = NLME_PERMIT_JOINING_request;// neu viec cho phep join chua duoc kich hoat thanh cong
                                                               // ZC se tai kich hoat stack de cho phep cac node khac join.
            }
            break;

        //ZigBee protocol Coordinator initiate route discovery to another device.
	case NLME_ROUTE_DISCOVERY_confirm:
            #if defined(USE_USART)
                if (!params.NLME_ROUTE_DISCOVERY_confirm.Status)
                    printf("\r\nRoute Reply OK");
                else
                {
                    printf("\r\nRoute Reply Failed:");
                    PrintChar(params.NLME_PERMIT_JOINING_confirm.Status);
                }
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        //This primitive allows the next higher layer of a ZigBee coordinator to
        //be notified when a new device has successfully joined its network by
        //association or rejoined using the NWK rejoin procedure
	case NLME_JOIN_indication:
            #if defined(USE_USART)
            {
                printf("\r\nNode ");
                /* In ra dia chi mang cua node vua join */
                PrintChar(params.NLME_JOIN_indication.ShortAddress.byte.MSB);
                PrintChar(params.NLME_JOIN_indication.ShortAddress.byte.LSB);
                /* In ra dia chi MAC  cua node vua join */
                printf(" with MAC Address ");
                for(i = 7; i >= 0; --i)
                    PrintChar(params.NLME_JOIN_indication.ExtendedAddress.v[i]);
                printf(" just joined");
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
                            {
                                KeyVal.v[i] = 0;
                            }
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
                            BYTE i;
                            for (i = 0; i < 16; i++)
                            {
                                KeyVal.v[i] = 0;
                            }
                            params.APSME_TRANSPORT_KEY_request.Key = &KeyVal;
                            params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = 0;
                            params.APSME_TRANSPORT_KEY_request._UseSecurity = TRUE;
                        #else
                            if (params.NLME_JOIN_indication.secureJoin)
                            {
                                BYTE i;
                                for (i = 0; i < 16; i++)
                                {
                                    KeyVal.v[i] = 0;
                                }
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
                                    GetNwkKeyInfo(&currentNetworkKeyInfo, (ROM void *) &(networkKeyInfo[i - 1]));
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

        //This primitive allows the next higher layer of ZigBee device to be notified if that
        //ZigBee device has left the network or if a neighboring device has left the network.
	case NLME_LEAVE_indication:
            {
                #if defined(USE_USART)
                    if (!memcmppgm2ram(&params.NLME_LEAVE_indication.DeviceAddress, (ROM void *) &macLongAddr, 8))//neu ZC roi khoi mang
                        printf("\r\nZC has left the network");
                    else//neu cac node khac roi khoi mang
                        printf("\r\nAnother node has left the network");
                #endif
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

	//This primitive allows the next higher layer of the initiating device
        //to be notified of the results of the request to reset the NWK layer
        case NLME_RESET_confirm:
            #if defined(USE_USART)
                printf("\r\nReset ZigBee Stack");
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

	//This primitive allows the next higher layer of the initiating device to be notified
        //of the results of its request for itself or another device to leave the network
        case NLME_LEAVE_confirm:
            #if defined(USE_USART)
                printf("\r\nDevice has left the network");
                PrintChar(params.NLME_LEAVE_confirm.Status);
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        //The primitive reports the results of a request to transfer
        //a data PDU (ASDU) from a local NHLE to one or more peer NHLEs
	case APSDE_DATA_confirm:
            #if defined(USE_USART)
                if (params.APSDE_DATA_confirm.Status)//neu viec gui ban tin di khong thanh cong
                {
                    printf("\r\nError sending message:");
                    PrintChar(params.APSDE_DATA_confirm.Status);
                }
                else
                    printf("\r\nSending message OK");
            #endif

            currentPrimitive = NO_PRIMITIVE;
            break;

        //This primitive indicates the transfer of a data PDU (ASDU)
        //from the APS sub-layer to the local application entity
	case APSDE_DATA_indication:
	{
            BYTE data;
            BYTE sequenceNumber = CLEAR;

            currentPrimitive = NO_PRIMITIVE;//next primitive is NO_PRIMITIVE
            switch (params.APSDE_DATA_indication.DstEndpoint)
            {
                /* Process anything sent to ZDO */
                case EP_ZDO:
                    #define dataLength 0
                    //sequenceNumber||data||
                    {
                        BYTE transByte = CLEAR;
                        sequenceNumber = APLGet();
                        switch (params.APSDE_DATA_indication.ClusterId.Val)
                        {
                            // ********************************************************
                            // Put a case here to handle each ZDO response that application requested.
                            // ********************************************************
                            case BIND_rsp:
                            case UNBIND_rsp:
                                data = APLGet();
                                #if defined(USE_USART)
                                    if (data == SUCCESS)
                                    {
                                        printf("\r\nBinding/Unbinding OK");
                                    }
                                    else
                                    {
                                        PrintChar(data);
                                        printf("\r\nBinding/Unbinding Failed");
                                    }
                                #endif
                                break;


                            case NWK_ADDR_rsp:
                                if (APLGet() == SUCCESS)
                                {
                                    #if defined(USE_USART)
                                        /* update our table when device recieves the address it requested */
                                        printf("\r\nReceiving NWK_ADDR_rsp");
                                    #endif

                                    // Skip over the IEEE address of the responder.
                                    for (data = 0; data < 8; data++)
                                    {
                                        currentAPSAddress.longAddr.v[data] = APLGet();
                                        transByte++;
                                    }
                                    currentAPSAddress.shortAddr.v[0] = destinationAddress.byte.LSB = APLGet();
                                    currentAPSAddress.shortAddr.v[1] = destinationAddress.byte.MSB = APLGet();
                                    transByte += 2;
                                    APSSaveAPSAddress(&currentAPSAddress);//luu lai dia chi hien tai cua lop APS
                                }
                                break;

                                /* Process any further ZDO responses here */
                            default:
                                break;
                        }

                        // Read out the rest of the MSG in case there is another transaction.
                        for (; transByte < dataLength; transByte++)
                            APLGet();
                    }
                    #undef dataLength
                    break;

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
                                printf("\r\nJust send cmd");
                            #endif
                            
                            SendOneByte(data,ACTOR_RESPONSE_CLUSTER);//bao ve Gateway da ra lenh dieu khien bat/tat van tuoi

                            #if defined(USE_USART) && defined(USE_DEBUG)
                                printf("\r\nConfirm to Gateway");
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
                                printf("\r\nJust send data to Router_EMB");
                            #endif
                        }
                            break;
                        #endif

                        default:
                            /* Catch all place for all none ZDO msgs not processed above */
                            #if defined(USE_USART)
                                printf("\r\nGot message...");
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

	case APSME_ADD_GROUP_confirm:
	case APSME_REMOVE_GROUP_confirm:
	case APSME_REMOVE_ALL_GROUPS_confirm:
            currentPrimitive = NO_PRIMITIVE;
            break;

        /* if nothing to process first check to see if we are in startup seqence */
	case NO_PRIMITIVE:
            if (!ZigBeeStatus.flags.bits.bNetworkFormed)
            {
		if (!ZigBeeStatus.flags.bits.bTryingToFormNetwork)
                {
                    #if defined(USE_USART)
                        printf("\r\nTrying to start network...");
                    #endif
                    /* Nap gia tri cho cac tham so cua primitive NLME_NETWORK_FORMATION_request */
                    params.NLME_NETWORK_FORMATION_request.ScanDuration = 8;
                    params.NLME_NETWORK_FORMATION_request.ScanChannels.Val = ALLOWED_CHANNELS;
                    params.NLME_NETWORK_FORMATION_request.PANId.Val = PAN_Identify;
                    params.NLME_NETWORK_FORMATION_request.BeaconOrder = MAC_PIB_macBeaconOrder;
                    params.NLME_NETWORK_FORMATION_request.SuperframeOrder = MAC_PIB_macSuperframeOrder;
                    params.NLME_NETWORK_FORMATION_request.BatteryLifeExtension = MAC_PIB_macBattLifeExt;
                    currentPrimitive = NLME_NETWORK_FORMATION_request;//ZC gui yeu cau khoi tao mang Zigbee
                }
            }
            else
            {
                if(!ZigBeeStatus.flags.bits.bHasBackgroundTasks)
                {
                    #if defined(USE_MQ6)
                        //gui thong tin co khoi ve router-emboard
                        if(ZigBeeReady() && WSANFlags.bits.MQ6Warning)
                        {
                            WSANFlags.bits.MQ6Warning = CLEAR;
                            SendOneByte(HasSmoke,NOTICE_STATE_NODE_CLUSTER);//thong bao co khoi ve router-emboard
                        }
                    #endif

                    #if defined(ENERGY_TRACKING)
                        //gui thong tin sap het nang luong ve router-emboard
                        if(ZigBeeReady() && WSANFlags.bits.LowPowerWarning)
                        {
                            WSANFlags.bits.LowPowerWarning = CLEAR;
                            SendOneByte(LowPower,NOTICE_STATE_NODE_CLUSTER);
                        }
                    #endif
                }
                APLDiscardRx();
            }
            break;

        default:
            #if defined(USE_USART)
                PrintChar(currentPrimitive);
                printf("\r\nUnhandled primitive");
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
    #if defined(USE_SHT10) && defined(USE_USART) && defined(USE_DEBUG)
        //Lay du lieu nhiet do-do am
        if(WSANFlags.bits.EnablePrintResult)
        {
            WSANFlags.bits.EnablePrintResult = CLEAR;

            //chu y: neu muon doi ra do C --> xem muc 4.3 datasheet SHT1x
            //xu ly lay du lieu do am tuong doi --> xem muc 4.1 & 4.2 datasheet SHT1x
            
            printf("\r\nTemperature: ");
            ConsolePut((temperature / 1000) + '0');
            ConsolePut((temperature % 1000)/100 + '0');
            ConsolePut((temperature % 100)/10 + '0');
            ConsolePut((temperature % 10) + '0');

            printf("\r\nHumidity: ");
            ConsolePut((humidity / 1000) + '0');
            ConsolePut((humidity % 1000)/100 + '0');
            ConsolePut((humidity % 100)/10 + '0');
            ConsolePut((humidity % 10) +  '0');
        }

    #endif

    #if defined(ENERGY_TRACKING) && defined(USE_MQ6)
    if (WSANFlags.bits.MQ6orVoltage == Finish_Convert)
    {
        #if defined(USE_USART) && defined(USE_DEBUG)
            /* Hien thi ket qua tu cam bien khoi */
            printf("\r\nMQ6 signal: ");
            ConsolePut((Mq6Signal / 1000) + '0');
            ConsolePut((Mq6Signal % 1000)/100 + '0');
            ConsolePut((Mq6Signal % 100)/10 + '0');
            ConsolePut((Mq6Signal % 10) + '0');

            /* Hien thi ket qua do dien ap nguon */
            printf("\r\nVoltage: ");
            ConsolePut((Energy_Level / 100) + '0');
            ConsolePut((Energy_Level % 100)/10 + '0');
            ConsolePut((Energy_Level % 10)  + '0');
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
            printf("\r\nVoltage: ");
            ConsolePut((Energy_Level /100) + '0');
            ConsolePut((Energy_Level % 100) /10 + '0');
            ConsolePut(Energy_Level % 10  + '0');
        #endif
        if (Energy_Level < ThresholdPower)
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
            printf("\r\nMQ6 signal:");
            ConsolePut((Mq6Signal / 1000) + '0');
            ConsolePut((Mq6Signal % 1000)/100 + '0');
            ConsolePut((Mq6Signal % 100) /10 + '0');
            ConsolePut(Mq6Signal % 10  + '0');
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

        if(timerExtension1 == 0)
        {
            timerExtension2++;

            /* Every 268 seconds, do the following*/

            #if defined(USE_SHT10) && defined(USE_DEBUG)
            	LoadSHT10();
                WSANFlags.bits.EnablePrintResult = SET;//cho phep in du lieu nhiet do-do am
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
                        Energy_Level = 400 - ADC_result;
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
                Energy_Level = 400 - Energy_Level;
                WSANFlags.bits.CompleteADC = SET;
            #endif
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
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = NetworkAddrMSB_EMB;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = NetworkAddrLSB_EMB;

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
    #if defined(USE_SHT10)
        //nap du lieu nhiet do - do am - nang luong chua qua xu ly vao buffer TX
        TxBuffer[TxData++] = (BYTE) (temperature >> 8);
        TxBuffer[TxData++] = (BYTE) (temperature);
        TxBuffer[TxData++] = (BYTE) (humidity >> 8);
        TxBuffer[TxData++] = (BYTE) (humidity);
        #if defined(ENERGY_TRACKING)
            TxBuffer[TxData++] = (BYTE) (Energy_Level);
        #else
            TxBuffer[TxData++] = 0x00;
        #endif
    #else
        TxBuffer[TxData++] = 0x00;
        TxBuffer[TxData++] = 0x00;
        TxBuffer[TxData++] = 0x00;
        TxBuffer[TxData++] = 0x00;
        #if defined(ENERGY_TRACKING)
            TxBuffer[TxData++] = (BYTE) (Energy_Level);
        #else
            TxBuffer[TxData++] = 0x00;
        #endif
    #endif

    ZigBeeBlockTx();
    params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = NetworkAddrMSB_EMB;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = NetworkAddrLSB_EMB;

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
