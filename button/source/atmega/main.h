/**
 * Atmel ATtiny13-20PU Lunch Launcher Button
 *
 *  Copyright 2014 by Simon Reich <spam@simonreich.de>
 *
 * This file is part of LunchLauncherButton.
 */


// main.h


// LEDs sind active-high geschaltet
#ifndef SUMMER
#define LED_AN(LED)     (PORTB |=  (1<<(LED)))
#define LED_AUS(LED)    (PORTB &= ~(1<<(LED)))
#define LED_TOGGLE(LED) (PORTB ^=  (1<<(LED)))
#define LED1            PB4
//#define LED2            PB1
//#define SUMMER          PD7
#define TASTER1         PB3
#define TASTER2         PB0
/*#define TASTER2         PB1
#define TASTER3         PB2*/





uint8_t status;
uint8_t blinken;
uint8_t buttonPressed;





void blinkenLED (uint8_t blinkenStatus);

#define IS_HIGH(PORT)   (PINB & (1<<(PORT)))
#endif
