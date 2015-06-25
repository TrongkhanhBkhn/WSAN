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
 * Router-EMB duoc su dung trong khu theo doi dong vat
 *
 *******************************************************************************
 * FileName:        Router.c
 * Project:         DemoPIC24Router
 * Version:         2.0
 *
 * Controller:      dsPIC33FJ128MC804
 * Editor:          MPLAB X IDE v1.80
 * Complier:        C30 v3.31 or higher
 *
 * Company support: Microchip Technology, Inc.
 *
 * Developer:       Nguyen Tien Dat - KSTN - DTVT - K54
 * Group:           WSAN group - Lab411
 * Edition:         16/08/2013
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

//Internal Fast RC (FRC)
_FOSCSEL(FNOSC_FRC);

//Clock switching is enabled, Fail-Safe Clock Monitor is disabled
//OSC2 pin has clock out function
//XT Oscillator Mode
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);

//Watchdog timer enabled/disabled by user software
_FWDT(FWDTEN_OFF & WDTPOST_PS512);
	
//******************************************************************************
// Constants			//defined application service	@dat_a3cbq91
//******************************************************************************

//Define for useful constants
#define ON              1
#define OFF             0

//Define for network address of Router-EMB
#define MyNetworkAddrMSB  0x00
#define MyNetworkAddrLSB  0x01

//Define registers of MCU to control Leds
#if defined(USE_LED)
    #define LED0_TRIS TRISAbits.TRISA10
    #define LED1_TRIS TRISAbits.TRISA7
    #define LED2_TRIS TRISAbits.TRISA9
    #define LED3_TRIS TRISAbits.TRISA8

    #define LED0 LATAbits.LATA10
    #define LED1 LATAbits.LATA7
    #define LED2 LATAbits.LATA9
    #define LED3 LATAbits.LATA8

    #define BLINK_LED_JOIN              5000
    #define BLINK_LED_NOT_JOIN          500
#endif
//******************************************************************************
// Function Prototypes
//******************************************************************************

// Function is modified by WSAN - lab411
void ProcessZigBeePrimitives(void);
// End by WSAN - lab411

// Function create by WSAN - lab411
void HardwareInit( void );
//void ProcessNONZigBeeTasks(void);
void SendOneByte(WORD ClusterID, BYTE MSB, BYTE LSB, BYTE cmd);
// End by WSAN - Lab411

//******************************************************************************
// Application Variables
//******************************************************************************

ZIGBEE_PRIMITIVE    currentPrimitive;
NETWORK_DESCRIPTOR  *currentNetworkDescriptor;
NETWORK_DESCRIPTOR  *NetworkDescriptor;
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

BYTE AllowJoin = ON;

extern NWK_STATUS nwkStatus;

// Variable declared by WSAN - Lab411
BYTE    i;
#if defined(USE_LED)
    WORD Led_Count = BLINK_LED_NOT_JOIN;
#endif

// End by WSAN - lab411

//******************************************************************************
//******************************************************************************
//******************************************************************************
int main(void)
{
    /* Configure by WSAN-lab411 */
    //PLL setup:
    CLKDIVbits.PLLPRE = 0; // N1=2: PLL VCO Output Divider Select bits; 0 -> /2 (default)
    PLLFBDbits.PLLDIV = 6; // M=40: PLL Feedback Divisor bits; 30 -> 30 x multiplier
    // (divisor is 2 more than the value)
    CLKDIVbits.PLLPOST = 0; // N2=2: PLL Phase Detector Input Divider bits; 0 -> /2

    /*
     * Fosc = Fin*(M/(N1+N2))
     * N1 = PLLPRE + 2
     * N2 = 2 x (PLLPOST + 1)
     * M = PLLDIV + 2
     * >> Fosc = 8M*(32/4) = 64MHz >> Fcy = 64/2 = 32MIPS
     */
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03); // NOSC !!
    __builtin_write_OSCCONL(0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011); // Internal OSC :000
    // XT: 010
    // Wait for PLL to lock
    while (OSCCONbits.LOCK != 1);
    OSCTUN = 0; // Tune FRC oscillator, if FRC is used
    RCONbits.SWDTEN = OFF; // Disable Watch Dog Timer
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
    orphanTries = 3;

    // If you are going to send data to a terminal, initialize the UART.
    UART1Init();
    UART2Init();
    #if defined(USE_DEBUG)
        UART1PutROMString((ROM char*)"UART1init\r\n");
        UART2PutROMString((ROM char*)"UART2init\r\n");
    #endif

    // Initialize the hardware - must be done before initializing ZigBee.
    HardwareInit();
    #if defined(USE_DEBUG)
        UART1PutROMString((ROM char*)"Init Hardware\r\n");
        printf("Init Hardware\r\n");
    #endif

    // Initialize the ZigBee Stack.
    ZigBeeInit();
    #if defined(USE_LED)
        LED0 = ON;//prove that system is ready to use
    #endif
    #if defined(USE_DEBUG)
        printf("Init Zigbee\r\n");
    #endif

    // *************************************************************************
    // Perform any other initialization here
    // *************************************************************************
    #if defined(USE_DEBUG)
    printf("R&D ZigBee-Router EMB by WSAN-Lab411\r\n");
    #if (RF_CHIP == MRF24J40)
        printf("Transceiver-MRF24J40\r\n");
    #else
        printf("Transceiver-Unknown\r\n" );
    #endif
    #endif
    // Enable interrupts to get everything going.
    RFIE = ON;

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

        if(Led_Count == 0)
        {
            LED1 = ~LED1;

            if(ZigBeeStatus.flags.bits.bNetworkJoined)
                Led_Count = BLINK_LED_JOIN;
            else
                Led_Count = BLINK_LED_NOT_JOIN;
        }
        else
        {
            --Led_Count;
        }
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
            #if defined(USE_DEBUG)
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

            switch(params.NLME_NETWORK_DISCOVERY_confirm.Status)
            {
                case 0x00:
                    #if defined(USE_DEBUG)
                        printf("Number of Zigbee network have been find: ");
                        PrintChar(params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount);
                        printf("\r\n");
                    #endif
                    // Save the descriptor list pointer so we can destroy it later.
                    NetworkDescriptor = params.NLME_NETWORK_DISCOVERY_confirm.NetworkDescriptor;

                    // Select a network to try to join.  We're not going to be picky right now...
                    currentNetworkDescriptor = NetworkDescriptor;

SubmitJoinRequest:
                    params.NLME_JOIN_request.PANId = currentNetworkDescriptor->PanID;

                    #if defined(USE_DEBUG)
                        printf("Prepare join to network: 0x");
                        PrintWord(params.NLME_JOIN_request.PANId.Val);
                        printf("\r\n");
                    #endif

                    if(params.NLME_JOIN_request.PANId.Val != 0x1AAB)
                    {
                        currentNetworkDescriptor = currentNetworkDescriptor->next;
                        if(currentNetworkDescriptor)
                        {
                            goto SubmitJoinRequest;
                        }
                        else
                        {
                            #ifdef USE_DEBUG
                                printf("Not found desire network 0x1AAB\r\n");
                            #endif

                            ZigBeeStatus.flags.bits.bNetworkJoined = 0;
                            ZigBeeStatus.flags.bits.bTryingToJoinNetwork = 0;
                        }
                    }
                    else
                    {
                        #ifdef USE_DEBUG
                            printf("Found desire network 0x1AAB\r\n");
                        #endif
                        params.NLME_JOIN_request.JoinAsRouter   = TRUE;
                        params.NLME_JOIN_request.RejoinNetwork  = FALSE;
                        params.NLME_JOIN_request.PowerSource    = MAINS_POWERED;
                        params.NLME_JOIN_request.RxOnWhenIdle   = TRUE;
                        params.NLME_JOIN_request.MACSecurity    = FALSE;
                        params.NLME_JOIN_request.ExtendedPANID = currentNetworkDescriptor->ExtendedPANID;

                        currentPrimitive = NLME_JOIN_request;

                        #if defined(USE_DEBUG)
                            printf("Network(s) found. Trying to join " );
                            PrintWord(params.NLME_JOIN_request.PANId.Val);
                            printf(" | ");
                            for(i=7;i ^ 0xFF;--i)
                                PrintChar(currentNetworkDescriptor->ExtendedPANID.v[i]);
                            printf("\r\n");
                        #endif
                    }
                    break;

                case 0xEA:
                    #if defined(USE_DEBUG)
                        if (!params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount)
                        {
                            printf("No networks found. Trying again!\r\n" );
                        }
                    #endif
                    break;

                default:
                    #if defined(USE_DEBUG)
                        PrintChar( params.NLME_NETWORK_DISCOVERY_confirm.Status );
                        printf(" Error finding network. Trying again...\r\n" );
                    #endif
                    break;
            }
            break;

        case NLME_JOIN_confirm:
            if (!params.NLME_JOIN_confirm.Status)
            {
                #if defined(USE_DEBUG)
                UART1PutROMString((ROM char*)"Joining succesfull\r\n");
                printf("Join OK!\r\n" );
                #endif
                // Free the network descriptor list, if it exists. If we joined as an orphan, it will be NULL.
                while (NetworkDescriptor)
                {
                    currentNetworkDescriptor = NetworkDescriptor->next;
                    nfree( NetworkDescriptor );
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
                #if defined(USE_DEBUG)
                printf("Status: ");
                PrintChar( params.NLME_JOIN_confirm.Status );
                #endif

                // If we were trying as an orphan, see if we have some more orphan attempts.
                if (ZigBeeStatus.flags.bits.bTryOrphanJoin)
                {
                    // If we tried to join as an orphan, we do not have NetworkDescriptor, so we do
                    // not have to free it.
                    #if defined(USE_DEBUG)
                    printf(". Could not join as orphan. " );
                    #endif
                    orphanTries--;
                    /* For Zigbee 2006 09/17/07, we now need to do an orphan join after a reset, but
                     * we choose not to do this forever, and quit after a few of retries
                     */
                    if (orphanTries)
                    {
                        #if defined(USE_DEBUG)
                        printf("Trying join as orphan again!\r\n" );
                        #endif
                    }
                    else
                    {
                        orphanTries = 3;
                        #if defined(USE_DEBUG)
                        printf("Must now try as a new node...\r\n" );
                        #endif
                        ZigBeeStatus.flags.bits.bTryOrphanJoin = 0;
                    }
                }
                else
                {
                    #if defined(USE_DEBUG)
                    printf(". Could not join selected network. " );
                    #endif
                    currentNetworkDescriptor = currentNetworkDescriptor->next;
                    if (currentNetworkDescriptor)
                    {
                        #if defined(USE_DEBUG)
                        printf("Trying next discovered network!\r\n" );
                        #endif
                        goto SubmitJoinRequest;
                    }
                    else
                    {
                        //Ran out of descriptors.  Free the network descriptor list, and fall
                        // through to try discovery again.
                        #if defined(USE_DEBUG)
                        printf("Cleaning up and retrying discovery!\r\n" );
                        #endif
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
                #if defined(USE_DEBUG)
                printf("Router Started! Enabling joins...\r\n" );
                #endif
                params.NLME_PERMIT_JOINING_request.PermitDuration = 0xFF;   // No Timeout
                params.NLME_PERMIT_JOINING_request._updatePayload = TRUE;
                currentPrimitive = NLME_PERMIT_JOINING_request;
            }
            else
            {
                #if defined(USE_DEBUG)
                printf("Router start Failed: " );
                PrintChar( params.NLME_JOIN_confirm.Status );
                printf(". We cannot route frames\r\n" );
                #endif
                currentPrimitive = NLME_START_ROUTER_request;//request again
            }
            break;

        case NLME_PERMIT_JOINING_confirm:
            if (!params.NLME_PERMIT_JOINING_confirm.Status)
            {
                #if defined(USE_DEBUG)
                printf("Current Network Address is: ");
                PrintChar(macPIB.macShortAddress.v[1]);
                PrintChar(macPIB.macShortAddress.v[0]);
                printf("\r\n");
                #endif
                if(macPIB.macShortAddress.Val != 0x0001)
                {
                    // Thiet lap lai dia chi mang cua Router_EMB @dat_a3cbq91
                    macPIB.macShortAddress.v[1] = MyNetworkAddrMSB;
                    macPIB.macShortAddress.v[0] = MyNetworkAddrLSB;
                    // Setup ShortAddr on Transceiver !
                    PHYSetShortRAMAddr(0x03, macPIB.macShortAddress.v[0]);
                    PHYSetShortRAMAddr(0x04, macPIB.macShortAddress.v[1]);
                    #if defined(USE_DEBUG)
                    printf("Network Address has just re-assigned is 0x0001\r\n");
                    #endif
                }
                currentPrimitive = NO_PRIMITIVE;
            }
            else
            {
                #if defined(USE_DEBUG)
                printf("Join permission Failed:\r\n " );
                PrintChar( params.NLME_PERMIT_JOINING_confirm.Status );
                printf(". We cannot allow joins" );
                #endif
                currentPrimitive = NO_PRIMITIVE;
            }
            break;

        case NLME_JOIN_indication:
            #if defined(USE_DEBUG)
                printf( "Node " );
                PrintWord(params.NLME_JOIN_indication.ShortAddress.Val);
                printf( " With MAC Address " );
                //@dat_a3cbq91: Print MAC address of node has just joined
                for(i = 7; i ^ 0xFF; --i)
                {
                    PrintChar(params.NLME_JOIN_indication.ExtendedAddress.v[i]);
                }
                printf( " just joined.\r\n" );
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

                if(LookupAPSAddress(&params.NLME_JOIN_indication.ExtendedAddress) )
                {

                    for( i = 0; i < apscMaxAddrMapEntries; i++)
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
                        // no need to ON deviceAddress, since it is overlap with NLME_JOIN_indication
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
                #if defined(USE_DEBUG)
                if(!memcmppgm2ram( &params.NLME_LEAVE_indication.DeviceAddress, &myLongAddr, 8 ))
                {
                    printf("We have left the network\r\n" );
                }
                else
                {
                    printf("Another node has left the network\r\n" );
                }
                #endif
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
            #if defined(USE_DEBUG)
            printf("ZigBee Stack has been reset\r\n" );
            #endif
            /* For Zigbee 2006 The Specs require that node needs to
             * try joining as an orphan first after each reset,
             * see Mandatory test 3.9
            */
            ZigBeeStatus.flags.bits.bTryOrphanJoin = ON;

            currentPrimitive = NO_PRIMITIVE;
            break;

        case NLME_LEAVE_confirm:
            #if defined(USE_DEBUG)
            printf("Leaving the Zigbee network: \r\n" );
            PrintChar(params.NLME_LEAVE_confirm.Status);
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSDE_DATA_indication:
            {
                currentPrimitive = NO_PRIMITIVE;
                BYTE data;
                BYTE MACAddr;

                switch (params.APSDE_DATA_indication.DstEndpoint)
                {
                    // ************************************************************************
                    // Place a case for each user defined endpoint.
                    // ************************************************************************
                    case WSAN_Dst:
                    {
                        switch(params.APSDE_DATA_indication.ClusterId.Val)
                        {
                            case STATE_NODE_CLUSTER:
                                #if defined(USE_LED)
                                    LED2 = ~LED2;
                                #endif
                                MACAddr = APLGet();//MAC address of MiniRadar
                                MACAddr = MACAddr - 0x30;
                                data = APLGet();
                                switch(data)
                                {
                                    case DetectingByMicroWave:
                                        UART2Put(MACAddr);//Send to Embeded board
                                        //PrintChar(MACAddr);
                                        break;

                                    case DetectingByPIR:
                                        UART2Put(MACAddr);//Send to Embeded board
                                        //PrintChar(MACAddr);
                                        break;
                                }
                                
                                break;

                            case JOIN_CONFIRM_CLUSTER:
                                #if defined(USE_LED)
                                    LED3 = ~LED3;
                                #endif
                                printf("#JN:");
                                //print Network address
                                PrintChar(params.APSDE_DATA_indication.SrcAddress.ShortAddr.byte.MSB);
                                PrintChar(params.APSDE_DATA_indication.SrcAddress.ShortAddr.byte.LSB);
                                //print MAC address
                                PrintChar(APLGet());
                                //printf("\r\n");
                                break;                                

                            default:
                                break;
                        }   /* switch 1*/

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
            #if defined(USE_DEBUG)
            if (params.APSDE_DATA_confirm.Status)
            {
                printf("Error sending message: ");
                PrintChar(params.APSDE_DATA_confirm.Status);
                printf("\r\n");
            }
            else
            {
                printf("Sending message OK!\r\n" );
            }
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case APSME_ADD_GROUP_confirm:
        case APSME_REMOVE_GROUP_confirm:
        case APSME_REMOVE_ALL_GROUPS_confirm:
            #if defined(USE_DEBUG)
            printf("Perform Group Operation\r\n" );
            #endif
            currentPrimitive = NO_PRIMITIVE;
            break;

        case NO_PRIMITIVE:
            if (AllowJoin &&!ZigBeeStatus.flags.bits.bNetworkJoined)
            {
                if (!ZigBeeStatus.flags.bits.bTryingToJoinNetwork)
                {
                    if (ZigBeeStatus.flags.bits.bTryOrphanJoin)
                    {
                        #if defined(USE_DEBUG)
                        printf("Trying to join network as an orphan...\r\n" );
                        #endif

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
                        #if defined(USE_DEBUG)
                        printf("Trying to join network as a new device!\r\n" );
                        #endif
                        params.NLME_NETWORK_DISCOVERY_request.ScanDuration          = 8;
                        params.NLME_NETWORK_DISCOVERY_request.ScanChannels.Val      = ALLOWED_CHANNELS;
                        currentPrimitive = NLME_NETWORK_DISCOVERY_request;
                    }
                }
            }
//            else
//            {
//
//                    // ************************************************************************
//                    // Place all processes that can send messages here.  Be sure to call
//                    // ZigBeeBlockTx() when currentPrimitive is ON to APSDE_DATA_request.
//                    // ************************************************************************
//            }
            break;

        default:
            #if defined(USE_DEBUG)
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
 * Discribe: xu ly cac cong viec khong lien quan den Zigbee.
 */
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
        SPI1CON1 = 0x013A;	// 0000 0001 0011 1010
        SPI1CON2bits.FRMEN = 0;
        SPI1STATbits.SPIROV = 0;		// ON Overflow bit
        SPI1STATbits.SPIEN = 1;		// ON the peripheral

        SPI2CON1 = 0x013A;	// 0000 0001 0011 1010
        SPI2CON2bits.FRMEN = 0;
        SPI2STATbits.SPIROV = 0;		// ON Overflow bit
        SPI2STATbits.SPIEN = 1;		// ON the peripheral

    #else
        SPI1CON1 = 0x013A;	// 0000 0001 0011 1010
        SPI1CON2bits.FRMEN = 0;
        SPI1STATbits.SPIROV = 0;		// ON Overflow bit
        SPI1STATbits.SPIEN = 1;		// ON the peripheral

        SPI2CON1 = 0x013A;	// 0000 0001 0011 1010
        SPI2CON2bits.FRMEN = 0;
        SPI2STATbits.SPIROV = 0;		// ON Overflow bit
        SPI2STATbits.SPIEN = 1;		// ON the peripheral
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

    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;

    //pins connect to leds is
    #if defined(USE_LED)
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;
    LED3_TRIS = 0;
    #endif

    RFIF = OFF;
    RFIE = ON;

    if (RF_INT_PIN == 0)
    {
        RFIF = ON;
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

void _ISR __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
    BYTE c;
    IFS1bits.U2RXIF = OFF;
    c = U2RXREG;
    UART1Put(c);
    # if defined(USE_LED)
        LED0 = ~LED0;
    #endif
}

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
        params.APSDE_DATA_request.TxOptions.Val = ON;
    #else
        params.APSDE_DATA_request.TxOptions.Val = OFF;
    #endif
    params.APSDE_DATA_request.TxOptions.bits.acknowledged = ON;
    params.APSDE_DATA_request.ClusterId.Val = ClusterID;

    currentPrimitive = APSDE_DATA_request;
}
