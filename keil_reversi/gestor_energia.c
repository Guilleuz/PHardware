#include "gestor_energia.h"
#include <LPC210x.H>                       /* LPC210x definitions */

// Apaga el procesador
void gestor_energia_power_down (void)  {
    EXTWAKE = 6; // EXTINT1  y EXTINT2 despertarán al procesador
    PCON |= 0x02;
    reiniciarPLL();
}

// Pone al procesador en modo idle
void gestor_energia_idle (void) {
    PCON |= 0x01;
}
