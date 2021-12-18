#include "Gestor_IO.h"
#include "gpio.h"
#include "eventos.h"
#include "cola.h"
#include <LPC210x.H>   
#include <stdint.h>
#include <string.h>
#include <ctype.h>

static char buffer[10];
static int ultimo = 0;

int esNumero(char* cadena) {
	for (int i = 0; i < strlen(cadena); i++) {
		if (cadena[i] - '0' < 0 || cadena[i] - '0' > 9) return 0;
	}
	
	return 1;
}

void gestor_io_nuevo_char(char caracter) {
    if (ultimo == 0 && caracter == '#') { // Si introducimos '#' como primer valor, lo guardamos en el buffer
        buffer[ultimo] = caracter;
        ultimo = (ultimo + 1) % 10;
    } else if (ultimo != 0 && caracter == '#') { // Si introducimos '#', la movemos a la primera posición del buffer si no lo está
        ultimo = 0; 
        buffer[ultimo] = caracter;
        ultimo = (ultimo + 1) % 10;
    } else if (buffer[0] == '#' && caracter == '!') { // Si hay '#' en la primera posición y recibimos '!', guardamos comando
            char comando[10]; 
						strncpy(comando, buffer + 1, ultimo - 1);
						comando[ultimo - 1] = '\0';
            if (strcmp(comando, "RST") == 0) {
                cola_guardar_eventos(evento_reset_juego, 0);
            }
            else if (strcmp(comando, "NEW") == 0) {
                cola_guardar_eventos(evento_empezar_juego, 0);
            }
            else if (strlen(comando) == 4 && esNumero(comando)) {
                uint8_t fila = comando[0] - '0';
                uint8_t columna = comando[1] - '0';
                uint8_t valor = comando[2] - '0';
                uint8_t check = comando[3] - '0'; 
                
                // Bits 0-7 valor, 8-15 col, 16-23 fila, 24-31 checksum
                uint32_t auxData = (check) << 24;
                auxData |= (fila) << 16;
                auxData |= (columna) << 8;
                auxData |= valor;
                
                cola_guardar_eventos(evento_jugada, auxData);
            }
            ultimo = 0;
    } else { // Si recibimos cualquier otro caracter, guardamos en buffer
        buffer[ultimo] = caracter;
        ultimo = (ultimo + 1) % 10;
    }
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




/*|		| 1 2 3	|		|
|	3P	| 4   6	|	5E	|
|		|   8 9 |		|
-------------------------
|		| 1 2 3	|		|
|	3P	| 4   6	|	5E	|
|		|   8 9 |		|
-------------------------
|		| 1 2 3	|		|
|	3P	| 4   6	|	5E	|
|		|   8 9 |		|*/
