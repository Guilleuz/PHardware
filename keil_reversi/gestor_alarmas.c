#include "gestor_alarmas.h"
#include "timer1.h"
#include "eventos.h"
#include "cola.h"

#define NUM_ALARMAS 8
static uint32_t eventos[NUM_ALARMAS];
// 0 no hay alarma registrada, 1 sí
static uint8_t ocupado[NUM_ALARMAS] = {0};
static uint32_t tInicio[NUM_ALARMAS];

// Devuelve 1 si la alarma es periodica, 0 en caso contrario
__inline static int esPeriodico(int i) {
    return eventos[i] & 0x00800000;
}

// Devuelve el periodo de la alarma
__inline static int periodo(int i) {
    return eventos[i] & 0x007fffff;
}

// devuelve el identificador del evento
__inline static evento_identificador evento_ID(int n) {
    return (n & 0xff000000) >> 24;
}

// Programamos una alarma
void gestor_set_alarma(uint32_t evento) {
    // Comprobamos si la alarma ya está almacenada
    for (int i = 0; i < NUM_ALARMAS; i++) {
        if (ocupado[i] && evento_ID(eventos[i]) == evento_ID(evento)) {
            // En tal caso, reprogramamos la alarma
            eventos[i] = evento;
			tInicio[i] = temporizador_leer();
            if (periodo(i) == 0) {
                // Cancelamos la alarma si su retardo es 0
                ocupado[i] = 0;
            }
            return;
        }
    }

    // Si no está almacenada, le buscamos un hueco y la guardamos
    for (int i = 0; i < NUM_ALARMAS; i++) {
        if (!ocupado[i]) {
            ocupado[i] = 1;
            eventos[i] = evento;
            tInicio[i] = temporizador_leer();
            if (periodo(i) == 0) {
                // Cancelamos la alarma si su retardo es 0
                ocupado[i] = 0;
            }
            return;
        }
    }

    // Si no hubiera hueco, no se guarda la alarma
}

// Lanzamos los eventos correspondientes a las alarmas pendientes
void gestor_disparar_alarmas(void) {
    for (int i = 0; i < NUM_ALARMAS; i++) {
        // Comprobamos si ha vencido el tiempo de cada alarma
        if (ocupado[i] && periodo(i) <= (temporizador_leer() - tInicio[i]) / 1000) {
            // En tal caso, lanzamos el evento correspondiente
            cola_guardar_eventos(evento_ID(eventos[i]), 0);

            // Si la alarma es períodica se reinicia, en otro caso se cancela
            if (!esPeriodico(i)) ocupado[i] = 0;
            else tInicio[i] = temporizador_leer();
        }
    }
}
