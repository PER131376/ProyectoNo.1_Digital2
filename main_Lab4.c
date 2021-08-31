/*
 * File:   main_Lab4.c
 * Author: acer
 *
 * Created on 10 de agosto de 2021, 10:29 AM
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
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
//*****************************************************************************
// Definiciï¿½n e importaciï¿½n de librerï¿½as
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pic16f887.h>
#include "I2C_L3.h"
#include "LCD8L3.h"
#include <xc.h>
#define _XTAL_FREQ 4000000
//*****************************************************************************
// Definiciï¿½n de variables
//*****************************************************************************
uint8_t contador = 0b00001111;
uint16_t VARIABLE;
int POT = 0;
int cont = 0;
int tempF = 0;
int cont4 = 0;
float v1 = 0;
float v2 = 0;
float v3 = 0;
char s[20];
//*****************************************************************************
// Definiciï¿½n de funciones para que se puedan colocar despuï¿½s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
void pantalla(void);
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    unsigned int a;
    TRISD = 0x00;           //PUERTO LCD
    Lcd_Init();             //InicializaciÃ³n para 8 bits
    while(1){
        //uint8_t contador = 0b00001111;
        I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
        I2C_Master_Write(0x50); //cargamos registro 101 0000 (escribir))
        I2C_Master_Write(contador);    //byte de informacion
        I2C_Master_Stop();  //transmision de datos terminado
       __delay_ms(200);
       
        I2C_Master_Start();
        I2C_Master_Write(0x51);     //cargamos misma direccion pero modo lectura
        PORTA = I2C_Master_Read(0);
        POT = PORTA;
        I2C_Master_Stop();
        __delay_ms(200); 
        /********SLAVE2****************/
        I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
        I2C_Master_Write(0x40); //cargamos registro 101 0000 (escribir))
        I2C_Master_Write(contador);    //byte de informacion
        I2C_Master_Stop();  //transmision de datos terminado
        __delay_ms(200);
       
        I2C_Master_Start();
        I2C_Master_Write(0x41);     //cargamos misma direccion pero modo lectura
        cont = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        /************************DS1621***************************************/
        I2C_Master_Start();    //Inicia la transmis de datos en SDA SEN = 1
        I2C_Master_Write(0x90); //cargamos registro 101 0000 (escribir))
        I2C_Master_Write(0xEE);    //byte de informacion
        I2C_Master_Stop();  //transmision de datos terminado
        //__delay_ms(200);
       
        I2C_Master_Start();
        I2C_Master_Write(0x90);     //cargamos misma direccion pero modo lectura
        I2C_Master_Write(0xAA);     //Comando leer temperatura
        I2C_Master_Stop();
        //__delay_ms(200);
        //I2C_Master_Write(0x91);
        I2C_Master_Start();
        I2C_Master_Write(0x91);
        VARIABLE = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        pantalla();
    }
    return;
}
//*****************************************************************************
// Funciï¿½n de Inicializaciï¿½n
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    
    PORTA = 0;
    
    I2C_Master_Init(100000);        // Inicializar Comuncaciï¿½n I2C
    //configuraciÃ³n del reloj (oscilador interno)
    OSCCONbits. IRCF2 = 1; 
    OSCCONbits. IRCF1 = 1;
    OSCCONbits. IRCF0 = 0;  //4Mhz
    OSCCONbits. SCS = 1;
}
void pantalla(void)
{      
    if(VARIABLE >= 201 & VARIABLE <=255)
    {
        tempF = 256 - VARIABLE;
        Lcd_Set_Cursor(1,8);            //Posicion LCD
        Lcd_Write_String("S2");
        sprintf(s, "-%.2dC", tempF);     //convertir valor entero a caracter
        Lcd_Set_Cursor(2,8);           //Posicion LCD
        Lcd_Write_String(s);
    }
    else{
        tempF = VARIABLE;
        Lcd_Set_Cursor(1,8);            //Posicion LCD
        Lcd_Write_String("S2");
        sprintf(s, "%.3dC", tempF);     //convertir valor entero a caracter
        Lcd_Set_Cursor(2,8);           //Posicion LCD
        Lcd_Write_String(s);
    }

        cont4 = cont & 0x0F;
    /****************PANTALLALCD***************************/
        Lcd_Set_Cursor(1,2);            //Posicion LCD
        Lcd_Write_String("POT");
        v1 = (POT*0.0196);             //Operacion para rango 0-5v             
        sprintf(s, "%.2fv", v1);        //convertir valor float a caracteres
        Lcd_Set_Cursor(2,1);            //Posicion LCD
        Lcd_Write_String(s);
        
        Lcd_Set_Cursor(1,13);            //Posicion LCD
        Lcd_Write_String("CONT");
        sprintf(s, "%.2d", cont4);     //convertir valor entero a caracter
        Lcd_Set_Cursor(2,14);           //Posicion LCD
        Lcd_Write_String(s);
}