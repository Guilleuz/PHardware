#include "cola.h"
#include "gestor_alarmas.h"
#include "gestor_energia.h"
#include "Gestor_IO.h"
#include "botones.h"
#include "gestor_pulsacion.h"
#include "timer0.h"
#include "timer1.h"
#include "sudoku_2021.h"
#include "RTC.h"
#include "watchdog.h"
#include "gestor_interrupciones.h"
#include "gestor_lineaserie.h"

// Estado del procesador
#define APAGADO 0
#define ENCENDIDO 1



int main(void) {
	int estado = ENCENDIDO;
	// Init linea serie
	gestor_ls_init();
	
	// Inicializa watchdog
	WD_init(20);
	
    // Inicializamos el timer1
    temporizador_iniciar();
    temporizador_empezar();

    // Inicializamos el timer0 (50ms)
    temporizador_periodico(5);

    // Inicializamos el gestor io
    gestor_io_init();
	
	// Inicializamos los botones
	button_init();

    // Inicializamos el juego
    sudoku_iniciar();

    // Inicializamos alarma de 15s de inactividad
    uint32_t alarma = evento_power_down << 24;
    alarma |=  0x00803a98;
    cola_guardar_eventos(evento_set_alarma, alarma);

    // Bucle de juego
    while (1) {
        if(cola_hay_nuevos()) {
            // Si hay un nuevo evento lo procesamos
            struct Evento e = cola_ultimo_evento();
			WD_feed(); // cada vez que lea un evento alimenta al WD
            switch (e.ID) {
                case evento_timer0:
                    // Disparamos las alarmas períodicas
                    gestor_disparar_alarmas();
                    break;
                case evento_set_alarma:
                    // Programamos una nueva alarma períodica
                    gestor_set_alarma(e.datosAux);
                    break;
                case evento_eint1:
                    // Gestionamos la pulsación de eint1
					if (estado == ENCENDIDO) {
						// Tratamos la pulsacion
                        sudoku_pulsacion_1();
					} else {
						// El procesador estaba apagado, la pulsación no cuenta para el juego
						estado = ENCENDIDO;
					}
                    pulsacion_nueva_pulsacion_1();
                    // Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                case evento_eint2:
                    // Gestionamos la pulsación de eint2
					if (estado == ENCENDIDO) {
						// Tratamos la pulsacion
                        sudoku_pulsacion_2();
					} else {
						// El procesador estaba apagado, la pulsación no cuenta para el juego
						estado = ENCENDIDO;
					}
					pulsacion_nueva_pulsacion_2();
                    // Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                case evento_nuevo_caracter:
                    gestor_io_nuevo_char(e.datosAux);
					// Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
					break;
                case evento_empezar_juego:
                    sudoku_nuevo();
                    break;
                case evento_reset_juego:
                    sudoku_reset();
                    break;
                case evento_jugada:
                    sudoku_jugada(e.datosAux);
                    break;
                case evento_alarma_eint1:
                    // Comprobamos si eint1 sigue pulsado
                    pulsacion_actualizar_estado_1();
                    break;
                case evento_alarma_eint2:
                    // Comprobamos si eint2 sigue pulsado
                    pulsacion_actualizar_estado_2();
                    break;
                case evento_timeout:
                    sudoku_timeout();
                    break;
                case evento_encender_led:
                    // Encendemos el led de validez
                    gestor_io_led_validez(1);
                    break;
                case evento_apagar_led:
                    // Apagamos el led de validez
                    gestor_io_led_validez(0);
                    break;
                case evento_encender_idle:
                {
                    // Encendemos el led de idle
                    gestor_io_latido(1);
                    uint32_t alarma = evento_apagar_idle << 24;
                    alarma |=  0x00000064; // Apagamos el led en 100 ms
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                }
                case evento_apagar_idle:
                { 
                    // Apagamos el led de idle
                    gestor_io_latido(0);
                    break;
                }
                case evento_power_down:
                    // Pasamos al modo power down
                    estado = APAGADO;
                    gestor_energia_power_down();
                    break;
            }
        }
        else {
            // Si no entramos en estado idle
            gestor_energia_idle();
        }
    }
}
