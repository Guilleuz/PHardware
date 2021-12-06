#include "Gestor_IO.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>

static char buffer[10];
static int ultimo = 0;
static int ultimoInit = -1;

void gestor_io_nuevo_char(uint32_t caracter) {
a	char c = caracter & 0xff;
	if (ultimo == ultimoInit) ultimoInit = -1;
	buffer[ultimo] = c;
	if (c == '#') ultimoInit = ultimo;
	else if (c == '!' && ultimoInit != -1) {
		char comando[11];
		if (ultimoInit < ultimo) {
			strncpy(comando, buffer + ultimoInit + 1, ultimo - ultimoInit - 1);
		} else if (ultimoInit == 9) {
			strncpy(comando, buffer, ultimo);
		} else{
			char sub[11];
			strncpy(comando, buffer + ultimoInit + 1, 10 - ultimoInit - 1);
			strncpy(sub, buffer, ultimo);
			strcat(comando, sub);
		}

		ultimoInit = -1;
	}
	ultimo = (ultimo + 1) % 10;
}

/* Inicializamos el GPIO estableciendo como entrada y salida
   los pines que nos interesan */
void gestor_io_init(void) {
	// Inicializamos todos los pines como entrada
	GPIO_iniciar();
	// Pines 0-13 salida
	// 0-3 valor de la celda
	// 4-12 candidatos
	// 13 led de validaci�n
	GPIO_marcar_salida(0, 14);
	
	// Pin 30 led de overflow
	// Pin 31 latido idle
	GPIO_marcar_salida(30,2);
}

// Escribimos en GPIO los valores de celda y candidatos
void gestor_io_visualizar_celda(int valor, int candidatos) {
	GPIO_escribir(0, 4, valor);
	GPIO_escribir(4, 9, candidatos);
}

// Escribimos valor en el pin correspondiente al led de validez
void gestor_io_led_validez(int valor) {
	GPIO_escribir(13, 1, valor);
}

// Escribimos valor en el pin correspondiente al led de overflow
void gestor_io_overflow(void) {
	GPIO_escribir(30, 1, 1);
}

// Escribimos el valor en el pin correspondiente al led de latido
void gestor_io_latido(int valor) {
	GPIO_escribir(31, 1, valor);
}

// Leemos de GPIO el valor de la columna
int	gestor_io_leer_col(void) {
	return GPIO_leer(20, 4);
}

// Leemos de GPIO el valor de la fila
int gestor_io_leer_fila(void) {
	return GPIO_leer(16, 4);
}

// Leemos de GPIO el nuevo valor a introducir en la celda
int gestor_io_leer_valor(void) {
	return GPIO_leer(24, 4);
}


