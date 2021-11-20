#ifndef __GESTOR_ENERGIA_H__
#define __GESTOR_ENERGIA_H__

// Apaga el procesador
void gestor_energia_power_down (void);

// Pone al procesador en modo idle
void gestor_energia_idle (void);

// Reiniciamos el PLL
extern void reiniciarPLL(void);
#endif 
