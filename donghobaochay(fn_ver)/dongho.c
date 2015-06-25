/* 
   Mach actor thuc hien nhiem vu canh bao chay theo 
   _ tin hieu dieu khien qua uart giao tiep voi node mang zigbee
   _ hien thi gia tri ra led
   _ quay servo theo vi tri dinh san
   _ bat coi bao khi tin hieu dat nguong   
  */
#include "definition.h"
#pragma config FOSC=INTIO67,WDTEN=OFF,LVP=OFF,PBADEN=OFF,MCLRE=ON  // cau hinh dao dong noi
//char led7[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
//char led[]= {0x00,0x01,0x02,0x04,0x08,0x30};
char Led7, VITRI;
char EnableProcess = 0, command;
char alarm;

void Interrupt_service();

void main()
{
    OSCCONbits.IRCF = 0b011;//1 MHz

    TRISB = 0x00;
    TRISA = 0x00;
    TRISDbits.TRISD7 = 0; // chan dieu khien coi bao

    alarm = 0;

    Led7 = 0x03;
    Init_UART();
//    Init_Tran_UART();
//    Init_Rec_UART();

    //initialize UART module
//    OpenUSART(USART_TX_INT_OFF &
//            USART_RX_INT_ON &
//            USART_ASYNCH_MODE &
//            USART_EIGHT_BIT &
//            USART_BRGH_HIGH &
//            BAUD_16_BIT_RATE,
//            12);

    INTCONbits.PEIE = 1;//Peripheral interrupt enable
    INTCONbits.GIE = 1; // Golbal interrupt enable
    Pwm_init();

    VITRI = VITRI_3;

    while(1)
    {
        LATAbits.LATA0 = Led7;// lay 4 bit chua gia tri cua Led7 truyen sang 7447, tranh chan ngat int0
        LATBbits.LATB1 = (Led7 >>1);
        LATBbits.LATB2 = (Led7 >>2);
        LATBbits.LATB3 = (Led7 >>3);
        LATDbits.LATD7 = alarm;

        SetDCPWM2(VITRI);

        if(EnableProcess)
        {
            Processing();
            EnableProcess = 0;
        }
    }
    //while(1);
}
#pragma code interrupt_vector=0x08
void interrupt_vector()
{
    _asm
        goto Interrupt_service
    _endasm
}
#pragma interrupt Interrupt_service
void Interrupt_service()
{
    command = RCREG;
    EnableProcess = 1;
    PIR1bits.RCIF = 0; // xoa co ngat nhan uart
}

void Processing()
{
    switch(command)
    {
	case 0x81:
            Led7 = 0b0001;
//            LATDbits.LATD7=0;
            alarm = 0;
            VITRI = VITRI_5;	// servo quay nguoc	neu quay xuoi doi vi tri 5 voi 1 doi xung
            Tran_UART(0x81);    // tra lai gia tri cho node zigbee
            while (!PIR1bits.TXIF);
            break;

        case 0x82:
            Led7 = 0b0010;
//	    LATDbits.LATD7=0;
	    alarm = 0;
	    VITRI = VITRI_4;
	    Tran_UART(0x82);
	    while (!PIR1bits.TXIF);
            break;

        case 0x83:
	    Led7 = 0b0011;
	    //LATDbits.LATD7=0;
            alarm = 0;
            VITRI = VITRI_3;
	    Tran_UART(0x83);
	    while (!PIR1bits.TXIF);
            break;

    	case 0x84:
  	    Led7 = 0b0100;
  	    //LATDbits.LATD7=0;
  	    alarm = 0;
  	    VITRI = VITRI_2;
            Tran_UART(0x84);
            while (!PIR1bits.TXIF);
            break;

	case 0x85:
	    Led7 = 0b0101;
	    alarm = 1;
	    //LATDbits.LATD7=1;
            VITRI = VITRI_1;
	    Tran_UART(0x85);
            while (!PIR1bits.TXIF);
            break;

	default:  Tran_UART(0xAA); // tin hieu tra lai bao loi
            break;
    }
}
