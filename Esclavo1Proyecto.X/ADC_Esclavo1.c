/*
 * File:   ADC_Esclavo1.c
 * Author: acer
 *
 * Created on 23 de agosto de 2021, 11:00 PM
 */


#include <xc.h>
#define _XTAL_FREQ 4000000
#include <stdint.h>
#include <pic16f887.h>
#include "ADC_Esclavo1.h"
uint8_t var1;
uint8_t var2;
void recibir_valoresADC(void)
{
    if (PIR1bits.ADIF == 1)
    {
            var1 = ADRESH;
            //PIR1bits.ADIF = 0;
    }
    PIR1bits.ADIF = 0;
}
void inicio_conversionADC(void)
{
    if(ADCON0bits.GO == 0)
    {
        /*f(ADCON0bits.CHS == 5)
        {
            ADCON0bits.CHS = 6;
        }
        else
        {
            ADCON0bits.CHS = 5;
        }
        //__delay_us(50);
    }*/
    __delay_us(50);
    ADCON0bits.GO = 1; 
    }
}
