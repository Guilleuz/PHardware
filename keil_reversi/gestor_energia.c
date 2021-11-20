#include "gestor_energia.h"
#include <LPC210x.H>                       /* LPC210x definitions */
// Set the processor into power down state 
// The watchdog cannot wake up the processor from power down
void gestor_energia_power_down (void)  {
    EXTWAKE = 6; // EXTINT1  y EXTINT2 despertarán al procesador
    PCON |= 0x02;
    reiniciarPLL();
}

void gestor_energia_idle (void) {
    PCON |= 0x01;
}
