#include "timer0.h"
#include "cola.h"
#include "eventos.h"
#include <LPC210X.H>                            // LPC21XX Peripheral Registers

void timer0_ISR (void) __irq {
    cola_guardar_eventos(evento_timer0, 0);
    T0IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                       // Acknowledge Interrupt
}

void temporizador_periodico(int periodo) {
	T0MR0 = (60000 * periodo) - 1; // Interrumpe cada periodo(ms)
    T0MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    T0TCR = 1;                     // Timer0 Enable
    // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
	VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
	// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
	VICVectCntl0 = 0x20 | 4;                   
    VICIntEnable = VICIntEnable | 0x00000010;                  // Enable Timer0 Interrupt
}
    
