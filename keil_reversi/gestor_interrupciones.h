#ifndef __INTERRUPCIONES_H__
#define __INTERRUPCIONES_H__

void __swi(0xFF) enable_isr (void);
void __swi(0xFE) disable_isr (void);

void __swi(0xFD) enable_isr_fiq (void);
void __swi(0xFC) disable_isr_fiq (void);

#endif
