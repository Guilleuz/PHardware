#include "cola.h"
#include "gestor_alarmas.h"
#include "gestor_energia.h"
#include "Gestor_IO.h"
#include "gestor_pulsacion.h"
#include "timer0.h"
#include "timer1.h"
#include "sudoku_2021.h"

#define APAGADO 0
#define ENCENDIDO 1
#define ESPERA_PULSACION 2

// maquinas de estado dibujos

/*
Sudoku interactua por API con gpio
    Cada alarma -> Sudoku mira valores del gpio, si ha cambiado, visualiza la nueva celda
    Para actualizar la visualizacion -> utilizamos la API del gpio
    Pulsaciones -> Sudoku leerá los valores del gpio, actualizará el juego y visualiza el resultado

Sudoku interactua por eventos con gpio
    Cada alarma de actualizar el juego -> GPIO mira si han cambiado los valores
        -> en tal caso generamos un nuevo evento, con los datos auxiliares de fila, columna
    Sudoku recibe el evento -> genera otro evento con los datos de la nueva celda (valor + candidatos)
    GPIO recibe el evento -> cambia la visualizacion

    Pulsacion -> se lo pasamos al GPIO -> gpio lee y genera evento
*/

int main(void) {
	int estado = ENCENDIDO;
    // Inicializamos el timer1
    temporizador_iniciar();
    temporizador_empezar();

    // Inicializamos el timer0 (50ms)
    temporizador_periodico(50);

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

    // Programamos una alarma que cada 250ms encienda el led del latido de idle
    uint32_t alarmaIdle = evento_encender_idle << 24;
    alarmaIdle |=  0x008000fa; 
    cola_guardar_eventos(evento_set_alarma, alarmaIdle);


    // Bucle de juego
    // De momento bucle infinito
    while (1) {
        if(cola_hay_nuevos()) {
            // Si hay un nuevo evento lo procesamos
            struct Evento e = cola_ultimo_evento();
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
                    // Programamos la alarma períodica del botón
					if (estado == ENCENDIDO) {
						// Tratamos la pulsacion
                        sudoku_pulsacion_1();
					} else {
						// El procesador estaba apagado, la pulsación no cuenta para el juego
						estado = ENCENDIDO;
					}
                    // Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                case evento_eint2:
                    // Gestionamos la pulsación de eint2
                    // Programamos la alarma períodica del botón
					if (estado == ENCENDIDO) {
						// Tratamos la pulsacion
                        sudoku_pulsacion_2();
					} else {
						// El procesador estaba apagado, la pulsación no cuenta para el juego
						estado = ENCENDIDO;
					}
					
                    // Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                case evento_reiniciar:
                    sudoku_iniciar();
                    //estado = ESPERA_PULSACION;
                    cola_guardar_eventos(evento_power_down, 0);
                case evento_alarma_eint1:
                    // Comprobamos si eint1 sigue pulsado
                    button_actualizar_estado_1();
                    break;
                case evento_alarma_eint2:
                    // Comprobamos si eint2 sigue pulsado
                    button_actualizar_estado_2();
                    break;
                case evento_actualizar_juego:
                    // Comprobamos valores de entrada
                    // si han cambiado, actualizamos valores de salida
                    // y reiniciamos la alarma de inactividad
					
					/* Si hay cambios en la entrada
					// Reiniciamos alarma de inactividad
                    cola_guardar_eventos(evento_set_alarma, alarma);
					*/
                    if (sudoku_actualizar()) {
                        // Reiniciamos alarma de inactividad
                        cola_guardar_eventos(evento_set_alarma, alarma);
                    }

                    break;
                case evento_encender_led:
                    gestor_io_led_validez(1);
                    break;
                case evento_apagar_led:
                    gestor_io_led_validez(0);
                    break;
                case evento_encender_idle:
                {
                    gestor_io_latido(1);
                    uint32_t alarma = evento_apagar_idle << 24;
                    alarma |=  0x00000064; // Apagamos el led en 100 ms
                    cola_guardar_eventos(evento_set_alarma, alarma);
                    break;
                }
                case evento_apagar_idle:
                { 
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
