/* 
 * File:   Project_3_LCD.h
 * Author: ads6067
 *
 * Created on April 9, 2022, 3:12 PM
 */

#ifndef EE200_LCD_H
#define	EE200_LCD_H
	//declare visible functions
void Init_LCD_Module(void);
void Position_LCD_Cursor(int cell_num);
void Write_LCD_String(char char_Array[16]);

#endif

