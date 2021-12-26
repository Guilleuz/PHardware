#include <stddef.h>
#include <stdio.h>
#include "sudoku_2021.h"
#include "cola.h"
#include "Gestor_IO.h"
#include "eventos.h"
#include "tableros.h"
#include "timer1.h"
#include "gestor_lineaserie.h"
#include "RTC.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// El sudoku guardará un estado que indicará si hay una partida en juego o
// si se va a producir un reinicio.
// En caso de que vayamos a reiniciar, el juego permanecerá inactivo hasta que 
// reciba una pulsación de botón, momento en el que se reiniciará el tablero
// y se dará comienzo al nuevo juego,

enum estado_sudoku {
    INICIO,
    JUGANDO,
    JUGADA_INTRODUCIDA,
    FINAL
};

// Estado del sudoku
static enum estado_sudoku estado;

// Tiempo empleado en el cálculo de errores y candidatos
static int tiempoTotal;

// Valores de fila y columna de la última jugada
static uint8_t filaJugada;
static uint8_t columnaJugada;
// Valor anterior de la celda en la que se aplica la jugada
static uint8_t valorOriginal;

static char cadenaSudoku[5000]; // Cadena reservada para escribir tableros
// Cadena de inicio de juego
static char cadenaInicio[500] = "\t\t\tSUDOKU\nIntroduzca '#NEW!' para comenzar partida o pulse uno de los botones\nIntroduzca '#RST!' para finalizar\nUna vez en la partida, introduzca '#FCVS!' para realizar una jugada:\n   siendo 'F' la Fila, 'C' la columna, 'V' el valor de la celda\n   y 'S' el resultado de aplicar modulo 8 a la suma F + C + V.\nSi la jugada es correcta, tendra 3s para confirmarla o cancelarla.";


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
    uint8_t valorCelda;
    celda_desmarcar_error(&cuadricula[fila][columna]);
    /* recorrer fila descartando valor de listas candidatos */
    for (j=0;j<NUM_FILAS;j++) {
			celda_eliminar_candidato(&cuadricula[fila][j],valor);
            valorCelda = celda_leer_valor(cuadricula[fila][j]);
            if (valor == valorCelda && j != columna) {
                celda_marcar_error(&cuadricula[fila][columna]);
                celda_marcar_error(&cuadricula[fila][j]);
            }
		}

    /* recorrer columna descartando valor de listas candidatos */
    for (i=0;i<NUM_FILAS;i++) {
			celda_eliminar_candidato(&cuadricula[i][columna],valor);
            valorCelda = celda_leer_valor(cuadricula[i][columna]);
            if (valor == valorCelda && i != fila) {
                celda_marcar_error(&cuadricula[fila][columna]);
                celda_marcar_error(&cuadricula[i][columna]);
            }
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
            valorCelda = celda_leer_valor(cuadricula[i][j]);
            if (valor == valorCelda && (i != fila || j != columna)) {
                celda_marcar_error(&cuadricula[fila][columna]);
                celda_marcar_error(&cuadricula[i][j]);
            }
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

// Devuelve 1 si el sudoku tiene en todas sus celdas valores correctos o pistas
// y todas las pistas tienen un valor distinto de 0
// Devuelve 0 en caso contrario
static int
sudoku_es_correcto(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
    for (int i = 0; i < NUM_FILAS; i++) {
        for (int j = 0; j < NUM_FILAS; j++) {
            if (celda_es_error(cuadricula[i][j]) || celda_leer_valor(cuadricula[i][j]) == 0) return 0;
        }
    }
    
    return 1;
}

/**************************************************************/
// Funciones para imprimir el sudoku, ...					   /
/**************************************************************/

// Devuelve en cadena una visualización del sudoku y de los candidatos
void sudoku_to_string(CELDA sudoku[NUM_FILAS][NUM_COLUMNAS], char *cadena) {
    static char separador[100] = "+ - - + - - + - - ++ - - + - - + - - ++ - - + - - + - - +\n\0";
    static char linea[200] = "\0";
    static char celda[50] = "\0";
    static char candidatosString[50] = "\0";
    strcpy(cadena, "\n\n\0");  
    strcpy(linea, "");
    for (int i = 0; i < 9; i++) {
        strcat(cadena, separador);
        if (i % 3 == 0 && i > 0) strcat(cadena, separador);
        for(int j = 0; j < 9; j++) {
            strcpy(celda, "");
            char cValor;
            int valor = celda_leer_valor(sudoku[i][j]);
            int esPista = celda_es_pista(sudoku[i][j]);
            int esError = celda_es_error(sudoku[i][j]);

            if (valor != 0) cValor = valor + '0';
            else cValor = ' ';

            if (esPista && esError) sprintf(celda, "| %cPE \0", cValor);
            else if (esPista) sprintf(celda, "|  %cP \0", cValor);
            else if (esError) sprintf(celda, "|  %cE \0", cValor); 
            else sprintf(celda, "|  %c  \0", cValor); 
            
            if (j % 3 == 2 && j != 8) strcat(celda, "|\0");
            strcat(linea, celda);
        }

        strcat(cadena, linea);
        strcat(cadena, "|\n\0");
		strcpy(linea, "");
    }

    strcat(cadena, separador);
    strcat(cadena, "\nF-C Candidatos\0");

    for (int i = 0; i < 9; i++) {
        strcat(cadena, "\n\0");
        for (int j = 0; j < 9; j++) {
            if (celda_leer_valor(sudoku[i][j]) == 0) {
                int candidatos = celda_leer_candidatos(sudoku[i][j]);
                sprintf(linea, "%d-%d \0", i, j);
                for (int h = 0; h < 9; h++) {
                    if ((candidatos & (1 << h)) == 0) {
                        sprintf(candidatosString, "%d\0", h+1);
                        strcat(linea, candidatosString);
                    }
                }
                sprintf(candidatosString, "%-13s\0", linea);
                strcat(cadena, candidatosString);
            } else {
                sprintf(linea, "%d-%d ---\0", i, j);
                sprintf(candidatosString, "%-13s\0", linea);
                strcat(cadena, candidatosString);
            }
        }
    }
}

// Imprime un mensaje de final de partida
// Indica el tiempo total de juego en minutos y segundos,
// así como el tiempo empleado un cálculos en microsegundos
// Indica también el motivo del final de la partida
void sudoku_imprime_final(char *motivoFinal) {
    static char mensajeFinal[300];
    static char tiempos[300];
    
	strcpy(mensajeFinal, "\n--- FIN DE LA PARTIDA ---\nMotivo: ");
    strcat(mensajeFinal, motivoFinal);
    sprintf(tiempos, "\nTiempo de juego total: %d minutos y %d segundos\nTiempo de calculo de candidatos: %d microsegundos\n\n", RTC_leer_minutos(), RTC_leer_segundos(), tiempoTotal);
    strcat(mensajeFinal, tiempos);
    gestor_io_enviar_cadena(mensajeFinal);
	
	sudoku_iniciar();
}

/**************************************************************/
// Funciones para el control del juego						   /
/**************************************************************/

// Inicializamos la partida
void sudoku_iniciar(void) {
    estado = INICIO;
    
    // Inicializamos el tablero
    for (int i = 0; i < NUM_FILAS; i++) {
        for (int j = 0; j < NUM_COLUMNAS; j++)
            tablero[i][j] = cuadricula_C_C[i][j];
    }
    
    // Inicializamos los candidatos del tablero
	candidatos_actualizar_c(tablero);
    gestor_io_enviar_cadena(cadenaInicio);
    
    // Establecemos una alarma períodica, para actualizar el juego cada 200ms
    uint32_t alarma = evento_actualizar_juego << 24;
    alarma |= 0x008000c8; 
    cola_guardar_eventos(evento_set_alarma, alarma);
}

void sudoku_iniciar_contadores(void) {
	// Inicializamos el contador de tiempo de calculo y el RTC
    tiempoTotal = 0;
    RTC_init();
}

// Introducir jugada en el tablero actual
void sudoku_introducir_jugada(uint8_t fila, uint8_t columna, uint8_t valor) {
    
    // Realizamos la jugada
    int candidatos = celda_leer_candidatos(tablero[fila][columna]);
    uint8_t valorAnterior = celda_leer_valor(tablero[fila][columna]);
    celda_poner_valor(&tablero[fila][columna], valor);
    
    // Si queremos borrar la celda
    if (valor == 0) {
        celda_desmarcar_error(&tablero[fila][columna]);
    }


    int tiempo;
    if (valorAnterior == 0 && valor != 0) {
        // Si la celda estaba vacía, no será necesario actualizar los candidatos de todo el tablero
        tiempo = clock_gettime();
        candidatos_propagar_c(tablero, fila, columna);
        tiempo = clock_gettime() - tiempo;
    } 
    else if (valorAnterior != 0) {
        // Si la celda ya tenía valor, recalculamos los candidatos
        tiempo = clock_gettime();
        candidatos_actualizar_c(tablero);
        tiempo = clock_gettime() - tiempo;
    }
    // Contabilizamos el tiempo de cálculo de errores y candidatos
    tiempoTotal += tiempo;

    // Mostramos el tablero resultante
    sudoku_to_string(tablero, cadenaSudoku);
    gestor_io_enviar_cadena(cadenaSudoku);
}

// Procesamos un evento de jugada recibido
void sudoku_jugada(uint32_t auxData) {
    uint8_t fila, columna, valor, check;
    if (estado == JUGANDO) {
        check = auxData >> 24;
        fila = auxData >> 16;
        columna = auxData >> 8;
        valor = auxData;
        // True si la celda y su valor estan dentro del rango admisible, y si la celda no es una pista
        int enRango = fila <= 8 && columna<= 8 && valor <= 9 && !celda_es_pista(tablero[fila][columna]);
		
        // Checksum correcto
        if (check == ((fila + columna + valor) % 8) && enRango) {
            // Si la jugada es correcta

            // Mostramos la nueva jugada
            char preVis[200];
            sprintf(preVis, "\n\nNUEVA JUGADA: Posicion de la celda:\n  Fila:%d\n  Columna:%d\n  Valor de la celda: %d\n", fila, columna, valor);
            gestor_io_enviar_cadena(preVis);
            
            // Guardamos los valores de fila y columna de la jugada
            filaJugada = fila;
            columnaJugada = columna;
            // Guardamos el valor de la celda antes de la jugada
            valorOriginal = celda_leer_valor(tablero[fila][columna]);
            
            // Introducimos la jugada
            sudoku_introducir_jugada(fila, columna, valor);
            
            // Introducimos un timeout de 3 segundos
            // Si la jugada no se confirma en esos 3 segundos, será cancelada
            uint32_t alarmaTimeout = evento_timeout << 24;
            //alarmaTimeout |=  0x00000bb8; 
            alarmaTimeout |= 0x00002710; // 10 segundos
            cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

            // Activar led (parpadear)
            // Programamos una alarma que cada 250ms encienda el led del latido de idle
            uint32_t alarmaLED = evento_encender_idle << 24;
            alarmaLED |=  0x008000fa; 
            cola_guardar_eventos(evento_set_alarma, alarmaLED);

            // Pasamos a estado JUGADA INTRODUCIDA
            estado = JUGADA_INTRODUCIDA;
        }
    }
}

// Tratamiento de la pulsación del botón 1
void sudoku_pulsacion_1(void) {
    switch (estado) {
        case INICIO:
		{
            // Comienza el juego
            estado = JUGANDO;
			
			sudoku_iniciar_contadores();

            // Mostramos el tablero inicial
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_io_enviar_cadena(cadenaSudoku);
            break;
		}
        case JUGADA_INTRODUCIDA:
        {
            // Confirmamos la jugada
            char mensajeConfirmacion[200] = "\n\n--- JUGADA CONFIRMADA ---";
            gestor_io_enviar_cadena(mensajeConfirmacion);

            // Pasamos a estado JUGANDO
            estado = JUGANDO;

            // Cancelamos el timeout
            uint32_t alarmaTimeout = evento_timeout << 24;
            alarmaTimeout |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

            // Desactivamos la alarma del led
            uint32_t alarmaLED = evento_encender_idle << 24;
            alarmaLED |=  0x00800000; 
            cola_guardar_eventos(evento_set_alarma, alarmaLED);

            // Comprobamos si el tablero es correcto y completo
            // Si lo es pasamos al estado FINAL
            if (sudoku_es_correcto(tablero)) {
                sudoku_imprime_final("Tablero correcto");
                estado = FINAL;
            }
            break;
        }
        case FINAL:
		{
		    // Mostramos el tablero inicial
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_io_enviar_cadena(cadenaSudoku);

            // Pasamos a estado JUGANDO
            estado = JUGANDO;
            break;
		}
    }
}

// Tratamiento de la pulsación del botón 2
void sudoku_pulsacion_2(void) {
    char cancelado[100] = "\n\n--- JUGADA CANCELADA ---\n";
    switch (estado) {
        case INICIO:
		{
            // Comienza el juego
            estado = JUGANDO;
			sudoku_iniciar_contadores();
			
            // Mostramos el tablero inicial
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_io_enviar_cadena(cadenaSudoku);
            break;
		}
        case JUGADA_INTRODUCIDA:
        {
			gestor_io_enviar_cadena(cancelado);
            // Cancelamos la jugada
            sudoku_introducir_jugada(filaJugada, columnaJugada, valorOriginal);

            // Cancelamos el timeout
            uint32_t alarmaTimeout = evento_timeout << 24;
            alarmaTimeout |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

            // Desactivamos la alarma del led
            uint32_t alarmaLED = evento_encender_idle << 24;
            alarmaLED |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaLED);
            
            // Pasamos a estado JUGANDO
            estado = JUGANDO;  
            break;   
        }
        case FINAL:
		{
			// Mostramos el tablero inicial 
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_io_enviar_cadena(cadenaSudoku);

            // Pasamos a estado JUGANDO
            estado = JUGANDO;
            break;
		}
    }
}

// Comportamiento al iniciar nueva partida
void sudoku_nuevo(void) {
    if (estado == INICIO || estado == FINAL) {
        // mostrar el tablero inicial
        sudoku_to_string(tablero, cadenaSudoku);
        gestor_io_enviar_cadena(cadenaSudoku);
		
        // Pasamos a estado JUGANDO
        estado = JUGANDO;
		sudoku_iniciar_contadores();
    }
}

// Comportamiento al reiniciar sudoku
void sudoku_reset(void) {
    if (estado == JUGANDO) {
        sudoku_imprime_final("reset");
		
        // Pasamos a estado FINAL
        estado = FINAL;
    }
}

// Tratamiento timeout de confirmación
void sudoku_timeout() {
    if (estado == JUGADA_INTRODUCIDA) {
        // Se cancela la jugada
        estado = JUGANDO;
        char cancelado[100] = "\n\n--- JUGADA CANCELADA ---\n";
        gestor_io_enviar_cadena(cancelado);

        // Cancelamos jugada
        sudoku_introducir_jugada(filaJugada, columnaJugada, valorOriginal);

        // Desactivamos la alarma del led
        uint32_t alarmaLED = evento_encender_idle << 24;
        alarmaLED |=  0x00800000; 
        cola_guardar_eventos(evento_set_alarma, alarmaLED);
    } 
}
