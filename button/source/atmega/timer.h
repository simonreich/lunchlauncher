// timer.h

extern void init_timer(void);		// initialize timer
extern ISR( TIM0_COMPA_vect );		// Der Compare Interrupt Handler wird aufgerufen, wenn TCNT0 = OCR0A = 250-1 ist (250 Schritte), d.h. genau alle 1 ms





// Variablen für die Zeit
volatile uint16_t timer_wert;
volatile uint16_t timer_millisekunden;
volatile uint16_t timer_sekunde;
volatile uint16_t timer_minute;
volatile uint16_t timer_stunde;
