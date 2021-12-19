#include "eventos.h"
#include "cola.h"
#include "Gestor_IO.h"
#include "gestor_interrupciones.h"
#include <LPC210x.H>                       /* LPC210x definitions */
#include <stdint.h>
#include <string.h>

const int MAX_SIZE = 5000;
static char bufferEnvio[MAX_SIZE];
static int ultimoRecibido = 0;
static int ultimoEnviado = 0;

/*
Buffer con dos indices -> recibido y enviado
Recibido aumenta al recibir cadenas para escribir
Enviado aumenta al escribir un char en uart
2 estados
  Estado inicial -> Buffer Enviado -> Todo lo recibido se ha escrito en UART
  Buffer no enviado -> parte de lo recibido no está escrito en UART aún

2 operaciones
  - Enviar cadena (recibimos cadena a escribir)
    + Incrementamos el indice de recibidos -> estado pasa a Buffer no Enviado
    + Si escribo un char en el buffer en la misma posicion que el enviado y el estado es buffer no enviado -> overflow

  - Continuar mensaje
    + Incrementamos el indice de enviados -> si alcanza al de recibidos el estado pasa a Buffer Enviado
*/

const int BUFFER_ENVIADO = 0;
const int BUFFER_NO_ENVIADO = 1;
const int CHAR_ESCRITO = 0;
const int CHAR_NO_ESCRITO = 1;

// Estado Buffer, enviado o no
static int estado = BUFFER_ENVIADO;
// Estado del último caracter, está escrito ya o no
static int estadoUltimoChar = CHAR_ESCRITO;

void gestor_ls_enviar_cadena(char* cadena) {
    disable_isr_fiq();
    for (int i = 0; i < strlen(cadena); i++) {
        if (ultimoRecibido == ultimoEnviado && estado == BUFFER_NO_ENVIADO) {
            // Overflow
            while(1);
        }
        bufferEnvio[ultimoRecibido] = cadena[i];
        ultimoRecibido = (ultimoRecibido + 1) % MAX_SIZE;

        if (estado == BUFFER_ENVIADO) {
            estado = BUFFER_NO_ENVIADO;
            if (estadoUltimoChar == CHAR_ESCRITO) {
                // Escribimos un caracter nulo en UART
                // para que se produzca una interrupción
                // y se envíe la cadena con continuar mensaje
                U0THR = '\0';
            }
        }
    }
    enable_isr_fiq();
}

void gestor_ls_continuar_mensaje(void) {
    if(estado == BUFFER_NO_ENVIADO) {
        U0THR = bufferEnvio[ultimoEnviado];
        estadoUltimoChar = CHAR_NO_ESCRITO;
        ultimoEnviado = (ultimoEnviado + 1) % MAX_SIZE;
        if (ultimoEnviado == ultimoRecibido) {
            estado = BUFFER_ENVIADO;
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
        gestor_ls_continuar_mensaje();
    }
    VICVectAddr = 0;
}

void gestor_ls_init(void) {
    PINSEL0 |= 0x00000005;                  /* Enable RxD0 and TxD0             */
    U0LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
    U0DLL = 4;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
    U0DLM = 1;
    U0LCR = 0x03;                          /* DLAB = 0   */
    U0IER |= 3;
    VICVectAddr4 = (unsigned long)uart0_ISR;
    VICVectCntl4 = 0x20 | 6;
    VICIntEnable |= 0x00000040;            // Habilitamos las interrupciones en el VIC
}
