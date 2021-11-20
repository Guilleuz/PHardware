#ifndef __GESTOR_ALARMAS_H__
#define __GESTOR_ALARMAS_H__

#include <stdint.h>

// Programamos una alarma
void gestor_set_alarma(uint32_t evento);

// Lanzamos los eventos correspondientes a las alarmas pendientes
void gestor_disparar_alarmas(void);

#endif
