#include "eventos.h"
#include "cola.h"
#include <LPC210x.H>                       /* LPC210x definitions */
#include <stdint.h>

void uart0_ISR(void) __irq {
    int interruptID = (U0IIR & 0xe) >> 1;
    if (interruptID == 2) {
        // Leemos char
        char c = U0RBR;
        uint32_t auxData = 0 | c; // Los 8 bits más bajos serán el char
        cola_guardar_eventos(evento_nuevo_caracter, auxData);
    } else if (interruptID == 1) {
        // Se ha escrito el char
        int b = 1;
    }
    char a = U0RBR;
	VICVectAddr = 0;
}

void gestor_init(void) {
    PINSEL0 |= 0x00000005;                  /* Enable RxD0 and TxD0             */
    U0LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
    U0DLL = 97;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
    U0LCR = 0x03;                          /* DLAB = 0   */
    U0IER |= 3;
    VICVectAddr4 = (unsigned long)uart0_ISR;
    VICVectCntl4 = 0x20 | 6;
    VICIntEnable |= 0x00000040;            // Habilitamos las interrupciones en el VIC
}
