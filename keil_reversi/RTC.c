#include "RTC.h"
#include <LPC210X.H>  

// Debería contar en power down?

void RTC_init(void) {
    CCR |= 2; // Reseteamos
    CCR &= 0xd; // Desactiva reset
    // PREINT = (PCLK / 32768) - 1 = (60 * 10^6 / 32768) - 1 = 1830
    // PREFRAC = PCLK - (PREINT + 1) * 32768 = 60 * 10^6 - 1831 * 32768 = 1792
    PREINT = 1830;
    PREFRAC = 1792; 
    CCR |= 1; // Inicia clock
}

int RTC_leer_minutos(void) {
    return MIN;
}

int RTC_leer_segundos(void) {
    return SEC;
}
