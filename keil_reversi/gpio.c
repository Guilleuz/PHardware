#include "stdint.h"
#include "gpio.h"
#include <LPC210X.H>

void GPIO_iniciar(void) {
    // Inicializamos todos los pines como entrada
    IODIR = 0x00000000;
}

int GPIO_leer(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    int valor = IOPIN & mascara;
    return valor >> bit_inicial;
}

void GPIO_escribir(int bit_inicial, int num_bits, int valor) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    int v = valor << bit_inicial;
    int vNegado = ~v & mascara;
    v = mascara & v;
    IOSET = IOSET | v;
    IOCLR = IOCLR | vNegado;
}

void GPIO_marcar_entrada(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    mascara = 0xffffffff ^ mascara;
    IODIR = IODIR & mascara;
}

void GPIO_marcar_salida(int bit_inicial, int num_bits) {
    int mascara = (1 << num_bits) - 1;
    mascara = mascara << bit_inicial;
    IODIR = IODIR | mascara;
}
