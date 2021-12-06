#include "botones.h"
#include "cola.h"
#include <LPC210X.H>

// Interrupción del botón 1
void eint1_ISR (void) __irq {
	VICIntEnClr = VICIntEnClr | 0x00008000; // Deshabilitamos las interrupciones de eint1
    EXTINT =  EXTINT | 2;                   // Limpiamos la interrupción    _
	VICVectAddr = 0;
    cola_guardar_eventos(evento_eint1, 0);  // Guardamos un evento en la cola indicando la pulsación
	
    
}

// Interrupción del botón 2
void eint2_ISR (void) __irq {
    VICIntEnClr = VICIntEnClr | 0x00010000; // Deshabilitamos las interrupciones de ein2
    EXTINT =  EXTINT | 4;                   // Limpiamos la interrupción         
    VICVectAddr = 0;
    cola_guardar_eventos(evento_eint2, 0); // Guardamos un evento en la cola indicando la pulsación

}

// Inicialización de los botones
void button_init(void) {
    EXTINT = 6; // Limpia flags de interrupción de eint1 y 2
    PINSEL0 |= 0xA0000000;
    VICVectAddr2 = (unsigned long)eint1_ISR;
    VICVectAddr3 = (unsigned long)eint2_ISR;
    VICVectCntl2 = 0x20 | 15;
    VICVectCntl3 = 0x20 | 16;                   
    VICIntEnable = VICIntEnable | 0x00018000; // Habilitamos las interrupciones EXTINT 1 y 2
}
