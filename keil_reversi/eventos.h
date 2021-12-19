#ifndef __EVENTOS_H__
#define __EVENTOS_H__
#include "stdint.h"

// Definición de los identificadores de evento
enum evento_identificador {
    evento_timer0,
    evento_set_alarma,
    evento_eint1,
    evento_eint2,
    evento_alarma_eint1,
    evento_alarma_eint2,
    evento_actualizar_juego, 
    evento_power_down,
    evento_encender_led,
    evento_apagar_led, 
    evento_encender_idle, 
    evento_apagar_idle,
    evento_nuevo_caracter,
    evento_reset_juego,
    evento_empezar_juego,
    evento_jugada,
    evento_timeout
};

// Definición del tipo Evento
struct Evento {
    enum evento_identificador ID;
    uint32_t datosAux;
    uint32_t timestamp;
};

#endif 
