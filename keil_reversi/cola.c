#include "cola.h"
#include "timer1.h"
#include "eventos.h"


 
static struct Evento cola[32];
// 0, el evento ha sido leído, 1 no
static uint8_t sinLeer[32] = {0};
static int ultimo = 0;
static int ultimoLeido = 0;

void cola_guardar_eventos(uint8_t ID, uint32_t auxData) {
    if (!sinLeer[ultimo]) {
        struct Evento e;
        e.ID = ID;
        e.datosAux = auxData;
        // Asignar valor del timer
        e.timestamp = temporizador_leer();
        cola[ultimo] = e;
        sinLeer[ultimo] = 1;
        ultimo = (ultimo + 1) % 32;
    }
    else {
        // Activamos el LED de overflow
        // Bucle infinito
        while (1);
    }
}

int cola_hay_nuevos(void) {
    return ultimo != ultimoLeido;
}

struct Evento cola_ultimo_evento(void) {
    struct Evento aux = cola[ultimoLeido];
    sinLeer[ultimoLeido] = 0;
    ultimoLeido = (ultimoLeido + 1) % 32;
    return aux;
}
