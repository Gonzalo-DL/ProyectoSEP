#include <avr/io.h>
#include <avr/interrupt.h>

#include "Timer_include.h"

void init_timer1() // Timer adquisicion 16bit
{
	TCCR1A = (1<<WGM11)+(1<<WGM10); //
	TCCR1B = (1<<CS12)+(1<<CS10)+(1<<WGM12)+(1<<WGM13); // clk/1024. OCR1A
	TIMSK1 = (1<<TOIE1); // enableamos interrupt de OVFLW de timer 2
	OCR1A = 0xFFFF; // aprox 4 s
	
}

void init_timer0() // Timer AM2302 8bit
{
	TCCR0A = (1<<WGM01)+(1<<WGM00); //
	TCCR0B = (1<<CS01)+(0<<CS00); //  clk/8
	TIMSK0 = 0; // no enableamos interrupt de OVFLW de timer 0
}

void init_timer2() // Timer AM2302 8bit
{
	TCCR2A = (1<<WGM01)+(1<<WGM00); // fastpwm
	TCCR2B = (0<<CS22)+(0<<CS21)+(0<<CS20); // clk/1024 pero detenido
	TIMSK2 = (0<<TOIE2); // no enableamos interrupt de OVFLW de timer 2
}