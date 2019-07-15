// MCU Clock Speed - needed for delay.h
#define F_CPU	16000000UL

// read and write addresses; set ADD0 = GND


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "I2C/I2C_implement_me.h"
#include "Timer/Timer_include.h"
#include "Display/LCD.h"
#include "Adquisicion/Adquisicion.h"

uint16_t temp_ext; // variable de temperatura exterior
uint16_t hum_ext; // variable de humedad exterior
uint16_t temp_int; // temperatura interior
uint16_t hum_int; // humedad interior
int punto_de_rocio_int;
int punto_de_rocio_ext;

int datos[8];
char buffer[10];
int estado;
int hora;
int minuto;
int dia;
int mes;
int ano;


uint16_t temp_max;
uint16_t temp_min;

// guardamos en RTC dia_actual/mes_actual/ tmin / tmax /dia_anterior/mes_anterior/ tmin / tmax
//						8	  /		9	 /  10  /  11  /		12  /		13	 /  14  /  15
int main(void)
{
	_delay_ms(2000);
    
	I2C_init();
	init_timer1();
	init_timer0();
	init_timer2();
	LCD_init();
	DDRD |= 0xDC; //1101 1100
	PORTD |= 0xDC;
	
	DDRD &= ~0x20; // xx0x xxxx PD5 para boton
	PORTD |= 0x20; // PUR para PD5.
	
	PCMSK2 = 0x20; // PD5 como PCINT para boton
	PCICR = (1<<PCIE2);
	
	/////////////////////////////////////////////////
	adquisicion_dht22(2, &temp_ext, &hum_ext);
	temp_ext=temp_ext*2;
	hum_ext=hum_ext*2; // adquisiciones iniciales
	
	adquisicion_dht22(3,&temp_int, &hum_int);
	temp_int=temp_int*2;
	hum_int=hum_int*2;
	/////////////////////////////////////////////////
	sei();
	
	estado=0;
	
	
	DS1307_get(&hora, &minuto, &dia, &mes, &ano);
	/*DS1307_escribir(0,0);
	DS1307_escribir(1,0x20);
	DS1307_escribir(2,0x00);
	DS1307_escribir(3,1);
	DS1307_escribir(4,0x08);
	DS1307_escribir(5,0x07);
	DS1307_escribir(6,0x19);
	DS1307_escribir(12,7);
	DS1307_escribir(13,7);
	DS1307_escribir(14,9);
	DS1307_escribir(15,20);*/
	
	
	LCD_write_string("Temp:",0,1);
	LCD_write_string("Hora:",40,0);
	LCD_write_string("INT",13,0);
	
    while (1) 
    {
		//_delay_ms(500);
    }
}



ISR (TIMER1_OVF_vect) // Adquisicion de datos periodico
{
	
	
	adquisicion_dht22(2, &temp_ext, &hum_ext);
	adquisicion_dht22(3, &temp_int, &hum_int);
	
	DS1307_get(&hora, &minuto, &dia, &mes, &ano);
	
	DS1307_leer(8,8,datos); //dia actual, chequeamos si el dia y mes es el mismo, si no actualizamos
	if(datos[0]!=dia || datos[1]!=mes)
	{
		DS1307_escribir(12,datos[0]);
		DS1307_escribir(13,datos[1]);
		DS1307_escribir(14,datos[2]);
		DS1307_escribir(15,datos[3]);
		datos[4]=datos[0];
		datos[5]=datos[1];
		datos[6]=datos[2];
		datos[7]=datos[3];
		
		datos[0]=dia;
		datos[1]=mes;
		datos[2]=temp_ext/10;
		datos[3]=datos[2];
		DS1307_escribir(8,dia);
		DS1307_escribir(9,mes);
		DS1307_escribir(10,datos[2]);
		DS1307_escribir(11,datos[3]);
		
	} else // si es la fecha actual
	{
		if (temp_ext/10<datos[2]) // si la temp es menor al minimo, actualizamos
		{
			datos[2]=temp_ext/10;
			DS1307_escribir(10,datos[2]);
		}
		if(temp_ext/10>datos[3]) // si la temp es mayor al max
		{
			datos[3]=temp_ext/10;
			DS1307_escribir(11,datos[3]);
		}
	}
	
	if (estado==1) // EXTERNO
	{
		
		if(temp_ext>=0) LCD_write_string(" ",5,0);
		itoa(temp_ext/10, buffer, 10);
		if(abs(temp_ext/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		
		LCD_write_string(".",8,0);
		itoa(temp_ext%10, buffer, 10);
		LCD_write_string(buffer,9,0);
		
		itoa(hum_ext/10, buffer, 10);
		if(abs(hum_ext/10)<10)
		{
			LCD_write_string("0",44,0);
			LCD_write_string(buffer,45,0);
		} else
		{
			LCD_write_string(buffer,44,0);
		}
		LCD_write_string(".",46,0);
		itoa(hum_ext%10, buffer, 10);
		LCD_write_string(buffer,47,0);
		LCD_write_string("%",48,0);
		
		punto_de_rocio_ext=sqrt(sqrt(sqrt(hum_ext)))/sqrt(sqrt(sqrt(1000)))*(110+temp_ext/10)-110;
		itoa(punto_de_rocio_ext, buffer, 10);
		if(punto_de_rocio_ext<10 && punto_de_rocio_ext>=0)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else LCD_write_string(buffer,54,0);
	} else if (estado==2) //interna
	{
		
		if(temp_int>=0) LCD_write_string(" ",5,0);
		itoa(temp_int/10, buffer, 10);
		if(abs(temp_int/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		LCD_write_string(".",8,0);
		itoa(temp_int%10, buffer, 10);
		LCD_write_string(buffer,9,0);
		
		itoa(hum_int/10, buffer, 10);
		if(abs(hum_ext/10)<10)
		{
			LCD_write_string("0",44,0);
			LCD_write_string(buffer,45,0);
		} else
		{
			LCD_write_string(buffer,44,0);
		}
		LCD_write_string(".",46,0);
		itoa(hum_int%10, buffer, 10);
		LCD_write_string(buffer,47,0);
		LCD_write_string("%",48,0);
		
		punto_de_rocio_int=sqrt(sqrt(sqrt(hum_int)))/sqrt(sqrt(sqrt(1000)))*(110+temp_int/10)-110;
		itoa(punto_de_rocio_int, buffer, 10);
		if(punto_de_rocio_int<10 && punto_de_rocio_int>=0)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else LCD_write_string(buffer,54,0);
	} else if (estado==3) //minima, max hoy
	{
		if(datos[2]>=0) LCD_write_string(" ",5,0);
		itoa(datos[2], buffer, 10);
		if(abs(datos[2])<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		
		if(datos[3]>=0) LCD_write_string(" ",5,0);
		itoa(datos[3], buffer, 10);
		if(abs(datos[3])<10)
		{
			LCD_write_string("0",46,0);
			LCD_write_string(buffer,47,0);
		} else
		{
			LCD_write_string(buffer,46,0);
		}
		
	} else if (estado==4)
	{
		if(datos[6]>=0) LCD_write_string(" ",5,0);
		itoa(datos[6], buffer, 10);
		if(abs(datos[6])<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		
		if(datos[7]>=0) LCD_write_string(" ",5,0);
		itoa(datos[7], buffer, 10);
		if(abs(datos[7])<10)
		{
			LCD_write_string("0",46,0);
			LCD_write_string(buffer,47,0);
		} else
		{
			LCD_write_string(buffer,46,0);
		}
		
	} else if (estado==0)//hora, fecha, Tint
	{
		itoa(hora, buffer, 10);
		if(hora<10)
		{
			LCD_write_string("0",45,0);
			LCD_write_string(buffer,46,0);
		} else
		{
			LCD_write_string(buffer,45,0);
		}
		LCD_write_string(":",47,0);
		
		itoa(minuto, buffer, 10);
		if(minuto<10)
		{
			LCD_write_string("0",48,0);
			LCD_write_string(buffer,49,0);
		} else
		{
			LCD_write_string(buffer,48,0);
		}
		
		itoa(dia, buffer, 10);
		if(dia<10)
		{
			LCD_write_string("0",51,0);
			LCD_write_string(buffer,52,0);
		} else
		{
			LCD_write_string(buffer,51,0);
		}
		LCD_write_string("/",53,0);
		
		itoa(mes, buffer, 10);
		if(mes<10)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else
		{
			LCD_write_string(buffer,54,0);
		}
		
		if(temp_int>=0) LCD_write_string(" ",5,0);
		itoa(temp_int/10, buffer, 10);
		if(abs(temp_int/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		LCD_write_string(buffer,6,0);
		LCD_write_string(".",8,0);
		itoa(temp_int%10, buffer, 10);
		LCD_write_string(buffer,9,0);
	}
	
}

ISR (PCINT2_vect)
{
	
	if (!(PIND&0x20))
	{
		TCCR2B = (1<<CS22)+(1<<CS21)+(1<<CS20); // clk/1024
		TIMSK2 = (1<<TOIE2);
		TCNT2=0;
	}
	else
	{
		TCNT2=0;
		TCCR2B = 0;
		TIMSK2 = 0;
	}
}

ISR (TIMER2_OVF_vect) // BOTON
{
	
	TCCR2B = (0<<CS22)+(0<<CS21)+(0<<CS20); // stop
	
	
	
	if (estado<4) // n estados
	{
		estado++;
	} else
	{
		estado=0;
	}
	
	if (estado==2) // interna
	{
		LCD_write_string("Temp:",0,1);
		LCD_write_string("Hum:",40,0);
		LCD_write_string("Dw:",50,0);
		LCD_write_string("INT",13,0);
		
		if(temp_int>=0) LCD_write_string(" ",5,0);
		itoa(temp_int/10, buffer, 10);
		if(abs(temp_int/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		LCD_write_string(".",8,0);
		itoa(temp_int%10, buffer, 10);
		LCD_write_string(buffer,9,0);
		
		itoa(hum_int/10, buffer, 10);
		if(abs(hum_int/10)<10)
		{
			LCD_write_string("0",44,0);
			LCD_write_string(buffer,45,0);
		} else
		{
			LCD_write_string(buffer,44,0);
		}
		LCD_write_string(".",46,0);
		itoa(hum_int%10, buffer, 10);
		LCD_write_string(buffer,47,0);
		LCD_write_string("%",48,0);
		
		
		itoa(punto_de_rocio_int, buffer, 10);
		if(punto_de_rocio_int<10 && punto_de_rocio_int>=0)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else LCD_write_string(buffer,54,0);
		
		
	} else if (estado==1) //externa
	{
		LCD_write_string("Temp:",0,1);
		LCD_write_string("Hum:",40,0);
		LCD_write_string("Dw:",50,0);
		LCD_write_string("EXT",13,0);
		
		if(temp_ext>=0) LCD_write_string(" ",5,0);
		itoa(temp_ext/10, buffer, 10);
		if(abs(temp_ext/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		LCD_write_string(".",8,0);
		itoa(temp_ext%10, buffer, 10);
		LCD_write_string(buffer,9,0);
		
		itoa(hum_ext/10, buffer, 10);
		if(abs(hum_ext/10)<10)
		{
			LCD_write_string("0",44,0);
			LCD_write_string(buffer,45,0);
		} else
		{
			LCD_write_string(buffer,44,0);
		}
		LCD_write_string(".",46,0);
		itoa(hum_ext%10, buffer, 10);
		LCD_write_string(buffer,47,0);
		LCD_write_string("%",48,0);
		
		
		itoa(punto_de_rocio_ext, buffer, 10);
		if(punto_de_rocio_ext<10 && punto_de_rocio_ext>=0)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else LCD_write_string(buffer,54,0);
	} else if (estado==3) //minmax hoy
	{
		LCD_write_string("Tmin:",0,1);
		LCD_write_string("Tmax:",40,0);
		LCD_write_string("HOY",13,0);
		itoa(datos[0], buffer, 10);
		if(datos[0]<10)
		{
			LCD_write_string("0",51,0);
			LCD_write_string(buffer,52,0);
		} else
		{
			LCD_write_string(buffer,51,0);
		}
		LCD_write_string("/",53,0);
		
		itoa(datos[1], buffer, 10);
		if(datos[1]<10)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else
		{
			LCD_write_string(buffer,54,0);
		}

		if(datos[2]>=0) LCD_write_string(" ",5,0);
		itoa(datos[2], buffer, 10);
		if(abs(datos[2])<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		
		if(datos[3]>=0) LCD_write_string(" ",5,0);
		itoa(datos[3], buffer, 10);
		if(abs(datos[3])<10)
		{
			LCD_write_string("0",46,0);
			LCD_write_string(buffer,47,0);
		} else
		{
			LCD_write_string(buffer,46,0);
		}
		
	}  else if (estado==4)
	{
		LCD_write_string("Tmin:",0,1);
		LCD_write_string("Tmax:",40,0);
		LCD_write_string("AYER",12,0);
		itoa(datos[4], buffer, 10);
		if(datos[4]<10)
		{
			LCD_write_string("0",51,0);
			LCD_write_string(buffer,52,0);
		} else
		{
			LCD_write_string(buffer,51,0);
		}
		LCD_write_string("/",53,0);
		
		itoa(datos[5], buffer, 10);
		if(datos[5]<10)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else
		{
			LCD_write_string(buffer,54,0);
		}

		if(datos[6]>=0) LCD_write_string(" ",5,0);
		itoa(datos[6], buffer, 10);
		if(abs(datos[6])<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		
		if(datos[7]>=0) LCD_write_string(" ",5,0);
		itoa(datos[7], buffer, 10);
		if(abs(datos[7])<10)
		{
			LCD_write_string("0",46,0);
			LCD_write_string(buffer,47,0);
		} else
		{
			LCD_write_string(buffer,46,0);
		}
		
	} else if (estado==0)
	{
		LCD_write_string("Temp:",0,1);
		LCD_write_string("Hora:",40,0);
		LCD_write_string("INT",13,0);
		
		itoa(hora, buffer, 10);
		if(hora<10)
		{ 
			LCD_write_string("0",45,0);
			LCD_write_string(buffer,46,0);
		} else
		{
			LCD_write_string(buffer,45,0);
		}
		LCD_write_string(":",47,0);
		
		itoa(minuto, buffer, 10);
		if(minuto<10)
		{
			LCD_write_string("0",48,0);
			LCD_write_string(buffer,49,0);
		} else
		{
			LCD_write_string(buffer,48,0);
		}
		
		itoa(dia, buffer, 10);
		if(dia<10)
		{
			LCD_write_string("0",51,0);
			LCD_write_string(buffer,52,0);
		} else
		{
			LCD_write_string(buffer,51,0);
		}
		LCD_write_string("/",53,0);
		
		itoa(mes, buffer, 10);
		if(mes<10)
		{
			LCD_write_string("0",54,0);
			LCD_write_string(buffer,55,0);
		} else
		{
			LCD_write_string(buffer,54,0);
		}
		
		if(temp_int>=0) LCD_write_string(" ",5,0);
		itoa(temp_int/10, buffer, 10);
		if(abs(temp_int/10)<10)
		{
			LCD_write_string("0",6,0);
			LCD_write_string(buffer,7,0);
		} else
		{
			LCD_write_string(buffer,6,0);
		}
		LCD_write_string(buffer,6,0);
		LCD_write_string(".",8,0);
		itoa(temp_int%10, buffer, 10);
		LCD_write_string(buffer,9,0);
	}
	TIMSK2 = 0;
}




