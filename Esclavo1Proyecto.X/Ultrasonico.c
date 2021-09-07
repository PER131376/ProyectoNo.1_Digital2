#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Progra

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>
#include <xc.h>
#include "I2C_Esclavo1.h"
#define _XTAL_FREQ 4000000
#define trigger1 RD0
#define echo1    RD1
#define trigger2 RD2
#define echo2    RD3
int dist = 0;
int dist2 = 0;
uint8_t var;
int cualquiera = 0;
int distance = 0;
uint8_t z;
int calc_distance(void); // Function Declaration
int calc_dist2(void);

void setup (void);
void __interrupt() hola (void){
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            cualquiera = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            SSPSTATbits.BF = 0;
            SSPBUF = var;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
        }
    }

void main(void)
{
  // Create Distance Variable
  setup();
  
  while(1)
  {
    dist = calc_dist()/10;
    dist2 = calc_dist2()/10;
    
    if(dist==1)
    {PORTB = 0x01; __delay_ms(50);}
    if(dist==2)
    {PORTB = 0x03; __delay_ms(50);}
    if(dist==3)
    {PORTB = 0x07; __delay_ms(50);}
    if(dist==4)
    {PORTB = 0x0F; __delay_ms(50);}
    else
    {PORTB = 0x00; __delay_ms(50);}
    
    if(dist2==1)
    {PORTA = 0x01; __delay_ms(50);}
    if(dist2==2)
    {PORTA = 0x03; __delay_ms(50);}
    if(dist2==3)
    {PORTA = 0x07; __delay_ms(50);}
    if(dist2==4)
    {PORTA = 0x0F; __delay_ms(50);}
    else
    {PORTA = 0x00; __delay_ms(50);}
    
    if(dist >= 3 | dist2 >= 3){
        var = 1;
        PORTDbits.RD7 = var;
        
    }else{
        var = 0;
        PORTDbits.RD7 = var;
    }
  }
  return;
}

int calc_dist(void)
{
  TMR1=0;
  
  trigger1=1;
  __delay_us(10);
  trigger1=0;
  
  while(!echo1);
  TMR1ON=1;
  while(echo1);
  TMR1ON=0;
  
  distance=TMR1/58.82;
  return distance;
}

int calc_dist2(void)
{
  TMR1=0;
  
  trigger2=1;
  __delay_us(10);
  trigger2=0;
  
  while(!echo2);
  TMR1ON=1;
  while(echo2);
  TMR1ON=0;
  
  distance=TMR1/58.82;
  return distance;
}

void setup (void){
 
  TRISB = 0x00;
  PORTB = 0x00;
  TRISA = 0x00;
  PORTA = 0x00;
  TRISD0 = 0;
  RD0 = 0;
  TRISD1 = 1;
  TRISD2 = 0;
  RD2 = 0;
  TRISD3 = 1;
  TRISD7 = 0;
  RD7 =0;
//----------------------------configuracion_oscilador---------------------------
    OSCCONbits.IRCF2 = 1; 
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;  //4Mhz
    OSCCONbits.SCS = 1;
  
//----------------------------Timer_1-------------------------------------------
    T1CKPS0=0;
    T1CKPS1=0;
    TMR1CS=0;
//--------------------------------Configuracion del I2C-------------------------
    I2C_Slave_Init(0x40);

//-----------------------------------Configuracion_de_interrupciones------------
    INTCONbits. GIE = 1;        //Globales
    INTCONbits.PEIE = 1;        //Perifericas
    PIE1bits.SSPIE = 1;
    PIR1bits.SSPIF = 0;
}