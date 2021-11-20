#include "stdint.h"
#include "gpio.h"
#include <LPC210X.H>

/*Inicializamos el GPIO estableciendo todos
 sus pines como entrada */
void GPIO_iniciar(void) {
    IODIR = 0x00000000;
}

// Leemos num_bits de valor, a partir del pin bit_inicial
int GPIO_leer(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    int valor = IOPIN & mascara;
    return valor >> bit_inicial;
}

// Escribimos num_bits bits de valor, a partir del pin bit_inicial
void GPIO_escribir(int bit_inicial, int num_bits, int valor) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    int v = valor << bit_inicial;
    int vNegado = ~v & mascara;
    v = mascara & v;
    IOSET = IOSET | v;
    IOCLR = IOCLR | vNegado;
}

// Marcamos num_bits pines como entrada, a partir del pin bit_inicial
void GPIO_marcar_entrada(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    mascara = 0xffffffff ^ mascara;
    IODIR = IODIR & mascara;
}

// Marcamos num_bits pines como salida, a partir del pin bit_inicial
void GPIO_marcar_salida(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    IODIR = IODIR | mascara;
}
