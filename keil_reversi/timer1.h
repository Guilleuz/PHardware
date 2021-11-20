#ifndef __TIMER1_H__
#define __TIMER1_H__

// Configuración inicial del timer
void temporizador_iniciar(void);

// Da comienzo al timer
void temporizador_empezar(void);

// Leemos el valor del timer
int temporizador_leer(void);

// Reinicia el contador del timer y lo para
void temporizador_parar(void);

#endif
