/*
 * File:   main_Esclavo2.c
 * Author: acer
 *
 * Created on 2 de septiembre de 2021, 11:28 AM
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Progra
//****DirectivasCompliador*****//
#define _XTAL_FREQ 4000000
//*****IMPORTACION DE LIBRERIAS******
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h> 
#include "I2C_Esclavo2.h"
uint8_t z;
uint8_t dato;
int datRecibido; 
//***********Prototipos de funciones************
void setup(void);           /*funcion principal */
//**************Interrupciones**************
void __interrupt()isr(void) /*interrupciones*/
{
    if(RBIF == 1)           //Bandera IOCB
    {
        if(RB0 == 0)        //Si boton en RB0 esta presionado
        {
            PORTA++;        //Aumenta contador del PORTA
            PORTBbits.RB7 = 0;
        }
        else
        {
            //PORTBbits.RB7 = 1;
        } 
    }
    INTCONbits.RBIF = 0;
    
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0; //reloj bajo configuracion datos
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) { //recepcion de datos
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupciï¿½n recepciï¿½n/transmisiï¿½n SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepciï¿½n se complete
            datRecibido = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepciï¿½n
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){ //Envï¿½o de datos
            z = SSPBUF;
            BF = 0;
            SSPBUF = PORTA;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//*********************************funcionPrincipal**********
void main(void) {
    setup();                /*funcion de configuracion principal*/
                PORTBbits.RB7 = 1;
    //*********************************LoopPrincipal*********
    while(1)
    {
        /*****CondicionEmpaquetado*/
        if(PORTA > 6)
        {
            PORTA = 0x00; 
        }
        if(datRecibido > 200 & PORTBbits.RB7 == 0 )
        {
            CCPR2L = 0x3E; 
            CCP2CONbits.DC2B1 = 1; 
            CCP2CONbits.DC2B0 = 0;
        }
        else if(datRecibido < 5)
        {
            CCPR2L = 0x9C; 
            CCP2CONbits.DC2B1 = 0; 
            CCP2CONbits.DC2B0 = 1;
            PORTBbits.RB7 = 1;
        }
            
    }
}
//*************Funciones************************
void setup(void)
{ 
    //*******configuración io entradas y salidas****
    ANSEL = 0b00100000;
    ANSELH = 0x00;     //Salidas Digitales
    
    TRISA = 0x00; 
    TRISB = 0x01;    
    OPTION_REGbits.nRBPU = 0; 
    WPUB = 0b00000001;      //Habilitar Pull interno 
    PORTA = 0x00; 
    PORTB = 0x00; 
    I2C_Slave_Init(0x20);
    //configuración IOC
    IOCBbits. IOCB0 = 1; 
    //configuración del reloj (oscilador interno)
    OSCCONbits. IRCF2 = 1; 
    OSCCONbits. IRCF1 = 1;
    OSCCONbits. IRCF0 = 0;  //4Mhz
    OSCCONbits. SCS = 1;
    //****configuracionPWM**********
    TRISCbits.TRISC2 = 1;    // RC2/CCP1 encendido
    TRISCbits.TRISC1 = 1;    // RC1/CCP2 encendido
    
    PR2 = 255;               // Configurando el periodo 4.10ms
    CCP1CONbits.P1M = 0;     // Configurar el modo PWM
    
    CCP1CONbits.CCP1M = 0b1100;     //Modo PWM para pin CCP1
    CCP2CONbits.CCP2M = 0b1100;     //Modo PWM para pin CCP2
    
    CCPR1L = 0x0f;              //Definimos para valor inicial de Dutycicle
    CCPR2L = 0x0f;              //Definimos para valor inicial de Dutycicle
    
    CCP1CONbits.DC1B = 0;           //Bits menos significativos de ancho de pulso PWM
    CCP2CONbits.DC2B0 = 0;          //Valores inciales
    CCP2CONbits.DC2B1 = 0;
    
    PIR1bits.TMR2IF  =  0;
    T2CONbits.T2CKPS =  0b11;       //Preescaler 1:16
    T2CONbits.TMR2ON =  1;          //On Tmr2
    
    while(PIR1bits.TMR2IF  ==  0);  //Esperar un ciclo Tmr2
    PIR1bits.TMR2IF  =  0;          //Reset Bandera
    
    TRISCbits.TRISC2 = 0;           //Se resetean las salidas CCPx
    TRISCbits.TRISC1 = 0;
    //configuración interrupción
    INTCONbits. GIE = 1;        //Globales
    INTCONbits. RBIE = 1;       //IOCB
    INTCONbits. RBIF = 0;
    INTCONbits.PEIE = 1;        //Perifericas
    PIE1bits.SSPIE = 1;
    PIR1bits.SSPIF = 0;
}