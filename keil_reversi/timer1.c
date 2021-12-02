#include "timer1.h"
#include <stdint.h>
#include <LPC210X.H>                            // LPC21XX Peripheral Registers

// Cuenta en microsegundos

// Configuración inicial del timer
void temporizador_iniciar(void) {
    // El temporizador contará cada 60 ciclos (1 micro segundo)
	  T1PR = 59;
	  T1MCR = 0;
}

// Da comienzo al timer
void temporizador_empezar(void) {
    T1TCR = 1;         // Timer1 Enable	
}

uint32_t __SWI_0 (void) {
    return T1TC;
}

// Reinicia el contador del timer y lo para
void temporizador_parar(void) {
    T1TCR = 3;  // Reinicia el contador
    T1TCR = 0;  // Para el reloj
}

