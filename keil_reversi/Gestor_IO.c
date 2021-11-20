#include "Gestor_IO.h"
#include "gpio.h"
#include <stdint.h>

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

void gestor_io_visualizar_celda(int valor, int candidatos) {
	GPIO_escribir(0, 4, valor);
	GPIO_escribir(4, 9, candidatos);
}

void gestor_io_led_validez(int valor) {
	GPIO_escribir(13, 1, valor);
}

void gestor_io_overflow(void) {
	GPIO_escribir(30, 1, 1);
}

void gestor_io_latido(int valor) {
	GPIO_escribir(31, 1, valor);
}

int	gestor_io_leer_col(void) {
	return GPIO_leer(20, 4);
}

int gestor_io_leer_fila(void) {
	return GPIO_leer(16, 4);
}

int gestor_io_leer_valor(void) {
	return GPIO_leer(24, 4);
}


