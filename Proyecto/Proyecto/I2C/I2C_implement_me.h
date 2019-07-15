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
 * This file is complete, but the according .c file not. See there and
 * implement the function bodies!
 * 
 ****************************************************/

#ifndef _I2C_IMPLEMENT_ME_H_
#define _I2C_IMPLEMENT_ME_H_

#include <stdbool.h>

#define DS1307_READ	 	0xD1 //1101 000 1
#define DS1307_WRITE	0xD0 //1101 000 0

// Send a start condition
void i2cSendStart();

// Send a stop condition
void i2cSendStop();

// Waits for the previous operation to finish
void i2cWaitForComplete();

// Transmits one byte over the bus to a slave
// (Can be either an address or a value.)
void i2cSendByte(unsigned char data);

// Receives one byte from a slave
// (Always a value; master has no address.)
// Send an ACK if you expect more data, NAK if not.
void i2cReceiveByte(bool sendAnACK);

// After receiving a byte, fetch this byte from the TWDR register
// (if you are interested in it).
// It is possible to *not* call this function after a call to
// i2cReceiveByte() if you are not interested in the actual value.
unsigned char i2cGetReceivedByte(void);

void DS1307_get(int *hora, int *minuto, int *dia, int *mes, int *ano);

void I2C_init();

void DS1307_escribir(int add, int dato);

void DS1307_leer(int add, int i,int *datos);

#endif // _I2C_IMPLEMENT_ME_H_