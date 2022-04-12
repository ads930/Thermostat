/*
 * File:   Project_3_main.c
 * Author: ads6067
 * 
 * Created on April 9, 2022, 3:12 PM
 */
#define FCY 3685000UL

#include <p33EP64MC502.h>
#include <libpic30.h>
#include <xc.h>
#include "Project_3_LCD.h" //header file to include lcd functions

// set config bits
#pragma config ICS = PGD2 // Communicate on PGED2 (pin 14) PGEC2 (pin 15)
#pragma conifg JTAGEN = OFF // Disable jtag to use RB8 through RB11
#pragma config OSCIOFNC = ON //allows pin 10 to function as an IO port

/* Declare functions */
void INITIALIZE_IO_PORTS(void);
void INITIALIZE_INT(void);
void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt(void);

int16_t in;
typedef enum { // define all 4 states
    S0, S1, S2, S3
} STATES;

STATES state = S0; // set s0 to default state

void INITIALIZE_IO_PORTS(void) {
    /* Configure pins 2 and 3 for digital I/O */
    ANSELAbits.ANSA0 = 0; // use pin 2 for digital I/O 
    ANSELAbits.ANSA1 = 0; // use pin 3 for digital I/O
    ANSELBbits.ANSB0 = 0; // use pin 4 for digital I/O
    ANSELBbits.ANSB1 = 0; // use pin 5 for digital I/O
    ANSELBbits.ANSB2 = 0; // use pin 6 for digital I/O
    ANSELBbits.ANSB3 = 0; // use pin 7 for digital I/O
    ANSELAbits.ANSA4 = 0; // use pin 12 for digital I/O
    
    
    /* Set I/O pin direction */
    TRISAbits.TRISA0 = 1;  // configure RA0 (pin 2) for input, B
    TRISAbits.TRISA1 = 1;  // configure RA1 (pin 3) for input, C
    TRISAbits.TRISA2 = 1; // configure RA2 (pin 9) for input, H
    TRISAbits.TRISA4 = 1; //pin 12 RESET input
    TRISAbits.TRISA3 = 0; //CLK Output (set as input by default)
    
    TRISBbits.TRISB0 = 0; // Set Y3 output
    TRISBbits.TRISB1 = 0; // Set Y output
    TRISBbits.TRISB2 = 0; // Set G output
    TRISBbits.TRISB3 = 0; // Set R output      
    
  }

void INITIALIZE_INT()
{
    RPINR0bits.INT1R = 0x14; // Assign INT1 to RPI20 (PIN 12) - 14 is hex for 20
    INTCON2bits.GIE = 1; //enable global interrupt
    INTCON2bits.INT1EP = 1; 
    IFS1bits.INT1IF = 0; //clear interrupt flag
    IEC1bits.INT1IE = 1; //enable interrupt
}


void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt(void){
    /* Write S0 to LCD*/
    char state0[] = "S0 - Standby";
    Position_LCD_Cursor(0x00); // place cursor at cell 0x00
    Write_LCD_String(state0); 
    state = S0;             // Set state to S0
    LATBbits.LATB0 = 1; //Y3 to high
    LATBbits.LATB1 = 0; 
    LATBbits.LATB2 = 0;
    LATBbits.LATB3 = 0;     
    IFS1bits.INT1IF = 0;    // Clear interrupt flag
} 

int main(void) {
        
    int B,C,H,reset;
    uint16_t input; //input it 16-bit unsigned integer
    char state0[] = "S0";
    char state1[] = "S1";
    char state2[] = "S2";
    char state3[] = "S3";

    Init_LCD_Module();
    INITIALIZE_IO_PORTS();
    INITIALIZE_INT();

    while (1) {// infinite loop 
       reset = PORTAbits.RA4 ^ 1; // read reset
       B = PORTAbits.RA0 ^ 1;  // read B
       C = PORTAbits.RA1 ^ 1;   // read C
       H = PORTAbits.RA2 ^ 1; // read H
       input = (reset << 3)|(B << 2) | (C << 1) | (H); //use OR operator to make the input in the form of 4*B+2*A+B
       //Reset is bit shifted to prevent case switching when reset and other inputs are held down together

       switch (state) {// determine next state
            case S0:
                switch (input) {
                    case 0: 
                    case 1: 
                    case 2: 
                    case 3: state = S3; break;                   
                    case 4: 
                    case 7: state = S0; break;
                    case 5: state = S1; break;
                    case 6: state = S2; break;
                    
                }; break;
            case S1:
                switch (input) {
                    case 0:
                    case 1: 
                    case 2: 
                    case 3: state = S3; break;
                    case 4: state = S0; break;
                    case 5: 
                    case 7: state = S1; break;
                    case 6: state = S2; break;                   
                };  break;
           case S2:
               switch (input) {
                    case 0:
                    case 1: 
                    case 2: 
                    case 3: state = S3; break;
                    case 4: state = S0; break;
                    case 5: state = S1; break;                    
                    case 6:
                    case 7: state = S2; break;
        }; break;
           case S3:
               switch (input) {
                    case 0:
                    case 1: 
                    case 2: 
                    case 3: state = S3; break;
                    case 4: 
                    case 5:                 
                    case 6:
                    case 7: state = S0; break;
        }; break;
       }
        
        switch (state) {// set output
            case S0: 
                LATBbits.LATB0 = 1; 
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                LATBbits.LATB3 = 0;
                Position_LCD_Cursor(0x00);
                Write_LCD_String(state0);
                break;
            case S1: 
                LATBbits.LATB0 = 0;
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                LATBbits.LATB3 = 1;
                Position_LCD_Cursor(0x00);
                Write_LCD_String(state1);
                break;
            case S2:
                LATBbits.LATB0 = 0;
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 1;
                LATBbits.LATB3 = 0;
                Position_LCD_Cursor(0x00);
                Write_LCD_String(state2);
                break;
            case S3:
                LATBbits.LATB0 = 0;
                LATBbits.LATB1 = 1;
                LATBbits.LATB2 = 0;
                LATBbits.LATB3 = 0;
                Position_LCD_Cursor(0x00);
                Write_LCD_String(state3);
                break;
        }
        __delay_ms(1500); //wait 1.5 seconds
        LATAbits.LATA3 = 0; // CLK LED is OFF
        ClrWdt(); // restart watchdog timer
        __delay_ms(1500); // wait 1.5 seconds
        LATAbits.LATA3 = 1; // CLK LED is ON

    }
    return 0;
    }
