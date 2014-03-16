// main.h

// LEDs sind active-high geschaltet
#ifndef SUMMER
#define LED_AN(LED)     (PORTB |=  (1<<(LED)))
#define LED_AUS(LED)    (PORTB &= ~(1<<(LED)))
#define LED_TOGGLE(LED) (PORTB ^=  (1<<(LED)))
#define LED1            PB0
#define LED2            PB1
//#define SUMMER          PB2
#define TASTER1         PB2
//#define TASTER2         PB4
//#define TASTER3         PB5
//#define SUART_TX		PB4			// do not use!
//#define SUART_RX		PB3

#define IS_HIGH(PORT)   (PINB & (1<<(PORT)))
#endif




// allgemeiner Krams
volatile uint8_t blinken;
