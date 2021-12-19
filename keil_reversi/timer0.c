#include "timer0.h"
#include "cola.h"
#include "eventos.h"
#include <LPC210X.H>               

// Rutina de interrupción del timer0
void timer0_ISR (void) __irq {
    cola_guardar_eventos(evento_timer0, 0);
    T0IR = 1;                              // Limpia flag de interrupción
    VICVectAddr = 0;                       // Acknowledge Interrupt
}

// Inicializamos el timer0 para que interrumpa cada periodo(ms)
void temporizador_periodico(int periodo) {
	T0MR0 = (60000 * periodo) - 1; 
    T0MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    T0TCR = 1;                     // Timer0 Enable            
    // Activamos las interrupciones del timer0, configuradas como FIQ     
    VICIntEnable = VICIntEnable | 0x00000010;                  
    VICIntSelect |= 0x00000010; 
}
    
