#ifndef __GESTOR_PULSACION_H__
#define __GESTOR_PULSACION_H__

#define PULSADO 0
#define NO_PULSADO 1

void button_init(void);
int button_nueva_pulsacion_1(void);
int button_nueva_pulsacion_2(void);
void button_clear_nueva_pulsacion_1(void);
void button_clear_nueva_pulsacion_2(void);
void button_actualizar_estado_1(void);
void button_actualizar_estado_2(void);
#endif
