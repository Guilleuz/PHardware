#ifndef __COLA_H__
#define __COLA_H__

#include <stdint.h>
#include "eventos.h"

// Guarda un nuevo evento en la cola
void cola_guardar_eventos(uint8_t ID_evento, uint32_t auxData);

// Devuelve 0 si no hay nuevos eventos por leer, 1 en caso contrario
int cola_hay_nuevos(void);

// Pre: cola_hay_nuevos() == 1
// Post: Devuelve el evento más antiguo sin leer
struct Evento cola_ultimo_evento(void);

#endif
