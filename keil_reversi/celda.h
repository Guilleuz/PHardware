/* guarda para evitar inclusiones múltiples (include guard) */
#ifndef CELDA_H
#define CELDA_H

#include <inttypes.h>

/* Cada celda se codifica en 16 bits
 * bits [15,7]: los 9 bits más significativos representan el vector de candidatos,
 * si el bit 7 + valor - 1 está a 0, valor es candidato, 1 en caso contrario
 * bit 6: no empleado
 * bit 5: error
 * bit 4: pista
 * bits [3,0]: valor de la celda
 */

enum { BIT_CANDIDATOS = 7 };

typedef uint16_t CELDA;

__inline static uint8_t celda_es_pista(CELDA celda) {
    return celda & 16;
}

__inline static void celda_marcar_error(CELDA *celda) {
    *celda = *celda | 0x0020;
}
__inline static void celda_desmarcar_error(CELDA *celda) {
    *celda = *celda & 0xffdf;
}

/*Reinicia los candidatos de la celda
inicialmente serán todos candidatos*/
__inline static void celda_inicializar_candidatos(CELDA *celdaptr) {
    *celdaptr = *celdaptr & 0x007f;
}

/* *****************************************************************************
 * elimina el candidato del valor almacenado en la celda indicada */
__inline static void celda_eliminar_candidato(CELDA *celdaptr, uint8_t valor)
{
    *celdaptr = *celdaptr | (0x0001 << (BIT_CANDIDATOS + valor - 1));
}

/* *****************************************************************************
 * modifica el valor almacenado en la celda indicada */
__inline static void
celda_poner_valor(CELDA *celdaptr, uint8_t val)
{
    *celdaptr = (*celdaptr & 0xFFF0) | (val & 0x000F);
}

/* *****************************************************************************
 * extrae el valor almacenado en los 16 bits de una celda */
__inline static uint8_t
celda_leer_valor(CELDA celda)
{
    return (celda & 0x000F);
}

// Leemos los candidatos de una celda
__inline static int
celda_leer_candidatos(CELDA celda) {
    return (celda & 0xff80) >> 7;
}
#endif // CELDA_H
