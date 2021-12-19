#include "gestor_pulsacion.h"
#include "cola.h"
#include "eventos.h"
#include "gestor_alarmas.h"
#include <LPC210X.H>

static int estado_pulsacion_1 = NO_PULSADO;
static int estado_pulsacion_2 = NO_PULSADO;

// Registramos una pulsación del botón 1
void pulsacion_nueva_pulsacion_1(void) {
	estado_pulsacion_1 = PULSADO;

	// Programamos una alarma períodica
    uint32_t alarma = evento_alarma_eint1 << 24;
    alarma |= 0x00800064; // Alarma períodica cada 100 ms 
    cola_guardar_eventos(evento_set_alarma, alarma);
}

// Registramos una nueva pulsación del botón 2
void pulsacion_nueva_pulsacion_2(void) {
	estado_pulsacion_2 = PULSADO;

	// Programamos una alarma períodica
    uint32_t alarma = evento_alarma_eint2 << 24;
    alarma |= 0x00800064; // Alarma períodica cada 100 ms 
    cola_guardar_eventos(evento_set_alarma, alarma);
}

// Actualización del estado del botón 1
void pulsacion_actualizar_estado_1(void) {
	EXTINT =  EXTINT | 2;        // clear interrupt flag de EINT1     
	if ((EXTINT & 2) == 2 && estado_pulsacion_1 == PULSADO){      // si el boton esta pulsado, la instruccion de arriba no hara nada y EXTINT valdra 1. Si el boton no esta pulsado valdra 0
		estado_pulsacion_1 = PULSADO;
	}
	else{
		estado_pulsacion_1 = NO_PULSADO;
		// Cancelamos la alarma
		uint32_t evento = 0;
		evento = evento_alarma_eint1 << 24;
		evento |= 0x00800000;
		gestor_set_alarma(evento);
		// si no esta pulsado se habilitan las interrupciones (antes ya se ha limpiado el de EXTINT)
		VICIntEnable = VICIntEnable | 0x00008000; // Enable EXTINT1 Interrupt (la interrupcion del boton se deshabilita a si misma, al terminar la pulsacion hay ue volver a habilitarla)
	}
}

// Actualización del estado del botón 2
void pulsacion_actualizar_estado_2(void){
	EXTINT =  EXTINT | 4;        // clear interrupt flag de EINT2   
	if ((EXTINT & 4) == 4 && estado_pulsacion_2 == PULSADO){ // si el boton esta pulsado, la instruccion de arriba no hara nada y EXTINT valdra 1. Si el boton no esta pulsado valdra 0
		estado_pulsacion_1 = PULSADO;
	}
	else{
		estado_pulsacion_1 = NO_PULSADO;
		// Cancelamos la alarma
		uint32_t evento = 0;
		evento = evento_alarma_eint2 << 24;
		evento |= 0x00800000;
		gestor_set_alarma(evento);
		// si no esta pulsado se habilitan las interrupciones (antes ya se ha limpiado el de EXTINT)
		VICIntEnable = VICIntEnable | 0x00010000; // Enable EXTINT2 Interrupt (la interrupcion del boton se deshabilita a si misma, al terminar la pulsacion hay ue volver a habilitarla)
	}
}
