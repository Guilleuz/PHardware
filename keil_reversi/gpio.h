#ifndef __GPIO_H__
#define __GPIO_H__

void GPIO_iniciar(void);
int GPIO_leer(int bit_inicial, int num_bits);
void GPIO_escribir(int bit_inicial, int num_bits, int valor);
void GPIO_marcar_entrada(int bit_inicial, int num_bits);
void GPIO_marcar_salida(int bit_inicial, int num_bits);

#endif 
