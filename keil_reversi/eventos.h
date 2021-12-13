#ifndef __EVENTOS_H__
#define __EVENTOS_H__
#include "stdint.h"

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
    evento_cadena_enviada,
    evento_continuar_envio,
    evento_timeout
};

// Identificadores de los distintos eventos utilizados
/*#define evento_identificador uint8_t
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
#define evento_apagar_idle 11*/

// Definición del tipo Evento
struct Evento {
    enum evento_identificador ID;
    uint32_t datosAux;
    uint32_t timestamp;
};

#endif 
