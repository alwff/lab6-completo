/* Universidad del Valle de Guatemala
IE2023:: Programación de Microcontroladores
LABORATORIO6.c
Autor: Alejandra Cardona
Hardware: ATMEGA328P
Creado: 22/04/2024
Última modificación: 22/04/2024

****************************************************************** */

// LIBRERÍAS

#define F_CPU 16000000UL //Frecuencia en la que opera el sistema - 16 MHz
//UL = unsigned long
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initUART09600(void); //FConfiguracion UART
void writeUART(char character); //Funcion para escribir
void writeTextUART(char* text); //Puntero
int intforport(char bufferRX);

//Variable Formato: Tipo nombre = Valor
//Variables
volatile char bufferRX; //Es volatil porque va a estar dentro de la interrupcion, pues puede cambiar en todo momento
//******************************************************************

int main(void)
{
	cli();
	//Como salidas
	DDRB |= 0xFF;
	DDRD |= (1<<PORTD6)|(1<<PORTD7);
	
	initUART09600();
	
	sei();
	
	/*
	//Al presionar enter basicamente se envia 10 y 13, eso es una nueva linea
	writeUART(10);
	writeUART(13);
	writeUART('\n'); //new line
	writeUART('H');
	writeUART('O');
	writeUART('L');
	writeUART('A');
	writeUART(' ');
	*/
	
	writeTextUART("\n Hola mundo! \n");
		
	writeUART('5');
	
	while (1)
	{
	//showInPort(characterARDport);
	}
}


void initUART09600(void){
	//Configurando los pines RX y TX
	DDRD &= ~(1<<DDD0); //D0 RX como entrada
	DDRD |= (1<<DDD1); //D1 TX como salida
	
	//Se define el modo de trabajo
	UCSR0A = 0;
	UCSR0A |= (1<<U2X0); // Usando el registro de control A. Se configura en Modo Fast U2X0 = 1
	
	//Se configura el registro de control B
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0); // Se habilita la interrupcion ISR RX
	UCSR0B |= (1<<RXEN0); // Se habilita RX
	UCSR0B |= (1<<TXEN0); // Se habilita TX
	
	//Se configura el registro de control C
	//Se define el frame
	UCSR0C = 0;
	UCSR0C = (1<<UCSZ01)| (1<<UCSZ00); // Define el numero de bits de data en el frame del receiver y el transmitter
	
	//Calculos
	//UBRRn=(fosc/mhz*BAUD)-1
	//BAUD=(fosc/mhz*(UBRRn+1))
	//Error%=((BAUDcalculado/BAUD)-1)*100%
	//Usando fosc de 16 MHZ 
	//UBRRn = 207
	//BAUDRATE = 9600

	UBRR0 = 207;
}

//Escritura - Verifica si el UDREn buffer esta vacio
void writeUART(char character){
	while(!(UCSR0A&(1<<UDRE0))); //Si esto esta vacio entonces se manda
	//Mientras while no sea cierto, y como esta vacio simplemente se atora en el while hasta que se cumpla la condicion
	UDR0 = character;
}

//Trabajando de la misma forma que la funcion anterior, esta es capaz de leer strings completas
void writeTextUART(char* text){
	uint8_t i;
	for (i=0; text[i]!='\0'; i++){ //Inicia en 0 y para cuando sea nulo \0
		while(!(UCSR0A&(1<<UDRE0))); 
		UDR0 = text[i]; //Barre todo el arreglo hasta acabar
	}
	
}

int intforport(char bufferRX){
	return bufferRX - '0'; //Convierte lo recibido por el buffer en un entero para moder mostrarlo en el puertoB
}

//Interrupcion 
//USART_RX
ISR(USART_RX_vect){
	//Actuando como un eco, lo que se envia, regresa lo  mismo
	bufferRX = UDR0;
	while(!(UCSR0A&(1<<UDRE0))); //Si esto esta vacio entonces se manda
	UDR0 = bufferRX; //Mete a UDR0 lo que esta en el buffer para enviarlo
	
	PORTB=intforport(bufferRX); //Lo recibido lo muestra en el portB
}
