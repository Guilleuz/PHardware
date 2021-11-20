#ifndef __GESTOR_IO_H__
#define __GESTOR_IO_H__

#include <stdint.h>

/* Inicializamos el GPIO estableciendo como entrada y salida
   los pines que nos interesan */
void gestor_io_init(void);

// Escribimos en GPIO los valores de celda y candidatos
void gestor_io_visualizar_celda(int valor, int candidatos);

// Escribimos valor en el pin correspondiente al led de validez
void gestor_io_led_validez(int valor);

// Escribimos el valor en el pin correspondiente al led de latido
void gestor_io_latido(int valor);

// Escribimos valor en el pin correspondiente al led de overflow
void gestor_io_overflow(void);

// Leemos de GPIO el valor de la columna
int gestor_io_leer_col(void);

// Leemos de GPIO el valor de la fila
int gestor_io_leer_fila(void);

// Leemos de GPIO el nuevo valor a introducir en la celda
int gestor_io_leer_valor(void);

#endif
