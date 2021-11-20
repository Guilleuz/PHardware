/* guarda para evitar inclusiones m�ltiples ("include guard") */
#ifndef SUDOKU_H_2021
#define SUDOKU_H_2021

#include <inttypes.h>
#include "celda.h"


/* Tama�os de la cuadricula */
/* Se utilizan 16 columnas para facilitar la visualizaci�n */
enum {NUM_FILAS = 9,
      PADDING = 7,
      NUM_COLUMNAS = NUM_FILAS + PADDING};

/* Definiciones para valores muy utilizados */
enum {FALSE = 0, TRUE = 1};

typedef uint16_t CELDA;
/* La informaci�n de cada celda est� codificada en 16 bits
 * con el siguiente formato, empezando en el bit m�s significativo (MSB):
 * 1 bit  PISTA
 * 1 bit  ERROR
 * 1 bit  no usado
 * 9 bits vector CANDIDATOS (0: es candidato, 1: no es candidato)
 * 4 bits VALOR
 */

/* declaracion de funciones visibles en el exterior */
int sudoku9x9(CELDA cuadricula_C_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_C_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA solucion[NUM_FILAS][NUM_COLUMNAS]);

void
candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna);

/* declaracion de funciones ARM a implementar */
extern
int
candidatos_actualizar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

extern
int
candidatos_actualizar_arm_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

extern
int
candidatos_propagar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                               uint8_t fila, uint8_t columna);


// Inicializamos la partida
void sudoku_iniciar(void);

// Actualizamos la visualización de la celda
// Devolverá 1 en caso de que haya habido un cambio en los pines de entrada
// Devolverá 0 en caso contrario
int sudoku_actualizar(void);

// Tratamiento de la pulsación del botón 1
// Al ser pulsado, se introducirá un valor en la celda
void sudoku_pulsacion_1(void);

// Tratamiento de la pulsación del botón 2
// Al ser pulsado, se borrará el valor de la celda
void sudoku_pulsacion_2(void);
#endif /* SUDOKU_H_2021 */

