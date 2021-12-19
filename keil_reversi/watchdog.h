#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

// Inicia el Watchdog
void WD_init(int sec);

// Alimenta al Watchdog
void WD_feed(void);

#endif
