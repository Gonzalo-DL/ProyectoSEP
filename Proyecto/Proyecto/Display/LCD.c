/*
 * CFile1.c
 *
 * Created: 01-07-2019 23:33:50
 *  Author: Gonzalo
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"


void LCD_write_instruction(int instr)
{
	_delay_ms(1);
	PORTC &= ~0x07; // tiramos a 0 RS R/W y E
	//instrucciones
	PORTB = (PORTB&0xC0) + (instr&0x3F);// solo modificamos valor bits 0-5
	PORTD = (PORTD&0x3F) + (instr&0xC0); // solo modificamos valor bits 6-7
	//
	PORTC |= 0x04; // E
	_delay_ms(1);
	PORTC &= ~0x04; // !E
	
}

void LCD_init()
{
	DDRC |= 0x07; // RS, R/W, E
	DDRB |= 0x3F; // BITS 0..5 -- PB0..5
	DDRD |= 0xC0; // BITS 6..7 -- PD6..7
	
	LCD_write_instruction(0x01); //CLEAR
	LCD_write_instruction(0x02); //return home
	LCD_write_instruction(0x06); // increment mode
	LCD_write_instruction(0x38); //16x2 format
	LCD_write_instruction(0x0C); // 1,ON,CURSOR,BLINK--0000
	
}

void LCD_set_add(int add)
{
	LCD_write_instruction(0x80+add);
}

void LCD_write_string(char* str,int add,int clear)
{
	char *ptr;
	ptr=str;
	if (clear==1)
	{
		LCD_write_instruction(0x01);
	}
	LCD_set_add(add);
	while(*ptr>0)
	{
		LCD_write_char(*ptr);
		ptr++;
	}
	
}

void LCD_write_char(char c)
{
	PORTC |= 0x05;
	PORTC &= ~0x02;
	PORTB = (PORTB&0xC0) + (c&0x3F);// solo modificamos valor bits 0-5
	PORTD = (PORTD&0x3F) + (c&0xC0); // solo modificamos valor bits 6-7
	_delay_ms(1);
	PORTC &= ~0x04;
}