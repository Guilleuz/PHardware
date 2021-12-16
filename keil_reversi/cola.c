#include "cola.h"
#include "timer1.h"
#include "eventos.h"
#include "Gestor_IO.h"
#include "gestor_interrupciones.h"
 
static struct Evento cola[32];
static volatile uint8_t sinLeer[32] = {0}; // 0, el evento ha sido leído, 1 no
static volatile int ultimo = 0;            // indice que indica el último elemento añadido
static volatile int ultimoLeido = 0;       // indice que indica el último elemento leído
extern int get_CPSR(void);

enum estado_interrupciones {
    IRQ_FIQ_HABILITADAS,
    FIQ_HABILITADA,
    DESCONOCIDO
};


enum estado_interrupciones cola_preprotocol() {
    int estado = get_CPSR();
    int fiqEnabled = ((estado & 0x40) >> 6) != 1;
    int isrEnabled = ((estado & 0x80) >> 7) != 1;
        
    // irqs -> i deshabilitada, f habilitada
    // fiqs -> i deshabilitada, f habilitada
    // llamada no interrupcion -> i, f habilitadas

    if (isrEnabled && fiqEnabled) {
        // IRQs y FIQs habilitadas, llamada habitual
        disable_isr_fiq();
        return IRQ_FIQ_HABILITADAS;
    } else if (fiqEnabled) {
        // IRQs deshabilitadas y FIQs habilitadas, llamada desde interrupción
        disable_fiq();
        return FIQ_HABILITADA;
    }

    return DESCONOCIDO;
}

void cola_postprotocol(enum estado_interrupciones estado) {
    if (estado == IRQ_FIQ_HABILITADAS) enable_isr_fiq();
    else if (estado == FIQ_HABILITADA) enable_fiq();
}


// Guarda un nuevo evento en la cola
void cola_guardar_eventos(uint8_t ID, uint32_t auxData) {
	enum estado_interrupciones estado = cola_preprotocol();
	
    if (!sinLeer[ultimo]) {
        struct Evento e;
        e.ID = (enum evento_identificador)ID;
        e.datosAux = auxData;
        // Asignar valor del timer
        e.timestamp = clock_gettime();
        cola[ultimo] = e;
        sinLeer[ultimo] = 1;
        ultimo = (ultimo + 1) % 32;
    }
    else {
        // Activamos el LED de overflow
        // Bucle infinito
        gestor_io_overflow();
        while (1);
    }
    
    cola_postprotocol(estado);
}

// Devuelve 0 si no hay nuevos eventos por leer, 1 en caso contrario
int cola_hay_nuevos(void) {
    disable_isr_fiq();
	enable_isr_fiq();
    return ultimo != ultimoLeido;
}

// Pre: cola_hay_nuevos() == 1
// Post: Devuelve el evento más antiguo sin leer
struct Evento cola_ultimo_evento(void) {
    disable_isr_fiq();
    struct Evento aux = cola[ultimoLeido];
    sinLeer[ultimoLeido] = 0;
    ultimoLeido = (ultimoLeido + 1) % 32;
    enable_isr_fiq();
	return aux;
}
