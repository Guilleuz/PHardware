#ifndef __TIMER1_H__
#define __TIMER1_H__
#include <stdint.h>

// Configuración inicial del timer
void temporizador_iniciar(void);

// Da comienzo al timer
void temporizador_empezar(void);

// Leemos el valor del timer
uint32_t __swi(0) clock_gettime(void);
uint32_t __SWI_0 (void);

// Devuelve el tiempo transcurrido en microsegundos
uint32_t temporizador_leer(void);

// Reinicia el contador del timer y lo para
void temporizador_parar(void);



#endif
