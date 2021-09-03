/*
 * File:   main_proyectoMaster.c
 * Author: acer
 *
 * Created on 24 de agosto de 2021, 07:23 PM
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
//*****IMPORTACION DE LIBRERIAS******
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pic16f887.h>
#include "I2CP_Master.h"
#include "LCDP_Master.h"
#include "UARTP_Master.h"
#include <xc.h>
#define _XTAL_FREQ 4000000
uint8_t contador = 0;       /*Declaración variables*/
int var1 = 0;
int var2 = 0;
int POT;
uint8_t BanStart = 0;      //Variable para start 
int contRecipiente = 0;
int datUltrasonico = 0;
float v1 = 0;
float v2 = 0;
float v3 = 0;
int POT;
char s[20];
//***********Prototipos de funciones************
void setup(void);           /*funcion principal */
void valorsensores(void);
void configuracionUART(void);
void com_master(void);
//**************Interrupciones**************
void __interrupt()isr(void) /*interrupciones*/
{
    if(RBIF == 1)           //Bandera IOCB
    {
        if(RB0 == 0)        //Si boton en RB0 esta presionado
        {
            BanStart = 1;        //Aumenta contador del PORTA
            PORTAbits.RA0 = 1; 
        }
    }
    INTCONbits.RBIF = 0;
}
//*********************************funcionPrincipal**********
void main(void) {
    setup();                /*funcion de configuracion principal*/
    unsigned int a;
    TRISD = 0x00;           //PUERTO LCD
    Lcd_Init();             //Inicialización para 8 bits
    //*********************************LoopPrincipal*********
    while(1)
    {
        com_master();
        if (datUltrasonico < 5)
        {
            PORTEbits.RE0 = 1;
        }
        else
        {
            PORTEbits.RE0 = 0;
        }
        //*******LCD***********//             
        sprintf(s, "%.3dcm", datUltrasonico);        //convertir valor float a caracteres
        Lcd_Set_Cursor(2,1);            //Posicion LCD
        Lcd_Write_String(s);            //Hacia pantall
        //Lcd_Clear();
        Lcd_Set_Cursor(1,2);            //Posicion LCD
        Lcd_Write_String("S1");         //Caracter en pantalla
        Lcd_Set_Cursor(1,8);            //Posicion LCD
        Lcd_Write_String("S2");         //Mostrar S2 en pantalla
        Lcd_Set_Cursor(1,14);
        Lcd_Write_String("S3");
        
        sprintf(s, "%.2dUni", contRecipiente);        //convertir valor float con 2 decimales
        Lcd_Set_Cursor(2,7);            //posicion cursor
        Lcd_Write_String(s);            //
        
        sprintf(s, "%.3d", POT);     //convertir valor entero a caracter
        Lcd_Set_Cursor(2,14);           //Posicion LCD
        Lcd_Write_String(s);
        __delay_ms(80);
        valorsensores();            //TRANSMISIONDATOS_UART
        //***********FINISH****************//
    }
}
//*************Funciones************************
void setup(void)
{ 
    //*******configuración io entradas y salidas****
    ANSEL = 0b00000000;
    ANSELH = 0x00;     //Salidas Digitales
    
    TRISA = 0x00;
    TRISB = 0b00000001;
    TRISE = 0x00;
    TRISBbits.TRISB0 = 1; 
    OPTION_REGbits.nRBPU = 0; 
    WPUB = 0b00000001;      //Habilitar Pull interno
    PORTA = 0x00; 
    PORTE = 0x00;
    
    I2C_Master_Init(100000);
    //configuración del reloj (oscilador interno)
    OSCCONbits. IRCF2 = 1; 
    OSCCONbits. IRCF1 = 1;
    OSCCONbits. IRCF0 = 0;  //4Mhz
    OSCCONbits. SCS = 1;
    //configuración Tmr0
    configuracionUART();
    //configuración IOC
    IOCBbits. IOCB0 = 1; 
    //IOCBbits. IOCB1 = 1;
    //configuración interrupción
    INTCONbits. GIE = 1;        //Globales
    INTCONbits. RBIE = 1;       //IOCB
    INTCONbits. RBIF = 0;
    INTCONbits.PEIE = 1;        //Perifericas
}
void configuracionUART(void)
{
    TXSTAbits.SYNC = 0;             //Modo asíncrono
    TXSTAbits.BRGH = 1;             //Seleccion BAUD RATE
    BAUDCTLbits.BRG16 = 0; 
    
    SPBRG = 25;                     //Registros para valor BAUD RATE
    SPBRGH = 0; 
    
    RCSTAbits.SPEN = 1;         //Habilitar puerto serial asíncrono
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;         //Habilitar recepción de datos 

    TXSTAbits.TXEN = 1;         //Habilitar transmision
}
void com_master(void)
{
    I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
    I2C_Master_Write(0x50); //cargamos registro 101 0000 (escribir))
    I2C_Master_Write(BanStart);    //byte de informacion
    I2C_Master_Stop();  //transmision de datos terminado
     __delay_ms(200);
       
    I2C_Master_Start();
    I2C_Master_Write(0x51);     //cargamos misma direccion pero modo lectura
    POT = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(200);  
    /*****ESCLAVOINFRARROJO*/
    I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
    I2C_Master_Write(0x20); //cargamos registro 101 0000 (escribir))
    I2C_Master_Write(BanStart);    //byte de informacion
    I2C_Master_Stop();  //transmision de datos terminado
     __delay_ms(200);
       
    I2C_Master_Start();
    I2C_Master_Write(0x21);     //cargamos misma direccion pero modo lectura
    contRecipiente = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(200);
    /********EsclavoULTRASONICO*/
    I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
    I2C_Master_Write(0x40); //cargamos registro 101 0000 (escribir))
    I2C_Master_Write(BanStart);    //byte de informacion
    I2C_Master_Stop();  //transmision de datos terminado
     __delay_ms(200);
       
    I2C_Master_Start();
    I2C_Master_Write(0x41);     //cargamos misma direccion pero modo lectura
    datUltrasonico = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(200);
}