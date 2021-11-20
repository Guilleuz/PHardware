#ifndef __GESTOR_PULSACION_H__
#define __GESTOR_PULSACION_H__

#define PULSADO 0
#define NO_PULSADO 1


// Inicialización de los botones
void button_init(void);

// Actualización del estado del botón 1
void button_actualizar_estado_1(void);

// Actualización del estado del botón 2
void button_actualizar_estado_2(void);

// Devuelve 1 si ha habido una nueva pulsación (boton 1), 
// 0 en caso contrario
int button_nueva_pulsacion_1(void);

// Devuelve 1 si ha habido una nueva pulsación (boton 2), 
// 0 en caso contrario
int button_nueva_pulsacion_2(void);

// Limpia la pulsación del botón 1
void button_clear_nueva_pulsacion_1(void);

// Limpia la pulsación del botón 2
void button_clear_nueva_pulsacion_2(void);


#endif
