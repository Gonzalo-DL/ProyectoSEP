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
 * This file will be given to you in an 'empty' state. The function bodies are
 * provided, but not their content. You are supposed to add the proper code
 * and complete these functions.
 * 
 * Hint 1: RTFM! The Atmel device datasheets contain all information necessary.
 * 
 * Hint 2: We do not expect you to write the most performant or exceptionally
 * well-engineered code, but it should work. If in doubt, sacrifice speed for
 * reliability. If you are bored, rewrite the entire library in assembler and
 * squeeze the last microsecond out of it.
 * 
 ****************************************************/


// includes
#define F_CPU	16000000UL		// MCU Clock Speed - needed for baud rate value computation



// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include "USART_implement_me.h"




// The initialisation function. Call it once from your main() program before
// issuing any USART commands with the functions below!
//
// Call it at any time to change the USART communication parameters.
//
// Returns zero in case of success, non-zero for errors.

/* You can use a struct, but isn't mandatory*/
uint8_t USART_Init(struct USART_configuration config)
{
	while(!(UCSR0A & (1<<UDRE0))); //Nos aseguramos que no este enviando nada antes de resetear el TxRx.
	
	UCSR0A |= (1<<U2X0); // prendemos esta opcion para aumentar precision
	UCSR0B = 0;		// UCSZ02 =0, para elegir 5-8 bits de data
	UCSR0C = 0;		// UMSEL0 = 00 , asincrono
	UBRR0=0;
	
	
	switch (config.data_bits)
	{
		case 5: UCSR0C |= 0;							// UCSZ0 = 00 -> 5
		break;
		case 6: UCSR0C |= (1<<UCSZ00);					// UCSZ0 = 01 -> 6
		break;
		case 7: UCSR0C |= (1<<UCSZ01);					// UCSZ0 = 10 -> 7
		break;
		case 8: UCSR0C |= (1<<UCSZ01) + (1<<UCSZ00);	// UCSZ0 = 11 -> 8
		break;
		default:PORTB |= 0x20;
		UCSR0C |= (1<<UCSZ01) + (1<<UCSZ00);			// UCSZ0 = 11 -> 8, si hay error se fija en 8.
		break;
	}
	
	switch (config.stop_bits)
	{
		case 1: UCSR0C |= 0;		//USBS0=0, 1 bit de stop
		break;
		case 2: UCSR0C |= (1<<USBS0);	//USBS0=1, 2 bits de stop
		break;
		default:PORTB |= 0x20;
		UCSR0C |= 0;				//En error USBS0=0, 1 bit de paridad.
		break;
	}
	
	switch (config.paridad)
	{
		case 0: UCSR0C |= 0;							//UPM0=00, sin paridad
		break;
		case 1: UCSR0C |= (1<<UPM01)+(1<<UPM00);		//UPM0=11, paridad impar
		break;
		case 2: UCSR0C |= (1<<UPM01);					//UPM0=10, paridad par
		break;
		default:PORTB |= 0x20;
		UCSR0C |= 0;									//En error, sin paridad
		break;
	}
	
	switch (config.baud_rate) // 16,000,000/(8*BAUDRATE)-1 (por u2x0 = 1)
	{
		case 1200: UBRR0 = 1666;
		break;
		case 2400: UBRR0 = 832;
		break;
		case 4800: UBRR0 = 416;
		break;
		case 9600: UBRR0 = 207;
		break;
		case 14400: UBRR0 = 138;
		break;
		case 19200: UBRR0 = 103;
		break;
		case 28800: UBRR0 = 68;
		break;
		case 38400: UBRR0 = 51;
		break;
		case 57600: UBRR0 = 34;
		break;
		default:PORTB |= 0x20;
		UBRR0 = 207; // Si se tira error, se fija baud_rate = 9600
		break;
	}
	UCSR0B |= (1<<TXEN0)+(1<<RXEN0)+(1<<RXCIE0);						//Se activa Tx. Aca se puede activar Rx, e interrupts respectivos. Activamos interrupt UDR.

}



// Transmits a single character
void USART_Transmit_char(uint8_t data)
{
	// A nice hint: With interrupts, you can send bytes whenever the register UDR0
	// is free. And there is an interrupt called USART_UDRE_vect that *tells you*
	// whenever UDR0 is free.
	// This requires you to have some bytes in the buffer that you would like to
	// send, of course. You have a buffer, don't you?
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}



// Transmits a given string
void USART_Transmit_String(char* string)
{
	char *ptr;
	ptr = string;
	while(*ptr != NULL)
	{
		USART_Transmit_char(*ptr);
		//if(*ptr=='\n'){
		//break;
		//}
		ptr++;
		
	}

}






// Receives a '\n' terminated string and writes it into a supplied buffer.
// The buffer must be guaranteed to handle at least bufflen bytes.
// Returns the number of bytes written into the buffer.