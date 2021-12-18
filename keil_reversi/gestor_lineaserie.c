#include "eventos.h"
#include "cola.h"
#include "Gestor_IO.h"
#include "gestor_interrupciones.h"
#include <LPC210x.H>                       /* LPC210x definitions */
#include <stdint.h>
#include <string.h>

const int MAX_SIZE = 5000;
static char bufferEnvio[MAX_SIZE];
static int siguiente;
static int total;

static int ultimoRecibido = 0;
static int ultimoEnviado = 0;

// EN gestor IO
// para poder mandar varias cadenas sin esperar
// buffer de x espacio con dos indices i, j
// Nueva cadena, escribo a partir de j
// Voy escribiendo lo de i

/*void gestor_ls_enviar_cadena(char* cadena) {
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
}*/

/*
Buffer con dos indices -> recibido y enviado
Recibido aumenta al recibir cadenas para escribir
Enviado aumenta al escribir un char en uart
2 estados
  Estado inicial -> Buffer Enviado -> Todo lo recibido se ha escrito en UART
  Buffer no enviado -> parte de lo recibido no está escrito en UART aún

2 ops
  Enviar cadena (recibimos cadena a escribir)
    Incrementamos el indice de recibidos -> estado pasa a Buffer no Enviado
    Si escribo un char en el buffer en la misma posicion que el enviado y el estado es buffer no enviado -> overflow
  Continuar mensaje
    Incrementamos el indice de enviados -> si alcanza al de recibidos -> estado Buffer Enviado


*/

const int BUFFER_ENVIADO = 0;
const int BUFFER_NO_ENVIADO = 1;
const int CHAR_ESCRITO = 0;
const int CHAR_NO_ESCRITO = 1;
static int estado = BUFFER_ENVIADO;
static int estadoUltimoChar = CHAR_ESCRITO;

void gestor_ls_enviar_cadena(char* cadena) {
  disable_isr_fiq();
  int longitud = strlen(cadena);
  for (int i = 0; i < strlen(cadena); i++) {
    if (ultimoRecibido == ultimoEnviado && estado == BUFFER_NO_ENVIADO) {
      // Overflow
      while(1);
    }
		if (cadena[i] == 'p') {
			int j = 2;
		}
    bufferEnvio[ultimoRecibido] = cadena[i];
    ultimoRecibido = (ultimoRecibido + 1) % MAX_SIZE;

    if (estado == BUFFER_ENVIADO) {
      estado = BUFFER_NO_ENVIADO;
      if (estadoUltimoChar == CHAR_ESCRITO) {
        U0THR = '\0';
      }
    }
  }
  enable_isr_fiq();
}

void gestor_ls_continuar_mensaje(void) {
  if(estado == BUFFER_NO_ENVIADO) {
		char c = bufferEnvio[ultimoEnviado];
		if (bufferEnvio[ultimoEnviado] == 'p') {
			c = bufferEnvio[ultimoEnviado];
		}
    U0THR = bufferEnvio[ultimoEnviado];
    estadoUltimoChar = CHAR_NO_ESCRITO;
    ultimoEnviado = (ultimoEnviado + 1) % MAX_SIZE;
    if (ultimoEnviado == ultimoRecibido) {
      estado = BUFFER_ENVIADO;
      cola_guardar_eventos(evento_cadena_enviada, 0);
    } 
  }
  else {
    // Si interrumpe con todo el buffer enviado, ultimo char escrito
    estadoUltimoChar = CHAR_ESCRITO;
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
