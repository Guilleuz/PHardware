#ifndef __GPIO_H__
#define __GPIO_H__

/*Inicializamos el GPIO estableciendo todos
 sus pines como entrada */
void GPIO_iniciar(void);
// Leemos num_bits de valor, a partir del pin bit_inicial
int GPIO_leer(int bit_inicial, int num_bits);

// Escribimos num_bits bits de valor, a partir del pin bit_inicial
void GPIO_escribir(int bit_inicial, int num_bits, int valor);

// Marcamos num_bits pines como entrada, a partir del pin bit_inicial
void GPIO_marcar_entrada(int bit_inicial, int num_bits);

// Marcamos num_bits pines como salida, a partir del pin bit_inicial
void GPIO_marcar_salida(int bit_inicial, int num_bits);

#endif 
