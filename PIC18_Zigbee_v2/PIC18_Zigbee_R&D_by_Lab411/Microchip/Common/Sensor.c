// Sensor.c
#include "Sensor.h"
#include <p18cxxx.h>

/*********************************************************************
 * Function:        int returnADC( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Return Temperature from LM35
 *
 * Overview:        None
 * Note:            Function: t = adc_val/10
                                        VREF+ = 1.024V
 ********************************************************************/

unsigned int VoltageIs() {
/*

    float temp;
    unsigned long sum = 0;
    unsigned int adc;
    unsigned char i;
    TRISAbits.TRISA0 = 1; // Set RA0 input Analog
    ADCON2 = 0xAF; // Right justify, FRC, 12 TAD ACQ time
    ADCON1 = 0x00; // ADC ref = VDD-VSS
    ANSEL = 0x01; // Disable Digital input buffer on RA0, RA0 is Analog input
    ADCON0 = 0x01; // Select AN0, ADC on
    DelayUs(5);
    for (i = 0; i < 10; i++) {
        ADCON0bits.GO = 1; // Start Converstation
        adc = 0;
        while (ADCON0bits.GO); // Complete ?
        adc = ADRESH;
        adc = adc << 8 + ADRESL;
        sum += adc;

    }

    TRISAbits.TRISA0 = 0; // Close ADC
    LATAbits.LATA0 = 0;
    ADCON0 = 0x00; // Disable ADC
    ANSEL = 0xFF; // Enable Digital Input buffer on RA0

    temp = 20480 / sum;
    return ((unsigned int) (temp * 10));
*/
    return 1;
}
