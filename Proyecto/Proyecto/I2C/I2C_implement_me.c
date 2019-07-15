/***************************************************
 * This is a I2C library for the ATMega328P.
 *
 * It can provide basic I2C communication for any application.
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
#include <avr/io.h>
#include <util/delay.h>

#include "I2C_implement_me.h"


// Send a start condition
void i2cSendStart()
{
	TWCR = (1<<TWINT) + (1<<TWSTA) + (1<<TWEN); // clear flag, condicion START y ENABLE
	
	
}




// Send a stop condition
void i2cSendStop()
{
	TWCR = (1<<TWINT) + (1<<TWSTO) + (1<<TWEN); // clear flag, condicion STOP y ENABLE. 
	while(!(TWCR & (1<<TWSTO))); //esperamos a que termine de parar
}




// Waits for the previous operation to finish
void i2cWaitForComplete()
{
	int i=0;
	while(!(TWCR & (1<<TWINT)))
	{
		if (i>500) break;
		i++;
		_delay_ms(1);	
	} // esperamos a que se setee el flag
}




// Transmits one byte over the bus to a slave
// (Can be either an address or a value.)
void i2cSendByte(unsigned char data)
{
	TWDR = data; // cargamos data
	TWCR = (1<<TWINT) + (1<<TWEN); // clear flag para comenzar transmision
}




// Receives one byte from a slave
// (Always a value; master has no address.)
// Send an ACK if you expect more data, NAK if not.
void i2cReceiveByte(bool sendAnACK)
{
	if (sendAnACK == 1)
	{
		TWCR = (1<<TWINT) + (1<<TWEN) + (1<<TWEA); // FLAG, ENABLE, ACK
	}else
	{
		TWCR = (1<<TWINT) + (1<<TWEN); // flag, enable, NACK
	}
	
}




// After receiving a byte, fetch this byte from the TWDR register
// (if you are interested in it).
// It is possible to *not* call this function after a call to
// i2cReceiveByte() if you are not interested in the actual value.
unsigned char i2cGetReceivedByte(void)
{
	return TWDR; //retornamos valor
}

void I2C_init()
{
	TWBR = 92;									// 80 kHz at 16MHz crystal and no prescaler
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));		// prescaler 0
	TWDR = 0xFF;								// sending only ones equals an idle high SDA line
	TWCR = (1<<TWEN)|							// Enable TWI-interface and release TWI pins.
	(0<<TWIE)|(0<<TWINT)|				// Disable Interupt.
	(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|	// No Signal requests.
	(0<<TWWC); //
}

void DS1307_escribir(int add, int dato) // de 8 a 63 es ram
{
	i2cSendStart();							// send start condition
	i2cWaitForComplete();
	i2cSendByte(DS1307_WRITE); 				// WRITE a DS1307
	i2cWaitForComplete();
	i2cSendByte(add); 						// fijamos read address a 0, para en
	i2cWaitForComplete();
	i2cSendByte(dato); 						// fijamos read address a 0, para en
	i2cWaitForComplete();
	i2cSendStop();
	TWCR = 0;
}

void DS1307_leer(int add, int largo,int *datos) // address, largo de datos a leer, arreglo para guardarlos
{
	int *ptr = datos;
	int i = 0;
	i2cSendStart();							// send start condition
	i2cWaitForComplete();
	i2cSendByte(DS1307_WRITE); 				// WRITE a DS1307
	i2cWaitForComplete();
	i2cSendByte(add); 						// fijamos read address a add, para lectura
	i2cWaitForComplete();
	
	i2cSendStart();							//repeat START
	i2cWaitForComplete();
	i2cSendByte(DS1307_READ);				// send READ address of DS1307
	i2cWaitForComplete();
	
	while (i<largo-1)
	{
		i2cReceiveByte(true);					// receives one byte from the bus and ACKs it
		i2cWaitForComplete();
		*ptr = i2cGetReceivedByte(); 			// guardamos minutos
		i2cWaitForComplete();
		i++;
		ptr++;
	}
	i2cReceiveByte(false);					// receives one byte from the bus and ACKs it
	i2cWaitForComplete();
	*ptr = i2cGetReceivedByte(); 			// guardamos minutos
	i2cWaitForComplete();
	
	i2cSendStop();							// send stop condition
	TWCR = 0;
}

void DS1307_get(int *hora, int *minuto, int *dia, int *mes, int *ano)
{
	
	int temporal;
	
	
	i2cSendStart();							// send start condition
	i2cWaitForComplete();
	
	i2cSendByte(DS1307_WRITE); 				// WRITE a DS1307
	i2cWaitForComplete();
	i2cSendByte(0x01); 						// fijamos read address a 0, para en
	i2cWaitForComplete();

	
	i2cSendStart();							// send repeated start condition
	i2cWaitForComplete();
	
	i2cSendByte(DS1307_READ);				// send READ address of DS1307
	i2cWaitForComplete();
	
	i2cReceiveByte(true);					// receives one byte from the bus and ACKs it
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte(); 			// guardamos minutos
	*minuto = 10*((temporal&0xF0)>>4)+(temporal&0x0F);
	i2cWaitForComplete();
	
	i2cReceiveByte(true);					// llamamos las horas
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte();				// HORAS, Leemos horas
	*hora = 10*((temporal&0x30)>>4)+(temporal&0x0F);
	i2cWaitForComplete();
	
	i2cReceiveByte(true);					// llamamos el DIA de la semana
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte();				// DIA DE LA SEMANA
	i2cWaitForComplete();
	
	i2cReceiveByte(true);					// 
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte();				// DIA DEL MES
	*dia = 10*((temporal&0x30)>>4)+(temporal&0x0F);
	i2cWaitForComplete();
	
	i2cReceiveByte(true);					// llamamos mes
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte();				// MES, Leemos horas
	*mes = 10*((temporal&0x30)>>4)+(temporal&0x0F);
	i2cWaitForComplete();
	
	i2cReceiveByte(false);					// llamamos año sin ack
	i2cWaitForComplete();
	temporal = i2cGetReceivedByte();				// AÑO, Leemos horas
	*ano = 10*((temporal&0xF0)>>4)+(temporal&0x0F);
	i2cWaitForComplete();
	
	i2cSendStop();							// send stop condition
	TWCR = 0;								// stop everything
	
	
	
}