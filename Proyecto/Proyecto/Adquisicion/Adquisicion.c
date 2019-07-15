/*
 * Adquisicion.c
 *
 * Created: 05-07-2019 12:09:21
 *  Author: Gonzalo
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "Adquisicion.h"

void adquisicion_dht22(int PUERTO, uint16_t *temp, uint16_t *hum ) // que puerto de PD, tenemos 2, 3, 4 y 5
{
	int am2302_in[40];
	DDRD |= (1<<PUERTO); // PC0 como salida
	PORTD &= ~(1<<PUERTO); //PC0 a L
	_delay_ms(5);
	DDRD &= ~(1<<PUERTO);
	
	int i=0;
	while((PIND&(1<<PUERTO))) //20-40us
	{
		if(i>50)
		{
			return;
		}
		i++;
		_delay_us(1);
	}
	
	i=0;
	while(!(PIND&(1<<PUERTO))) // 80us
	{
		if(i>100)
		{
			return;
		}
		i++;
		_delay_us(1);
	}
	
	i=0;
	while((PIND&(1<<PUERTO))) // 80us
	{
		if(i>100)
		{
			return;
		}
		i++;
		_delay_us(1);
	}
	i=0;
	while (i<40)
	{
		int j=0;
		while(!(PIND&(1<<PUERTO)))
		{
			if (j>100)
			{
				return;
			}
			j++;
			_delay_us(1);
		}
		j=0;
		TCNT0=0;
		while((PIND&(1<<PUERTO)))
		{
			if (j>100)
			{
				return;
			}
			j++;
			_delay_us(1);
		}
		am2302_in[i]=TCNT0;
		
		i++;
	}
	DDRD |= (1<<PUERTO); // PC0 como salida
	PORTD |= (1<<PUERTO); //PC0 a L
	
	
	uint16_t hum_in=0;
	uint16_t temp_in=0;
	int sum=0;
	int check_sum=0;
	for (i=0; i<40; i++)
	{
		if(i<16 && am2302_in[i]>100)
		{
			sum = sum + (1<<(7-(i%8)));
			hum_in = hum_in + (1<<(16-i));
		} else if(i<32 && am2302_in[i]>100)
		{
			sum = sum + (1<<(7-(i%8)));
			temp_in = temp_in + (1<<(32-i));
		} else if(i<40 && am2302_in[i]>100)
		{
			check_sum = check_sum + (1<<(39-i));
		}
		
		
	}
	//if (sum!=check_sum) return;
	*temp = *temp/2+ temp_in/2;
	*hum = *hum/2 + hum_in/2;
}