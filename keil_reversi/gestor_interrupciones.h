#ifndef __INTERRUPCIONES_H__
#define __INTERRUPCIONES_H__

// Activa o desactiva las interrupciones IRQ
void __swi(0xFF) enable_isr (void);
void __swi(0xFE) disable_isr (void);

// Activa o desactiva las interrupciones IRQ y FIQ
void __swi(0xFD) enable_isr_fiq (void);
void __swi(0xFC) disable_isr_fiq (void);

// Activa o desactiva las interrupciones FIQ
void __swi(0xFB) enable_fiq (void);
void __swi(0xFA) disable_fiq (void);

#endif
