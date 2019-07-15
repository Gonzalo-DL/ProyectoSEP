/*
 * IncFile1.h
 *
 * Created: 01-07-2019 23:33:56
 *  Author: Gonzalo
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

void LCD_write_instruction(int instr);
void LCD_init();
void LCD_set_add(int add);

void LCD_write_string(char* str,int add,int clear);
void LCD_write_char(char c);
#endif /* INCFILE1_H_ */