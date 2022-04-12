/*
 * File:   Project_3_LCD.c
 * Author: ads6067
 *
 * Created on April 9, 2022, 3:11 PM
 */


#define FCY 3685000UL

#include <p33EP64MC502.h>
#include <libpic30.h>
#include <xc.h>        // include header to access strlen
#include <string.h>    //access strlen

/* define constants for programming the LCD module */
#define pgm_delay_ms 2
#define INSTRUCTION 0
#define DATA 1

/* declare local functions */
void Init_LCD_DIO_Port (void);
void Toggle_Enable_line (void);
void Write_LCD_Nibble(int data, int cmd);
void Write_LCD_Byte(int data, int cmd);

/* declare global functions */
void Init_LCD_Module(void);
void Position_Cursor(int cell_num);
void Write_LCD_String(char char_Array[16]);

void Init_LCD_DIO_Port (void) {
    ANSELB = 0;     // use peripheral pins associated with PORTB for digital I/O
    TRISB = 0xC0FF; // set RB8 to RB13 for output, the rest for input; 0xC0FF == 0b1100000011111111
    PORTBbits.RB13 = 0;      // set some LCD inputs low
    PORTBbits.RB12 = 0;
    PORTBbits.RB11 = 0;
    PORTBbits.RB10 = 0;
    PORTBbits.RB9 = 0;
    PORTBbits.RB8 = 0;
  }

void Toggle_Enable_line (void) {
    __delay_ms(pgm_delay_ms);   
    PORTBbits.RB13 = 1;        
    __delay_ms(pgm_delay_ms);  
    PORTBbits.RB13 = 0;  
    __delay_ms(pgm_delay_ms);  
}

void Write_LCD_Nibble(int data, int cmd) {
    PORTB =  (PORTB & 0xC0FF) | data << 8;     // place nibble at LCD inputs DB4 through DB7
    PORTBbits.RB12 = cmd;      // set RS; cmd = 0 for instruction, 1 for data
    Toggle_Enable_line ();     // strobe data into LCD module
    PORTBbits.RB13 = 0;        // set certain lcd inputs to low
    PORTBbits.RB12 = 0;
    PORTBbits.RB11 = 0;
    PORTBbits.RB10 = 0;
    PORTBbits.RB9 = 0;
    PORTBbits.RB8 = 0;
}

void Write_LCD_Byte(int data, int cmd) {
    Write_LCD_Nibble((data & 0x00F0) >> 4, cmd); // write upper nibble
    Write_LCD_Nibble(data & 0x000F, cmd);       // write lower nibble
}

void Init_LCD_Module(void) {
    Init_LCD_DIO_Port();                    // Configure RB8 through RB13 for digital output
    Write_LCD_Nibble(0b0011, INSTRUCTION);  // Initialize the LCD Module
    Write_LCD_Nibble(0b0011, INSTRUCTION);
    Write_LCD_Nibble(0b0011, INSTRUCTION);
    Write_LCD_Nibble(0b0010, INSTRUCTION);  // invoke 4-bit mode
    Write_LCD_Byte(0b00101000, INSTRUCTION);// 4-bit mode, two-line,5X7 dot
    Write_LCD_Byte(0b00000001, INSTRUCTION);// clear display, cursor at 0x00
    Write_LCD_Byte(0b00001100, INSTRUCTION);// display on, cursor off
}

void Position_LCD_Cursor(int cell_num) {
    Write_LCD_Byte(0x80 + cell_num, INSTRUCTION);
}

void Write_LCD_String(char char_Array[16]) {
    int idx;

    for (idx = 0; idx < strlen(char_Array); idx++)
        Write_LCD_Byte(char_Array[idx], DATA);
}
