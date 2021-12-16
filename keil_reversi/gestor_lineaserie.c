#include "eventos.h"
#include "cola.h"
#include "Gestor_IO.h"
#include <LPC210x.H>                       /* LPC210x definitions */
#include <stdint.h>
#include <string.h>

static char bufferEnvio[5000];
static int siguiente;
static int total;

// EN gestor IO
// para poder mandar varias cadenas sin esperar
// buffer de x espacio con dos indices i, j
// Nueva cadena, escribo a partir de j
// Voy escribiendo lo de i

void gestor_ls_enviar_cadena(char* cadena) {
	strcpy(bufferEnvio, cadena);
	siguiente = 0;
	total = strlen(bufferEnvio);

	if (total > 0) {
		U0THR = bufferEnvio[siguiente];
		siguiente++;
	} else {
		cola_guardar_eventos(evento_cadena_enviada, 0);
	}
}

void gestor_ls_continuar_mensaje(void) {
	if(siguiente < total) {
	U0THR = bufferEnvio[siguiente];
	siguiente++;
	if (siguiente == total) 
		cola_guardar_eventos(evento_cadena_enviada, 0);
	}
}

void uart0_ISR(void) __irq {
    int interruptID = (U0IIR & 0xe) >> 1;
    if (interruptID == 2) {
        // Leemos char
        char c = U0RBR;
        uint32_t auxData = 0 | c; // Los 8 bits más bajos serán el char
        cola_guardar_eventos(evento_nuevo_caracter, auxData);
    } else if (interruptID == 1) {
        // Se ha escrito el char
        //cola_guardar_eventos(evento_continuar_envio, 0);
				gestor_ls_continuar_mensaje();
    }
		VICVectAddr = 0;
}

void gestor_ls_init(void) {
    PINSEL0 |= 0x00000005;                  /* Enable RxD0 and TxD0             */
    U0LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
    U0DLL = 4;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
    U0DLM = 1;
		//U0DLL = 135;
		//U0DLM = 4;
		
    //U0FDR |= 0x21;                          //MulVal = 2, DivAddVal = 1

    U0LCR = 0x03;                          /* DLAB = 0   */
    U0IER |= 3;
    VICVectAddr4 = (unsigned long)uart0_ISR;
    VICVectCntl4 = 0x20 | 6;
    VICIntEnable |= 0x00000040;            // Habilitamos las interrupciones en el VIC
}
