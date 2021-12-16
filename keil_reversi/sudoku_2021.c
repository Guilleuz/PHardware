#include <stddef.h>
#include <stdio.h>
#include "sudoku_2021.h"
#include "cola.h"
#include "Gestor_IO.h"
#include "eventos.h"
#include "tableros.h"
#include "timer1.h"
#include "gestor_lineaserie.h"
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

static enum estado_sudoku estado;

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

void sudoku_to_string(CELDA sudoku[NUM_FILAS][NUM_COLUMNAS], char *cadena) {
    static char separador[100] = "+ - - + - - + - - ++ - - + - - + - - ++ - - + - - + - - +\n";
    strcpy(cadena, "\n");  
		char linea[1000];
		char celda[100];
    for (int i = 0; i < 9; i++) {
        strcat(cadena, separador);
        if (i % 3 == 0) strcat(cadena, separador);
        for(int j = 0; j < 9; j++) {
            strcpy(celda, "");
            char cValor;
            char cTipo;
            int valor = celda_leer_valor(sudoku[i][j]);
            
            if (valor != 0) cValor = valor + '0';
            else cValor = ' ';

            if (celda_es_pista(sudoku[i][j])) cTipo = 'P';
            else if (celda_es_error(sudoku[i][j])) cTipo = 'E';
            else cTipo = ' ';
            sprintf(celda, "|  %c%c ", cValor, cTipo); 

            if (j % 3 == 2 && j != 8) {
                strcat(celda, "|");
            }
            strcat(linea, celda);
        }

        strcat(cadena, linea);
        strcat(cadena, "|\n");
		strcpy(linea, "");
    }

    strcat(cadena, separador);

	static char candidatosString[50];
    for (int i = 0; i < 9; i++) {
        strcat(cadena, "\n");
        for (int j = 0; j < 9; j++) {
            if (celda_leer_valor(sudoku[i][j]) == 0) {
                int candidatos = celda_leer_candidatos(sudoku[i][j]);
                sprintf(linea, "%d-%d ", i, j);
                for (int h = 0; h < 9; h++) {
                    if ((candidatos & (1 << h)) == 0) {
                        sprintf(candidatosString, "%d", h+1);
                        strcat(linea, candidatosString);
                    }
                }
                strcat(linea, "\t\t\t");
                strcat(cadena, linea);
            }
        }
    }
}

void candidatos_to_string(CELDA sudoku[NUM_FILAS][NUM_COLUMNAS], char *cadena) {
    static char linea[1000];
	static char candidatosString[50];
    for (int i = 0; i < 9; i++) {
        strcat(cadena, "\n");
        for (int j = 0; j < 9; j++) {
            if (celda_leer_valor(sudoku[i][j]) == 0) {
                int candidatos = celda_leer_candidatos(sudoku[i][j]);
                sprintf(linea, "%d-%d ", i, j);
                for (int h = 0; h < 9; h++) {
                    if ((candidatos & (1 << h)) == 0) {
                        sprintf(candidatosString, "%d", h+1);
                        strcat(linea, candidatosString);
                    }
                }
                strcat(linea, "\t\t\t");
                strcat(cadena, linea);
            }
        }
    }
}
static char cadenaSudoku[5000];
static char cadenaInicio[500] = "\t\t\tSUDOKU\nIntroduzca '#NEW!' para comenzar partida o pulse uno de los botones\nIntroduzca '#RST!' para finalizar\nUna vez en la partida, introduzca '#FCVS!' para realizar una jugada:\n   siendo 'F' la Fila, 'C' la columna, 'V' el valor de la celda\n   y 'S' el resultado de aplicar modulo 8 a la suma F + C + V.\nSi la jugada es correcta, tendra 3s para confirmarla o cancelarla.";
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
    gestor_ls_enviar_cadena(cadenaInicio);
    // Establecemos una alarma períodica, para actualizar el juego cada 200ms
    uint32_t alarma = evento_actualizar_juego << 24;
    alarma |= 0x008000c8; 
    cola_guardar_eventos(evento_set_alarma, alarma);
}

// Tratamiento de la pulsación del botón 1
// Al ser pulsado, se introducirá un valor en la celda
void sudoku_pulsacion_1(void) {
    char candidatos[500];
    switch (estado) {
        case INICIO:
            estado = JUGANDO;
            // mostrar tablero
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_ls_enviar_cadena(cadenaSudoku);
            break;
        case JUGADA_INTRODUCIDA:
        {
            // Confirmamos jugada
            estado = JUGANDO;

            // Cancelamos el timeout
            uint32_t alarmaTimeout = evento_timeout << 24;
            alarmaTimeout |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

            // Desactivamos la alarma del led
            uint32_t alarmaLED = evento_encender_idle << 24;
            alarmaLED |=  0x00800000; 
            cola_guardar_eventos(evento_set_alarma, alarmaLED);
            break;
        }
        case FINAL: 
            estado = JUGANDO;
            break;
    }
}

// Tratamiento de la pulsación del botón 2
// Al ser pulsado, se borrará el valor de la celda
void sudoku_pulsacion_2(void) {
	char cancelado[100] = "\n--- JUGADA CANCELADA ---\n";
    switch (estado) {
        case INICIO:
            estado = JUGANDO;
            // mostrar tablero
            sudoku_to_string(tablero, cadenaSudoku);
            gestor_ls_enviar_cadena(cadenaSudoku);
            break;
        case JUGADA_INTRODUCIDA:
        {
            // Cancelamos jugada

            // Cancelamos el timeout
            uint32_t alarmaTimeout = evento_timeout << 24;
            alarmaTimeout |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

            // Desactivamos la alarma del led
            uint32_t alarmaLED = evento_encender_idle << 24;
            alarmaLED |=  0x00000000; 
            cola_guardar_eventos(evento_set_alarma, alarmaLED);

            gestor_ls_enviar_cadena(cancelado);
            estado = JUGANDO;  
            break;   
        }
        case FINAL: 
            estado = JUGANDO;
            break;
    }
}

static uint8_t filaJugada;
static uint8_t columnaJugada;
static uint8_t valorJugada;
void sudoku_introducir_jugada(uint8_t fila, uint8_t columna, uint8_t valor) {
    filaJugada = fila;
    columnaJugada = columna;
    valorJugada = valor;
    
    // Previsualizacion
    sudoku_to_string(tablero, cadenaSudoku);
    char preVis[200];
    sprintf(preVis, "\nNUEVA JUGADA: Posicion de la celda:\n  Fila:%d\n  Columna:%d\nValor de la celda: %d\n", fila, columna, valor);
    strcat(cadenaSudoku, preVis);
    gestor_ls_enviar_cadena(cadenaSudoku);
    
    // Timeout de 3 segundos
    uint32_t alarmaTimeout = evento_timeout << 24;
    alarmaTimeout |=  0x00000bb8; 
    cola_guardar_eventos(evento_set_alarma, alarmaTimeout);

    // Activar led (parpadear)
    // Programamos una alarma que cada 250ms encienda el led del latido de idle
    uint32_t alarmaLED = evento_encender_idle << 24;
    alarmaLED |=  0x008000fa; 
    cola_guardar_eventos(evento_set_alarma, alarmaLED);

    estado = JUGADA_INTRODUCIDA;
}

void sudoku_jugada(uint32_t auxData) {
    uint8_t fila, columna, valor, check;
    if (estado == JUGANDO) {
        check = auxData >> 24;
        fila = auxData >> 16;
        columna = auxData >> 8;
        valor = auxData;
        // True si la celda y su valor estan dentro del rango admisible, y si la celda no es una pista
        int enRango = fila <= 8 && columna<= 8 && valor <= 9 && !celda_es_pista(tablero[fila][columna]);
		if (enRango) {
			int i = 1;
		} else {
			int i = 2;
		}
        // Checksum correcto
        if (check == ((fila + columna + valor) % 8) && enRango) {
            sudoku_introducir_jugada(fila, columna, valor);
        }
    }
}

void sudoku_nuevo(void) {
    if (estado == INICIO || estado == FINAL) {
        // mostrar el tablero
        estado = JUGANDO;
        sudoku_to_string(tablero, cadenaSudoku);
        gestor_ls_enviar_cadena(cadenaSudoku);
    }
}

void sudoku_reset(void) {
    if (estado == JUGANDO || estado == JUGADA_INTRODUCIDA) {
        estado = FINAL;
    }
}

void sudoku_timeout() {
    if (estado == JUGADA_INTRODUCIDA) {
        // Se cancela la jugada
        estado = JUGANDO;
        char cancelado[100] = "\n--- JUGADA CANCELADA ---\n";
        gestor_ls_enviar_cadena(cancelado);

        // Desactivamos la alarma del led
        uint32_t alarmaLED = evento_encender_idle << 24;
        alarmaLED |=  0x00800000; 
        cola_guardar_eventos(evento_set_alarma, alarmaLED);
    } 
}
