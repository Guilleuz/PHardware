#ifndef __RTC_H__
#define __RTC_H__

// Inicialización del RTC
void RTC_init(void);

// Devuelve los minutos transcurridos desde el inicio
int RTC_leer_minutos(void);

// Devuelve los segundos transcurridos del último minuto
int RTC_leer_segundos(void);

#endif
