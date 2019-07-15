/***************************************************
 * This is a USART library for the ATMega328P.
 *
 * It can provide basic USART (serial) communication for any application.
 *
 * This code is in the public domain. Feel free to do with it whatever you want.
 * 
 * 
 * 
 * FOR STUDENTS:
 * 
 * This file has an incomplete struct definition. Add definitions for its
 * members! Check the other files and the lab description to find out which
 * parameters and what ranges are needed.
 * 
 ****************************************************/

#ifndef _USART_IMPLEMENT_ME_H_
#define _USART_IMPLEMENT_ME_H_

#include <stdint.h>

/*You can use it, but isn't mandatory*/



struct USART_configuration
{
	uint16_t baud_rate;
	uint8_t data_bits;
	uint8_t paridad;
	uint8_t stop_bits;
};



// Call once to initialise USART communication
/* You can use struc, but isn't mandatory*/
uint8_t USART_Init(struct USART_configuration config);

// Transmits a single character
void USART_Transmit_char(uint8_t data );

// Transmits a given string
void USART_Transmit_String(char* string);





#endif // _USART_IMPLEMENT_ME_H_