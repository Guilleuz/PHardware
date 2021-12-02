#include <stddef.h>
#include <stdio.h>
#include "sudoku_2021.h"
#include "cola.h"
#include "Gestor_IO.h"
#include "eventos.h"
#include "tableros.h"
#include "timer1.h"

// El sudoku guardará un estado que indicará si hay una partida en juego o
// si se va a producir un reinicio.
// En caso de que vayamos a reiniciar, el juego permanecerá inactivo hasta que 
// reciba una pulsación de botón, momento en el que se reiniciará el tablero
// y se dará comienzo al nuevo juego,
#define JUGANDO 0
#define REINICIO 1
static int estado;

static uint8_t fila, columna, ultimoValor;     // Últimos valores de fila, columna y celda leídos de la entrada
static CELDA tablero[NUM_FILAS][NUM_COLUMNAS]; // Tablero en juego

/* *****************************************************************************
 * propaga el valor de una determinada celda en C
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y región */
/* Recibe como parametro la cuadricula, y la fila y columna de
 * la celda a propagar; no devuelve nada
 */
void candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
	uint8_t fila, uint8_t columna)
{
    uint8_t j, i , init_i, init_j, end_i, end_j;
    /* puede ayudar esta "look up table" a mejorar el rendimiento */
    const uint8_t init_region[NUM_FILAS] = {0, 0, 0, 3, 3, 3, 6, 6, 6};

    /* valor que se propaga */
    uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

    /* recorrer fila descartando valor de listas candidatos */
    for (j=0;j<NUM_FILAS;j++) {
			celda_eliminar_candidato(&cuadricula[fila][j],valor);
		}

    /* recorrer columna descartando valor de listas candidatos */
    for (i=0;i<NUM_FILAS;i++) {
			celda_eliminar_candidato(&cuadricula[i][columna],valor);
		}

    /* determinar fronteras región */
    init_i = init_region[fila];
    init_j = init_region[columna];
    end_i = init_i + 3;
    end_j = init_j + 3;

    /* recorrer region descartando valor de listas candidatos */
    for (i=init_i; i<end_i; i++) {
      for(j=init_j; j<end_j; j++) {
	      celda_eliminar_candidato(&cuadricula[i][j],valor);
	    }
    }
}

/* *****************************************************************************
 * calcula todas las listas de candidatos (9x9)
 * necesario tras borrar o cambiar un valor (listas corrompidas)
 * retorna el numero de celdas vacias */

/* Init del sudoku en codigo C invocando a propagar en C
 * Recibe la cuadricula como primer parametro
 * y devuelve en celdas_vacias el número de celdas vacias
 */
static int candidatos_actualizar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
    int celdas_vacias = 0;
    uint8_t i;
    uint8_t j;

	//borrar todos los candidatos
    for(i = 0; i < NUM_FILAS; i++) {
        for (j = 0; j < NUM_FILAS; j++) {
            celda_inicializar_candidatos(&cuadricula[i][j]);
        }
    }

	//recalcular candidatos de las celdas vacias calculando cuantas hay vacias
    for(i = 0; i < NUM_FILAS; i++) {
        for (j = 0; j < NUM_FILAS; j++) {
            if (celda_leer_valor(cuadricula[i][j]) != 0) {
                candidatos_propagar_c(cuadricula, i, j);
            }
            else celdas_vacias = celdas_vacias + 1;
        }
    }

	//retornar el numero de celdas vacias
	return celdas_vacias;
}


/**************************************************************/
// Funciones para el control del juego						   /
/**************************************************************/


// Inicializamos la partida
void sudoku_iniciar(void) {
    estado = JUGANDO;
    fila = 0;
    columna = 0;
    ultimoValor = 0;

    // Inicializamos el tablero
    for (int i = 0; i < NUM_FILAS; i++) {
        for (int j = 0; j < NUM_COLUMNAS; j++)
            tablero[i][j] = cuadricula_C_C[i][j];
    }
    
    // Inicializamos los candidatos del tablero
	candidatos_actualizar_c(tablero);
    // Establecemos una alarma períodica, para actualizar el juego cada 200ms
    uint32_t alarma = evento_actualizar_juego << 24;
    alarma |= 0x008000c8; 
    cola_guardar_eventos(evento_set_alarma, alarma);
}

// Actualizamos la visualización de la celda
// Devolverá 1 en caso de que haya habido un cambio en los pines de entrada
// Devolverá 0 en caso contrario
int sudoku_actualizar(void) {
    if (estado == JUGANDO) {
        // Comprobamos el estado de los pines únicamente si hay una partida en juego
        uint8_t filaIN, columnaIN, valorIN;
        filaIN = gestor_io_leer_fila();
        columnaIN = gestor_io_leer_col();
        valorIN = gestor_io_leer_valor();

        // Si la celda está dentro de rango
        if (filaIN <= 8 && columnaIN <= 8) {
            gestor_io_visualizar_celda(celda_leer_valor(tablero[filaIN][columnaIN]), celda_leer_candidatos(tablero[filaIN][columnaIN]));
            
            // Si hemos cambiado de celda, encendemos o apagamos el led dependiendo de si es pista o no
            if (fila != filaIN || columna != columnaIN) {
                if (celda_es_pista(tablero[filaIN][columnaIN])) cola_guardar_eventos(evento_encender_led, 0);
                else cola_guardar_eventos(evento_apagar_led, 0);
            }
        }

        // SOLO CAMBIAR EL LED CUANDO CAMBIEMOS DE CELDA
        
        // Si se ha detectado algun cambio en los pines de entrada
        if (fila != filaIN || columna != columnaIN || ultimoValor != valorIN) {
            fila = filaIN;
            columna = columnaIN;
            ultimoValor = valorIN;
            return 1;
        }
        return 0;
    }
    return 0;
}

// Tratamiento de la pulsación del botón 1
// Al ser pulsado, se introducirá un valor en la celda
void sudoku_pulsacion_1(void) {
    if (estado == JUGANDO) {    
        int filaIN, columnaIN, valorIN;
        filaIN = gestor_io_leer_fila();
        columnaIN = gestor_io_leer_col();
        valorIN = gestor_io_leer_valor();
        
        // Si se introduce un 0 en la columna 0, fila 0, reiniciamos el juego
        if (filaIN == 0 && columnaIN == 0 && valorIN == 0) {
            // Reiniciamos el juego
            estado = REINICIO;
        }
        // Si la celda no está fuera de rango, el valor es válido y no es una pista
        else if (filaIN <= 8 && columnaIN <= 8 && valorIN <= 9 && valorIN >= 1 && !celda_es_pista(tablero[filaIN][columnaIN])) {
            int candidatos = celda_leer_candidatos(tablero[filaIN][columnaIN]);
            uint8_t valorAnterior = celda_leer_valor(tablero[filaIN][columnaIN]);
            celda_poner_valor(&tablero[filaIN][columnaIN], valorIN);
            
            if (valorAnterior == 0) {
                // Si la celda está vacía, no será necesario actualizar los candidatos
                int tiempo = clock_gettime();
                candidatos_propagar_c(tablero, filaIN, columnaIN);
                tiempo = clock_gettime() - tiempo;
            } 
            else {
                // Si la celdía ya tenía valor, recalculamos los candidatos
                int tiempo = clock_gettime();
                candidatos_actualizar_c(tablero);
                tiempo = clock_gettime() - tiempo;
            }
            
            if ((candidatos & (1 << (valorIN - 1))) == 0) {
                // Si el valor introducido era un posible candidato de la celda
                // La jugada será válica
                celda_desmarcar_error(&tablero[filaIN][columnaIN]);
                
                // Encendemos el led y lo apagamos después de 1s
                cola_guardar_eventos(evento_encender_led, 0);
                uint32_t alarma = evento_apagar_led << 24;
                alarma |= 0x000003e8; 
                cola_guardar_eventos(evento_set_alarma, alarma);
            }
            else {
                // En caso contrario, la jugada es errónea
                celda_marcar_error(&tablero[filaIN][columnaIN]);
            }
        }
    }
    else {
        // Si recibimos una pulsación y no hay una partida en juego, lo reiniciamos
        sudoku_iniciar();
    }
}

// Tratamiento de la pulsación del botón 2
// Al ser pulsado, se borrará el valor de la celda
void sudoku_pulsacion_2(void) {
    if (estado == JUGANDO) {
        uint8_t filaIN, columnaIN;
        filaIN = gestor_io_leer_fila();
        columnaIN = gestor_io_leer_col();
        // Si la celda no está fuera de rango, no está vacía y no es una pista
        if (filaIN <= 8 && columnaIN <= 8 && celda_leer_valor(tablero[filaIN][columnaIN]) != 0 && !celda_es_pista(tablero[filaIN][columnaIN])) {
            celda_desmarcar_error(&tablero[filaIN][columnaIN]);
            celda_poner_valor(&tablero[filaIN][columnaIN], 0);
            int tiempo = clock_gettime();
            // Actualizamos los candidatos
            candidatos_actualizar_c(tablero);
            tiempo = clock_gettime() - tiempo;
            
            // Encendemos el led y lo apagamos después de 1s
            cola_guardar_eventos(evento_encender_led, 0);
            uint32_t alarma = evento_apagar_led << 24;
            alarma |= 0x000003e8;
            cola_guardar_eventos(evento_set_alarma, alarma);
        }
    }
    else {
        // Si recibimos una pulsación y no hay una partida en juego, lo reiniciamos
        sudoku_iniciar();
    }
}

