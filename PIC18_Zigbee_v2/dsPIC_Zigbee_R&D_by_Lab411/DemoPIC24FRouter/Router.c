/*******************************************************************************
 * Microchip ZigBee2006 Residential Stack
 *
 * Zigbee Router
 *
 * Day la ma nguon de cau hinh cho mot node tro thanh Router-EMB theo chuan giao
 * thuc Zigbee. Ma nguon chay tren phan cung duoc thiet ke boi nhom WSAN - lab411.
 * Trong ma nguon cua nhom, co su dung kien truc Microchip Stack de xay dung cac
 * ung dung theo chuan giao tiep khong day Zigbee. De hieu duoc hoat dong cua he
 * thong, hay doc tai lieu WSAN Specification
 *
 * Router-EMB nay duoc su dung trong khu vuc canh bao chay rung va cham soc lan
 *
 *******************************************************************************
 * FileName:        Router.c
 * Project:         DemoPIC24Router
 * Version:         2.0
 *
 * Controller:      dsPIC33FJ128MC804
 * Editor:          MPLAB X IDE v1.41
 * Complier:        C30 v3.30 or higher
 *
 * Company support: Microchip Technology, Inc.
 *
 * Developer:       Nguyen Tien Dat - KSTN - DTVT - K54
 * Group:           WSAN group - Lab411
 * Edition:         09/03/2013
 *
 *******************************************************************************/

//******************************************************************************
// Header Files
//******************************************************************************

// Include the main ZigBee header file.
#include "zAPL.h"

#ifdef I_SUPPORT_SECURITY
    #include "zSecurity.h"
#endif

// If you are going to send data to a emboard, include this file.
#include "console.h"

//******************************************************************************
// Configuration Bits
//******************************************************************************

//Primary oscillator
_FOSCSEL(FNOSC_PRI & IESO_ON);

//Clock switching is disabled, Fail-Safe Clock Monitor is disabled
//OSC2 pin has clock out function
//XT Oscillator Mode
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_XT);

//Watchdog timer enabled/disabled by user software
_FWDT(FWDTEN_OFF & WDTPRE_PR128 & WINDIS_OFF & WDTPOST_PS512);
	
//******************************************************************************
// Constants			//defined application service	@dat_a3cbq91
//******************************************************************************
#define ON              1
#define SET             1
#define ENABLE          1
#define OFF             0
#define CLEAR           0
#define DISABLE         0

#if defined(USE_LED)
    #define LED0_TRIS TRISAbits.TRISA10
    #define LED1_TRIS TRISAbits.TRISA7
    #define LED2_TRIS TRISAbits.TRISA9
    #define LED3_TRIS TRISAbits.TRISA8

    #define LED0 LATAbits.LATA10
    #define LED1 LATAbits.LATA7
    #define LED2 LATAbits.LATA9
    #define LED3 LATAbits.LATA8


    #define BLINK_LED_JOIN              4000
    #define BLINK_LED_NOT_JOIN          300
#endif

//******************************************************************************
// Function Prototypes
//******************************************************************************
// Function is modified by WSAN - lab411
void ProcessZigBeePrimitives(void);
// End by WSAN - lab411

// Function create by WSAN - lab411
#if defined(USE_COMPUTER)
BYTE GetHexByte(BYTE MSB, BYTE LSB);
#else
void HardwareInit(void);
void ProcessNONZigBeeTasks(void);
void SendOneByte(WORD ClusterID, BYTE MSB, BYTE LSB, BYTE cmd);
void SendRouteDrawRequest(BYTE MSB_dest_addr, BYTE LSB_dest_addr);
#endif

// End by WSAN - Lab411

//******************************************************************************
// Application Variables
//******************************************************************************
NETWORK_DESCRIPTOR  *NetworkDescriptor;
NETWORK_DESCRIPTOR  *currentNetworkDescriptor;
ZIGBEE_PRIMITIVE    currentPrimitive;
BYTE                orphanTries;
#ifdef I_SUPPORT_SECURITY
    extern KEY_VAL	KeyVal;
    #ifdef USE_EXTERNAL_NVM
        extern WORD trustCenterLongAddr;
        extern NETWORK_KEY_INFO plainSecurityKey[2];
    #else
        extern ROM LONG_ADDR trustCenterLongAddr;
    #endif
#endif

BYTE AllowJoin = ENABLE;
extern NWK_STATUS nwkStatus;

// Variable declared by WSAN - Lab411
BYTE    i;
volatile BYTE counter = CLEAR, EnableSendCmd = DISABLE;
BYTE Addr_MSB, Addr_LSB, Cmd,ServiceCmd, ContentCmd;
#ifdef USE_COMPUTER
    volatile BYTE Val[6] = {0,0,0,0,0,0};
#endif

#if defined(USE_LED)
    WORD Led_Count = BLINK_LED_NOT_JOIN;
#endif
// End by WSAN - lab411

//******************************************************************************
//******************************************************************************
//******************************************************************************
int main(void)
{
    CLKDIV = 0x0000;//Fcy = Fosc/2
    
    /* Configure PLL by WSAN-lab411 */
//    CLKDIVbits.PLLPRE = 0; // N1=2: PLL VCO Output Divider Select bits
//    PLLFBDbits.PLLDIV = 6; // M=8: PLL Feedback Divisor bits
//    CLKDIVbits.PLLPOST = 0; // N2=2: PLL Phase Detector Input Divider bits
//
//    /*
//     * Fosc = Fin*(M/(N1+N2))
//     * N1 = PLLPRE + 2
//     * N2 = 2 x (PLLPOST + 1)
//     * M = PLLDIV + 2
//     * -> Fosc = 8M*(8/4) = 16MHz -> Fcy = 16/2 = 8MIPS
//     */
//    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
//    __builtin_write_OSCCONH(0x03); // NOSC !!
//    __builtin_write_OSCCONL(0x01);
//    // Wait for Clock switch to occur
//    while (OSCCONbits.COSC != 0b011); // Internal OSC :000
//    // XT: 010
//    // Wait for PLL to lock
//    while (OSCCONbits.LOCK != 1);
//    OSCTUN = 0; // Tune FRC oscillator, if FRC is used
//    RCONbits.SWDTEN = DISABLE; // Disable Watch Dog Timer

    // Peripheral Pin Select
    asm volatile (  "mov #OSCCONL, w1  \n"
                    "mov #0x46, w2     \n"
                    "mov #0x57, w3     \n"
                    "mov.b w2, [w1]    \n"
                    "mov.b w3, [w1]    \n"
                    "bclr OSCCON, #6");

    // INT1 >>> RP18------------------------------------------------------------
    RPINR0bits.INT1R = 0b10010;
    // ------SPI1---------------------------------------------------------------
    // SCK1 >> RP15 >> RB15
    RPINR20bits.SCK1R = 0b01111;
    RPOR7bits.RP15R = 0b01000;
    // SDO1 >> RP16 >> RC0
    RPOR8bits.RP16R = 0b00111;
    // SDI1 >> RP17 >> RC1
    RPINR20bits.SDI1R = 0b10001;
    //--------SPI2--------------------------------------------------------------
    //SCK2 >> RP24
    RPINR22bits.SCK2R = 0b11000;
    RPOR12bits.RP24R = 0b01011;
    //SDO2 >> RP21
    RPOR10bits.RP21R = 0b01010;
    //SDI2 >> RP20
    RPINR22bits.SDI2R = 0b10100;
    //--------UART1-------------------------------------------------------------
    // RX1 >> RP22
    RPINR18bits.U1RXR = 22;
    // TX1 >> RP9
    RPOR4bits.RP9R = 3;
    //--------UART2-------------------------------------------------------------
    // RX2 >> RP19
    RPINR19bits.U2RXR = 19;
    //TX2 >> RP25
    RPOR12bits.RP25R = 5;

    asm volatile (  "mov #OSCCONL, w1  \n"
                    "mov #0x46, w2     \n"
                    "mov #0x57, w3     \n"
                    "mov.b w2, [w1]    \n"
                    "mov.b w3, [w1]    \n"
                    "bset OSCCON, #6");
    //--------------------------------------------------------------------------
    /* End by WSAN-lab411 */
    CLRWDT();
    ENABLE_WDT();

    NetworkDescriptor = NULL;
    orphanTries = 3;//the number of rejoin as a orphan node

    // If you are going to send data to a terminal, initialize the UART.
    ConsoleInit();

    // Initialize the hardware - must be done before initializing ZigBee.
    HardwareInit();
    printf("Init Hardware\r\n");

    // Initialize the ZigBee Stack.
    ZigBeeInit();
    printf("Init Zigbee\r\n");

    // *************************************************************************
    // Perform any other initialization here
    // *************************************************************************
    printf("R&D ZigBee-Router EMB by WSAN-Lab411\r\n");
    #if (RF_CHIP == MRF24J40)
        printf("Transceiver-MRF24J40\r\n");
    #else
        printf("Transceiver-Unknown\r\n" );
    #endif

    // Enable interrupts to get everything going.
    RFIE = ENABLE;

    /* Start the network anew each time Node is booted up */
    NWKClearNeighborTable();
    #if defined(I_SUPPORT_BINDING)
        ClearBindingTable();
    #endif
    
    /* Clear the Group Table */
    RemoveAllGroups();
    
    #if defined(I_SUPPORT_BINDING)
        RemoveAllBindings(macPIB.macShortAddress);
    #endif
    
    /* Clearing nwk status flags */
    nwkStatus.flags.Val = 0x00;

    /* Initialize for primitive */
    currentPrimitive = NO_PRIMITIVE;
    
    while (1)
    {
        CLRWDT();

        /* Determine which is the next ZigBee Primitive to operate on */
        ZigBeeTasks( &currentPrimitive );

        /* Process the next ZigBee Primitive */
        ProcessZigBeePrimitives();

        /* do any non ZigBee related tasks and then go back to ZigBee tasks */
//        ProcessNONZigBeeTasks();

        #if defined (USE_LED)
            if(Led_Count == 0)
            {
                LED0 = ~LED0;

                if(ZigBeeStatus.flags.bits.bNetworkJoined)
                    Led_Count = BLINK_LED_JOIN;
                else
                    Led_Count = BLINK_LED_NOT_JOIN;
            }
            else
            {
                --Led_Count;
            }
        #endif
    }
}     

/* Prototype: void ProcessZigBeePrimitive(void)
 * Input: None
 * Output: None
 * Discribe: Gui ket qua xu ly primitive hien tai len lop ung dung.
 *           Thiet lap primitive moi cho vong lap ke tiep.
 * Note: NO_PRIMITIVE va APSDE_DATA_indication la 2 primitive dung de gui va
 *       nhan ban tin chuan Zigbee.
 */
void ProcessZigBeePrimitives(void)
{
    switch (currentPrimitive)
    {
        case NLME_ROUTE_DISCOVERY_confirm:
            if (!params.NLME_ROUTE_DISCOVERY_confirm.Status)
            {
                printf("Route Reply OK\r\n" );
            }
            else
            {
                PrintChar( params.NLME_ROUTE_DISCOVERY_confirm.Status );
                printf(" Route Reply Failed\r\n" );
            }
            currentPrimitive = NO_PRIMITIVE;
            break;

        case NLME_NETWORK_DISCOVERY_confirm:
            currentPrimitive = NO_PRIMITIVE;

            switch(params.NLME_NETWORK_DISCOVERY_confirm.Status)
            {
                case 0x00:
                    printf("Number of Zigbee network have been find: ");
                    PrintChar(params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount);
                    printf("\r\n");

                    // Save the descriptor list pointer so we can destroy it later.
                    NetworkDescriptor = params.NLME_NETWORK_DISCOVERY_confirm.NetworkDescriptor;

                    // Select a network to try to join.  We're not going to be picky right now...
                    currentNetworkDescriptor = NetworkDescriptor;

SubmitJoinRequest:
                    params.NLME_JOIN_request.PANId = currentNetworkDescriptor->PanID;

                    printf("Prepare join to network: 0x");
                    PrintWord(params.NLME_JOIN_request.PANId.Val);
                    printf("\r\n");

                    if(params.NLME_JOIN_request.PANId.Val != 0x1AAA)
                    {
                        currentNetworkDescriptor = currentNetworkDescriptor->next;
                        if(currentNetworkDescriptor)
                        {
                            goto SubmitJoinRequest;
                        }
                        else
                        {
                            printf("Not found desire network 0x1AAA\r\n");

                            ZigBeeStatus.flags.bits.bNetworkJoined = 0;
                            ZigBeeStatus.flags.bits.bTryingToJoinNetwork = 0;
                        }
                    }
                    else
                    {
                        printf("Found desire network 0x1AAA\r\n");

                        params.NLME_JOIN_request.JoinAsRouter   = TRUE;
                        params.NLME_JOIN_request.RejoinNetwork  = FALSE;
                        params.NLME_JOIN_request.PowerSource    = MAINS_POWERED;
                        params.NLME_JOIN_request.RxOnWhenIdle   = TRUE;
                        params.NLME_JOIN_request.MACSecurity    = FALSE;
                        params.NLME_JOIN_request.ExtendedPANID = currentNetworkDescriptor->ExtendedPANID;

                        currentPrimitive = NLME_JOIN_request;

                        printf("Network(s) found. Trying to join " );
                        PrintWord(params.NLME_JOIN_request.PANId.Val);
                        printf(" | ");
                        for(i=7;i ^ 0xFF;--i)
                            PrintChar(currentNetworkDescriptor->ExtendedPANID.v[i]);
                        printf("\r\n");
                    }
                    break;

                case 0xEA:
                    if (!params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount)
                    {
                        printf("No networks found. Trying again!\r\n" );
                    }
                    break;

                default:
                    PrintChar( params.NLME_NETWORK_DISCOVERY_confirm.Status );
                    printf(" Error finding network. Trying again...\r\n" );
                    break;
            }
            break;

        case NLME_JOIN_confirm:
            if (!params.NLME_JOIN_confirm.Status)
            {
                printf("Join to this network OK!\r\n" );

                // Free the network descriptor list, if it exists. If we joined as an orphan, it will be NULL.
                while (NetworkDescriptor)
                {
                    currentNetworkDescriptor = NetworkDescriptor->next;//tro toi bang mo ta mang tim thay tiep theo
                    nfree( NetworkDescriptor );//giai phong bang mo ta mang truoc do
                    NetworkDescriptor = currentNetworkDescriptor;
                }

                // Start routing capability.
                params.NLME_START_ROUTER_request.BeaconOrder = MAC_PIB_macBeaconOrder;
                params.NLME_START_ROUTER_request.SuperframeOrder = MAC_PIB_macSuperframeOrder;
                params.NLME_START_ROUTER_request.BatteryLifeExtension = FALSE;
                currentPrimitive = NLME_START_ROUTER_request;
            }
            else
            {
                currentPrimitive = NO_PRIMITIVE;
                printf("Status ");
                PrintChar( params.NLME_JOIN_confirm.Status );

                // If we were trying as an orphan, see if we have some more orphan attempts.
                if (ZigBeeStatus.flags.bits.bTryOrphanJoin)
                {
                    // If we tried to join as an orphan, we do not have NetworkDescriptor, so we do
                    // not have to free it.
                    printf(". Could not join as orphan. " );
                    orphanTries--;
                    /* For Zigbee 2006 09/17/07, we now need to do an orphan join after a reset, but
                     * we choose not to do this forever, and quit after a few of retries
                     */
                    if (orphanTries)
                    {
                        printf("Trying join as orphan again!\r\n" );
                    }
                    else
                    {
                        orphanTries = 3;
                        printf("Must now try as a new node...\r\n" );
                        ZigBeeStatus.flags.bits.bTryOrphanJoin = CLEAR;
                    }
                }
                else
                {
                    printf(". Could not join selected network: " );
                    PrintWord(currentNetworkDescriptor->PanID.Val);

                    currentNetworkDescriptor = currentNetworkDescriptor->next;
                    //if have other networks
                    if (currentNetworkDescriptor)
                    {
                        printf(". Trying next discovered network: " );
                        PrintWord(currentNetworkDescriptor->PanID.Val);
                        printf("\r\n");
                        goto SubmitJoinRequest;
                    }
                    //neu chi tim thay mot mang duy nhat
                    else
                    {
                        //Ran out of descriptors.  Free the network descriptor list, and fall
                        // through to try discovery again.
                        printf(". Cleaning up and retrying discovery!\r\n" );
                        while (NetworkDescriptor)
                        {
                            currentNetworkDescriptor = NetworkDescriptor->next;
                            nfree( NetworkDescriptor );
                            NetworkDescriptor = currentNetworkDescriptor;
                        }
                    }
                }
            }
            break;

        case NLME_START_ROUTER_confirm:
            if (!params.NLME_START_ROUTER_confirm.Status)
            {
                printf("Router Started! Enabling joins...\r\n" );
                params.NLME_PERMIT_JOINING_request.PermitDuration = 0xFF;   // No Timeout
                params.NLME_PERMIT_JOINING_request._updatePayload = TRUE;
                currentPrimitive = NLME_PERMIT_JOINING_request;
            }
            else
            {
                printf("Router start Failed:" );
                PrintChar( params.NLME_JOIN_confirm.Status );
                printf(". We cannot route frames\r\n" );
                currentPrimitive = NLME_START_ROUTER_request;//request again
            }
            break;

        case NLME_PERMIT_JOINING_confirm:
            if (!params.NLME_PERMIT_JOINING_confirm.Status)
            {
                printf("Current Network Address is: ");
                PrintWord(macPIB.macShortAddress.Val);
                printf("\r\n");
                if(macPIB.macShortAddress.Val != 0x0001)
                {
                    // Thiet lap lai dia chi mang cua Router_EMB @dat_a3cbq91
                    macPIB.macShortAddress.v[1] = NetworkAddrMSB_EMB;
                    macPIB.macShortAddress.v[0] = NetworkAddrLSB_EMB;
                    // Set ShortAddr on Transceiver !
                    PHYSetShortRAMAddr(0x03, macPIB.macShortAddress.v[0]);
                    PHYSetShortRAMAddr(0x04, macPIB.macShortAddress.v[1]);
                    printf("Network Address has just re-assigned is 0x0001\r\n");
                }
                currentPrimitive = NO_PRIMITIVE;
            }
            else
            {
                printf("Join permission Failed: " );
                PrintChar( params.NLME_PERMIT_JOINING_confirm.Status );
                printf(". We cannot allow joins\r\n" );
                currentPrimitive = NLME_PERMIT_JOINING_request;//request again
            }
            break;

        case NLME_JOIN_indication:
            printf( "Node " );
            PrintWord(params.NLME_JOIN_indication.ShortAddress.Val);
            printf( " With MAC Address " );
            //@dat_a3cbq91: Print MAC address of node has just joined
            for(i = 7; i ^ 0xFF; --i)
            {
                PrintChar(params.NLME_JOIN_indication.ExtendedAddress.v[i]);
            }
            printf( " just joined.\r\n" );
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

                if(LookupAPSAddress(&params.NLME_JOIN_indication.ExtendedAddress) )
                {

                    for( i = 0; i ^ apscMaxAddrMapEntries; i++)
                    {
                        #ifdef USE_EXTERNAL_NVM
                            GetAPSAddress( &currentAPSAddress,  apsAddressMap + i * sizeof(APS_ADDRESS_MAP) );
                        #else
                            GetAPSAddress( &currentAPSAddress,  &apsAddressMap[i] );
                        #endif
                        if (!memcmp( (void *)&currentAPSAddress.longAddr, (void *)&currentAPSAddress1.longAddr, 8 ))
                        {
                            /* overwrite the old with the new short/long address combo  */
                            #ifdef USE_EXTERNAL_NVM
                                PutAPSAddress( apsAddressMap + i * sizeof(APS_ADDRESS_MAP), &currentAPSAddress1);
                            #else
                                PutAPSAddress( &apsAddressMap[i], &currentAPSAddress1 );
                            #endif
                        }
                    }   /* end for loop */
                }       /* end if */
            }           /* end address check block */
            #ifdef I_SUPPORT_SECURITY
                #ifdef I_AM_TRUST_CENTER
                {
                    BOOL AllowJoin = TRUE;
                    // decide if you allow this device to join
                    if( !AllowJoin )
                    {
                        // no need to set deviceAddress, since it is overlap with NLME_JOIN_indication
                        //params.NLME_LEAVE_request.DeviceAddress = params.NLME_JOIN_indication.ExtendedAddress;
                        params.NLME_LEAVE_request.RemoveChildren = TRUE;
                        currentPrimitive = NLME_LEAVE_request;
                        break;
                    }

                    #ifdef I_SUPPORT_SECURITY_SPEC
                        if( params.NLME_JOIN_indication.secureJoin )
                        {
                            BYTE i;
                            for(i = 0; i < 16; i++)
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
                                currentNetworkKeyInfo = plainSecurityKey[i-1];
                            #else
                                GetNwkKeyInfo(&currentNetworkKeyInfo, (ROM void *)&(NetworkKeyInfo[i-1]));
                            #endif
                            params.APSME_TRANSPORT_KEY_request.Key = &(currentNetworkKeyInfo.NetKey);
                            params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = currentNetworkKeyInfo.SeqNumber.v[0];
                        }
                    #else
                        #ifdef PRECONFIGURE_KEY
                        {
                            BYTE i;
                            for(i = 0; i < 16; i++)
                            {
                                KeyVal.v[i] = 0;
                            }
                            params.APSME_TRANSPORT_KEY_request.Key = &KeyVal;
                            params.APSME_TRANSPORT_KEY_request.TransportKeyData.NetworkKey.KeySeqNumber = 0;
                            params.APSME_TRANSPORT_KEY_request._UseSecurity = TRUE;
                        }
                        #else
                            if( params.NLME_JOIN_indication.secureJoin )
                            {
                                BYTE i;
                                for(i = 0; i < 16; i++)
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
                                    currentNetworkKeyInfo = plainSecurityKey[i-1];
                                #else
                                    GetNwkKeyInfo(&currentNetworkKeyInfo, (ROM void *)&(NetworkKeyInfo[i-1]));
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
                        params.APSME_UPDATE_DEVICE_request.Status = (params.NLME_JOIN_indication.secureJoin ) ? 0x00 : 0x01;
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
                    for(i=0; i < 8; i++)
                        params.APSME_UPDATE_DEVICE_request.DestAddress.v[i] = 0xaa;

                    currentPrimitive = APSME_UPDATE_DEVICE_request;
                #endif
            #else
                currentPrimitive = NO_PRIMITIVE;
            #endif
            break;

        case NLME_LEAVE_indication:
            {
                LONG_ADDR myLongAddr;

                GetMACAddress(&myLongAddr);
                if(!memcmppgm2ram( &params.NLME_LEAVE_indication.DeviceAddress, &myLongAddr, 8 ))
                {
                    printf("We have left the network\r\n" );
                }
                else
                {
                    printf("Another node has left the network\r\n" );
                }
            }
            #ifdef I_SUPPORT_SECURITY
            {
                SHORT_ADDR	LeavingChildShortAddress;
                if( !APSFromLongToShort(&params.NLME_LEAVE_indication.DeviceAddress) )
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
            printf("ZigBee Stack has been reset\r\n" );
            /* For Zigbee 2006 The Specs require that node needs to
             * try joining as an orphan first after each reset,
             * see Mandatory test 3.9
            */
            ZigBeeStatus.flags.bits.bTryOrphanJoin = SET;

            currentPrimitive = NO_PRIMITIVE;
            break;

        case NLME_LEAVE_confirm:
            PrintChar(params.NLME_LEAVE_confirm.Status);
            printf(" Leaving the Zigbee network\r\n" );
            
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSDE_DATA_indication:
            {
                currentPrimitive = NO_PRIMITIVE;

                switch (params.APSDE_DATA_indication.DstEndpoint)
                {
                    // ************************************************************************
                    // Place a case for each user defined endpoint.
                    // ************************************************************************
                    case WSAN_Dst:
                    {
                        switch(params.APSDE_DATA_indication.ClusterId.Val)
                        {
                            case SLEEP_CONFIRM_CLUSTER:
                                printf("#VL:");
                                PrintChar(APLGet());
                                printf("\r\n");
                                break;

                            case WAKE_CONFIRM_CLUSTER:
                                printf("#WC:");
                                PrintChar(APLGet());
                                printf("\r\n");
                                break;

                            case STATE_NODE_CLUSTER:
                                printf("#SN:");
                                PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);
                                PrintChar(APLGet());//MM
                                PrintChar(APLGet());//SS
                                printf("\r\n");
                                break;

                            case JOIN_CONFIRM_CLUSTER:
                                printf("#JN:");
                                PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);
                                PrintChar(APLGet()); //MM
                                APLGet();//Rejected byte
                                printf("\r\n");
                                #if defined(USE_LED)
                                    LED1 = ~LED1;
                                #endif
                                break;
                                
                            case HTE_RESPONSE_CLUSTER:
                                {
                                    printf("#RD:");
                                    PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);
                                    PrintChar(APLGet());//Byte0 dia chi MAC
                                    PrintChar(APLGet());//nhiet do
                                    PrintChar(APLGet());
                                    PrintChar(APLGet());//do am
                                    PrintChar(APLGet());
                                    PrintChar(APLGet());//dien ap
                                    PrintChar(APLGet());
                                    printf("\r\n");
                                    #if defined(USE_LED)
                                        LED2 = ~LED2;
                                    #endif
                                }
                                break;

                            case HTE_AUTO_SEND_CLUSTER:
                                {
                                    printf("#AD:");
                                    PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);
                                    PrintChar(APLGet());//Byte0 dia chi MAC
                                    PrintChar(APLGet());//nhiet do
                                    PrintChar(APLGet());
                                    PrintChar(APLGet());//do am
                                    PrintChar(APLGet());
                                    PrintChar(APLGet());//dien ap
                                    PrintChar(APLGet());
                                    printf("\r\n");
                                    #if defined(USE_LED)
                                        LED2 = ~LED2;
                                    #endif
                                }
                                break;

                            /* Place other Cluster.ID cases here */
                            case ACTOR_RESPONSE_CLUSTER:
                                {
                                    printf("#OK:");
                                    PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);//Dia chi mang node gui
                                    PrintChar(APLGet());//Byte0 dia chi MAC ndoe gui
                                    PrintChar(APLGet());//Ma xac nhan cua Actor
                                    printf("\r\n");
                                    #if defined(USE_LED)
                                        LED3 = ~LED3;
                                    #endif
                                }
                                break;
								
                                case ROUTE_DRAW_REPLY_CLUSTER:
                                    {
                                        BYTE i;
                                        BYTE NodeNumber;
                                        BYTE *pListNode;
                                        printf("#TR:");
                                        PrintWord(params.APSDE_DATA_indication.SrcAddress.ShortAddr.Val);//Dia chi mang node gui
                                        NodeNumber = APLGet();//Lay ro so node mang goi tin di qua
                                        PrintChar(NodeNumber);
                                        for (i = 0; i<NodeNumber;i++) PrintChar(APLGet());
                                        printf("\r\n");
                                    }
                                    break;
						
                            default:
                                break;
                        }   /* switch end*/

                        if( currentPrimitive != APSDE_DATA_request )
                            TxData = TX_DATA_START;
                    }           /* if msg */
                        break;

                    default:
                        break;
                }
                APLDiscardRx();
            }
            break;

        case APSDE_DATA_confirm:
            if (params.APSDE_DATA_confirm.Status)
            {
                PrintChar(params.APSDE_DATA_confirm.Status);
                printf(" Error sending message\r\n");
            }
            else
            {
                printf("Sending message OK!\r\n" );
            }
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSME_ADD_GROUP_confirm:
        case APSME_REMOVE_GROUP_confirm:
        case APSME_REMOVE_ALL_GROUPS_confirm:
            printf("Perform Group Operation\r\n" );
            currentPrimitive = NO_PRIMITIVE;
            break;

        case NO_PRIMITIVE:
            //if the router has not joined any network, do the following
            if (AllowJoin &&!ZigBeeStatus.flags.bits.bNetworkJoined)
            {
                //if the router has not tried to join any network
                if (!ZigBeeStatus.flags.bits.bTryingToJoinNetwork)
                {
                    //if the router want to join as an orphan
                    if (ZigBeeStatus.flags.bits.bTryOrphanJoin)
                    {
                        printf("Trying to join network as an orphan...\r\n" );

                        params.NLME_JOIN_request.ScanDuration     = 8;
                        params.NLME_JOIN_request.ScanChannels.Val = ALLOWED_CHANNELS;
                        params.NLME_JOIN_request.JoinAsRouter     = TRUE;
                        params.NLME_JOIN_request.RejoinNetwork    = 0x01;
                        params.NLME_JOIN_request.PowerSource      = MAINS_POWERED;
                        params.NLME_JOIN_request.RxOnWhenIdle     = TRUE;
                        params.NLME_JOIN_request.MACSecurity      = FALSE;

                        params.NLME_JOIN_request.ExtendedPANID = currentNetworkDescriptor->ExtendedPANID;
                        currentPrimitive = NLME_JOIN_request;
                    }
                    else
                    {
                        printf("Trying to join network as a new device!\r\n" );
                        params.NLME_NETWORK_DISCOVERY_request.ScanDuration          = 8;
                        params.NLME_NETWORK_DISCOVERY_request.ScanChannels.Val      = ALLOWED_CHANNELS;
                        currentPrimitive = NLME_NETWORK_DISCOVERY_request;
                    }
                }
            }
            else
            {
                #ifdef USE_COMPUTER
                if (ZigBeeReady() & (!ZigBeeStatus.flags.bits.bHasBackgroundTasks))
                {
                    if(EnableSendCmd)
                    {
                        Addr_MSB = GetHexByte(Val[0],Val[1]);
                        Addr_LSB = GetHexByte(Val[2],Val[3]);
                        Cmd = GetHexByte(Val[4],Val[5]);

                        printf("Command: ");
                        PrintChar(Addr_MSB);
                        PrintChar(Addr_LSB);
                        PrintChar(Cmd);
                        printf("\r\n");
                        
                        //Neu Buffer[2] = 0x00 -> yeu cau du lieu nhiet do - do am
                        //Neu Buffer[2] != 0x00 -> yeu cau bat van
                                                //for send broadcast messages
                        if ((Addr_MSB == 0xff)&&(Addr_LSB == 0xff))
                        {
                            ContentCmd = Cmd & 0x1F;//take content of cmd
                            ServiceCmd = Cmd >> 5;//take kind of cmd

                            switch (ServiceCmd)
                            {
                                case 1:
                                    SendOneByte(SLEEP_SYN_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    EnableSendCmd = CLEAR;
                                    #ifdef USE_USART
                                        printf("Sleep request OK\r\n");
                                    #endif
                                    break;

                                case 2:
                                    #ifdef USE_EXTERNAL_NVM
                                        pCurrentRoutingEntry = routingTable;   //+ (WORD)neighborIndex * (WORD)sizeof(NEIGHBOR_RECORD);
                                    #else
                                        pCurrentRoutingEntry = &(routingTable[0]);
                                    #endif
                                    printf("#RT:");
                                    PrintChar(macPIB.macShortAddress.v[1]);
                                    PrintChar(macPIB.macShortAddress.v[0]);
                                    for ( i=0; i < ROUTING_TABLE_SIZE-RESERVED_ROUTING_TABLE_ENTRIES; i++ )
                                    {
                                        GetRoutingEntry(&currentRoutingEntry,pCurrentRoutingEntry);
                                        if ((currentRoutingEntry.destAddress.byte.MSB !=0xff))
                                        {
                                            PrintChar(currentRoutingEntry.destAddress.byte.MSB);
                                            PrintChar(currentRoutingEntry.destAddress.byte.LSB);
                                            PrintChar(currentRoutingEntry.nextHop.byte.MSB);
                                            PrintChar(currentRoutingEntry.nextHop.byte.LSB);
                                        }
                                        #ifdef USE_EXTERNAL_NVM
                                            pCurrentRoutingEntry += (WORD)sizeof(ROUTING_ENTRY);
                                        #else
                                            pCurrentRoutingEntry++;
                                        #endif
                                    }
                                    printf("\r\n");
                                    SendOneByte(ROUTING_TABLE_READ_CLUSTER, Addr_MSB, Addr_LSB, Cmd);
                                    EnableSendCmd = CLEAR;
                                    #ifdef USE_USART
                                        printf("Read Routing Table request OK");
                                    #endif
                                    break;

                                case 0x:
                                    SendOneByte(RE_CONFIG_CYCLE_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    break;

                                case 5:
                                    SendOneByte(RE_ASSIGN_LEVEL_MQ6_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    break;

                                default:
                                    break;
                            }
                        }
                        else
                        {
                            //for send unicast message
                            switch (Cmd)
							{
							case 0x00://Lay nhiet do - do am sensor
								SendOneByte(HTE_REQUEST_CLUSTER, Addr_MSB,Addr_LSB,Cmd);
							break;
							case 0xF1://Yeu cau ve tuyen
							SendRouteDrawRequest(Addr_MSB, Addr_LSB);
							break;
                            default://Gui lenh cho Actor
                            SendOneByte(ACTOR_REQUEST_CLUSTER, Addr_MSB,Addr_LSB,Cmd);
							break;
                            EnableSendCmd = CLEAR;
							}
                        }
                    }
                }//end computer
                #else
                if (ZigBeeReady() & (!ZigBeeStatus.flags.bits.bHasBackgroundTasks))
                {
                    if(EnableSendCmd) 
                    {
                        //for send broadcast messages
                        if ((Addr_MSB == 0xff)&&(Addr_LSB == 0xff))
                        {
                            ContentCmd = Cmd & 0x0F;//take content of cmd
                            ServiceCmd = Cmd & 0xF0;//take kind of cmd

                            switch (ServiceCmd)
                            {
                                //che do tiet kiem nang luong: tat - ngu luan phien
                                case 0x00:
                                    //Neu ContentCmd = 0x01 thi chuyen tu che do active --> tiet kiem nang luong.
                                    //Neu ContentCmd = 0x02 thi chuyen tu che do tiet kiem nang luong --> active.
                                    SendOneByte(SLEEP_SYN_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    EnableSendCmd = CLEAR;
                                    
                                    if(ContentCmd == 0x01)
                                        printf("Sleep request OK\r\n");
                                    if(ContentCmd == 0x02)
                                    	printf("Normal Mode requested OK\r\n");
                                    break;

                                //thay doi chu ki gui du lieu nhiet do do am trong che do active.
                                case 0x10:
                                    SendOneByte(RE_CONFIG_CYCLE_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    EnableSendCmd = CLEAR;
                                    break;

                                //thay doi nguong so sanh cam bien MQ6_V1
                                case 0x20:
                                    SendOneByte(RE_ASSIGN_LEVEL_MQ6_CLUSTER, Addr_MSB, Addr_LSB, ContentCmd);
                                    EnableSendCmd = CLEAR;
                                    break;

                                default:
                                    break;
                           }
                        }
                        else
                        {
                            switch(Cmd)
                            {
                                case 0x00:
                                    SendOneByte(HTE_REQUEST_CLUSTER, Addr_MSB,Addr_LSB,Cmd);//Lay nhiet do - do am sensor
                                    break;

                                case 0xF1:
                                    SendRouteDrawRequest(Addr_MSB, Addr_LSB);	//Yeu cau ve tuyen
                                    break;

                                default:
                                    SendOneByte(ACTOR_REQUEST_CLUSTER, Addr_MSB,Addr_LSB,Cmd);//Gui lenh cho Actor
                                    break;
                            }
                            
                            EnableSendCmd = CLEAR;
                        }
                    }
                }
                #endif
            }
            break;

        default:
            PrintChar( currentPrimitive );
            printf(" Unhandled primitive\r\n" );
            currentPrimitive = NO_PRIMITIVE;
            break;
    }
}

///* Prototype: void ProcessNONZigBeeTasks(void)
// * Input: None
// * Output: None
// * Discribe: xu ly cac cong viec khong lien quan den Zigbee.
// */
//void ProcessNONZigBeeTasks(void)
//{
//
//        // *********************************************************************
//        // Place any non-ZigBee related processing here.  Be sure that the code
//        // will loop back and execute ZigBeeTasks() in a timely manner.
//        // *********************************************************************
//
//}

/*******************************************************************************
HardwareInit

All port directioning and SPI must be initialized before calling ZigBeeInit().

For demonstration purposes, required signals are configured individually.
*******************************************************************************/
void HardwareInit(void)
{
    //digital pin
    AD1PCFGL = 0x01FF;
    #if(CLOCK_FREQ < 1000000)
        SPI1CON1 = 0b0000000100111111;      // CLOCK_FREQ as SPI CLOCK
        SPI1STAT = 0x8000;

        SPI2CON1 = 0b0000000100111111;      // CLOCK_FREQ as SPI CLOCK
        SPI2STAT = 0x8000;
    #else
        SPI1CON1 = 0b0000000100111110;      // CLOCK_FREQ/4 as SPI CLOCK
        SPI1STAT = 0x8000;

        SPI2CON1 = 0b0000000100111110;      // CLOCK_FREQ/4 as SPI CLOCK
        SPI2STAT = 0x8000;
    #endif

    #ifdef USE_EXTERNAL_NVM
        EEPROM_nCS = 1;
        EEPROM_nCS_TRIS = 0;
        IFS2bits.SPI2IF = 1;
    #endif

    PHY_RESETn = 0;
    PHY_RESETn_TRIS = 0;
    PHY_CS = 1;
    PHY_CS_TRIS = 0;
    
    RFIF = CLEAR;
    RFIE = ENABLE;
    
    //pins connect to leds is
    #if defined(USE_LED)
    LED0_TRIS = CLEAR;
    LED1_TRIS = CLEAR;
    LED2_TRIS = CLEAR;
    LED3_TRIS = CLEAR;
    #endif

    if (RF_INT_PIN == 0)
    {
        RFIF = SET;
    }

    RF_SPIInit();
    EE_SPIInit();
}

/*******************************************************************************
User Interrupt Handler

The stack uses some interrupts for its internal processing. Once it is done
checking for its interrupts, the stack calls this function to allow for any
additional interrupt processing.
*******************************************************************************/
#ifdef USE_COMPUTER
void _ISR __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
    BYTE d;
    IFS1bits.U2RXIF = CLEAR;
    Val[counter] = U2RXREG;
    if(++counter == 6)
    {
        counter = CLEAR;
        EnableSendCmd = ENABLE;
        while(U2STAbits.URXDA)
        {
            d = U2RXREG;
        }
    }
}

/* Writen by Hoang Anh Son */
BYTE GetHexByte(BYTE a, BYTE b)
{
    BYTE oneByte;
    //gia tri nhap vao tu 0 -> 9
    if ((a>=0x30)&&(a<=0x39))
        oneByte = (a - 0x30) << 4;
    //gia tri nhap vao tu A -> F
    else oneByte = (a - 0x37) << 4;

    if ((b>=0x30)&&(b<=0x39))
        oneByte += (b - 0x30);
    else oneByte += (b - 0x37);

    return oneByte;
}
/* End by Hoang Anh Son */
#else
/* Writen by Hoang Anh Son */
void _ISR __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
    BYTE data;
    data = U2RXREG;
    IFS1bits.U2RXIF = CLEAR;
    if (data == '$') counter = CLEAR;
    else
    {
        switch(counter)
        {
            case 0:
                Addr_MSB = data;
                ++counter;
                break;
            case 1:
                Addr_LSB = data;
                ++counter;
                break;
            case 2:
                Cmd = data;
                EnableSendCmd = ENABLE;
                ++counter;
                break;

            default:
                counter = CLEAR;
                #if defined USE_USART
                    printf("Command Error\r\n");
                #endif
                break;
        }
    }
}
/* End by Hoang Anh Son */
#endif
/*********************************************************************
 * Function:        void SendRouteDrawRequest(BYTE MSB_addr, BYTE LSB_addr)
 *
 * PreCondition:
 *
 * Cluater:         REQUEST_ROUTE_CLUSTER
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Overview:        Gui yeu cau ve tuyen tu Gateway den nut co dia chi mang MSB_addr, LSB_addr
 *
 * Note: By Hoang Anh Son - K53
 ********************************************************************/
void SendRouteDrawRequest(BYTE MSB_dest_addr, BYTE LSB_dest_addr)
{
    BYTE i;
    TxBuffer[TxData++] = MSB_dest_addr;//Dia chi Node dich
    TxBuffer[TxData++] = LSB_dest_addr;
    TxBuffer[TxData++] = 0x00;
    for (i = 0; i < ROUTING_TABLE_SIZE-RESERVED_ROUTING_TABLE_ENTRIES; i++ )//Gui den forward Node
    {
        GetRoutingEntry(&currentRoutingEntry,pCurrentRoutingEntry);
        if ((MSB_dest_addr == currentRoutingEntry.destAddress.byte.MSB)&&(LSB_dest_addr == currentRoutingEntry.destAddress.byte.LSB)&&(currentRoutingEntry.status == ROUTE_ACTIVE))
        {
            params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = currentRoutingEntry.nextHop.byte.MSB;
            params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = currentRoutingEntry.nextHop.byte.LSB;
            break;
        }
    }
 
    params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;//0x02 : Ban tin Unicast, Set che do dia chi mang ( DstAddress) 16 bit va yeu cau co' DstEndPoint
    params.APSDE_DATA_request.SrcEndpoint = WSAN_Src;
    params.APSDE_DATA_request.DstEndpoint = WSAN_Dst;
    params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;

    //params.APSDE_DATA_request.asduLength; TxData
    params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;//gioi han so hop ma du lieu duoc phep truyen qua, o day la 10 hop

//    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_FORCE;
//    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_SUPPRESS;
    params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;

    #ifdef I_SUPPORT_SECURITY
        params.APSDE_DATA_request.TxOptions.Val = ENABLE;
    #else
        params.APSDE_DATA_request.TxOptions.Val = DISABLE;//khong ho tro bao mat
    #endif
    params.APSDE_DATA_request.TxOptions.bits.acknowledged = ENABLE;// Yeu cau ACK tu thiet bi thu
    params.APSDE_DATA_request.ClusterId.Val = ROUTE_DRAW_REQUEST_CLUSTER;

    ZigBeeBlockTx();//goi ham nay de ZigbeeIsReady() --> false
    currentPrimitive = APSDE_DATA_request;
}
/*********************************************************************
 * Function:        void SensOneByte(WORD ClusterID, BYTE MSB, BYTE LSB, BYTE cmd)
 *
 * PreCondition:
 *
 * Cluater:         
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Overview:        Dung de gui lenh den cac node mang
 *
 * Note:
 ********************************************************************/
void SendOneByte(WORD ClusterID, BYTE MSB, BYTE LSB, BYTE cmd)
{
    TxBuffer[TxData++] = cmd;
    ZigBeeBlockTx();

    /* load parameters for APSDE_DATA_request primitive */
    params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;

    /* load network address of router-emboard */
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[1] = MSB;
    params.APSDE_DATA_request.DstAddress.ShortAddr.v[0] = LSB;

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
        params.APSDE_DATA_request.TxOptions.Val = DISABLE;
    #endif
    params.APSDE_DATA_request.TxOptions.bits.acknowledged = ENABLE;
    params.APSDE_DATA_request.ClusterId.Val = ClusterID;

    currentPrimitive = APSDE_DATA_request;
}

