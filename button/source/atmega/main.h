// main.h

// LEDs sind active-high geschaltet
#ifndef SUMMER
#define LED_AN(LED)     (PORTB |=  (1<<(LED)))
#define LED_AUS(LED)    (PORTB &= ~(1<<(LED)))
#define LED_TOGGLE(LED) (PORTB ^=  (1<<(LED)))
#define LED1            PB0
#define LED2            PB1
//#define SUMMER          PD7
#define TASTER1         PB2
/*#define TASTER2         PD3
#define TASTER3         PD4*/





uint8_t status;
uint8_t blinken;





#define IS_HIGH(PORT)   (PINB & (1<<(PORT)))
#endif
