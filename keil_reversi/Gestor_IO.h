#ifndef __GESTOR_IO_H__
#define __GESTOR_IO_H__

#include <stdint.h>

void gestor_io_init(void);
void gestor_io_visualizar_celda(int valor, int candidatos);
void gestor_io_led_validez(int valor);
void gestor_io_latido(int valor);
void gestor_io_overflow(void);
int gestor_io_leer_col(void);
int gestor_io_leer_fila(void);
int gestor_io_leer_valor(void);

#endif
