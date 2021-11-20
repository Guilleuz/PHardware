#include "timer1.h"
#include <LPC210X.H>                            // LPC21XX Peripheral Registers

// Cuenta en microsegundos

void temporizador_iniciar(void) {
    // El temporizador contará cada 60 ciclos (1 micro segundo)
	  T1PR = 59;
	  T1MCR = 0;
}

void temporizador_empezar(void) {
    T1TCR = 1;         // Timer1 Enable	
}

int temporizador_leer(void) {
		return T1TC;
}

void temporizador_parar(void) {
    T1TCR = 3;  // Reinicia el contador
    T1TCR = 0;  // Para el reloj
}

