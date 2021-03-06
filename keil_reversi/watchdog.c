#include "watchdog.h"
#include "gestor_interrupciones.h"
#include <LPC210X.H>  

// Alimenta al Watchdog
void WD_feed(void) {
    disable_isr_fiq();
    WDFEED = 0xAA;
    WDFEED = 0x55;
    enable_isr_fiq();
}

// Inicia el Watchdog
void WD_init(int sec) {
    if( WDMOD & 0x04 ) {
        WDMOD &= ~0x04;	
    }
    // TPCLK = 1 / (60 * 10^6)
    // sec = TPCLK * x * 4
    // X = sec / (1/60 *4 * 10^-6)
    // Xs = sec / (4/60) = sec * 60 / 4
    // X = Xs * 10^6
    int x = (sec * 60) >> 2;
    int y = x * 1000000;
    WDTC = y;
    WDMOD = 3;
    WD_feed();
}

