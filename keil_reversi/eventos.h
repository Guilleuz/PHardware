#ifndef __EVENTOS_H__
#define __EVENTOS_H__
#include "stdint.h"

// Identificadores de los distintos eventos utilizados
#define evento_identificador uint8_t
#define evento_timer0 0
#define evento_set_alarma 1
#define evento_eint1 2
#define evento_eint2 3
#define evento_alarma_eint1 4
#define evento_alarma_eint2 5
#define evento_actualizar_juego 6
#define evento_power_down 7
#define evento_encender_led 8
#define evento_apagar_led 9
#define evento_encender_idle 10
#define evento_apagar_idle 11

// Definición del tipo Evento
struct Evento {
    evento_identificador ID;
    uint32_t datosAux;
    uint32_t timestamp;
};

#endif 
