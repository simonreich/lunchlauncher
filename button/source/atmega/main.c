#define F_CPU 9600000


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "main.h"
#include "softuart.h"
#include "timer.h"





// interface between avr-libc stdio and the modified Fleury uart-lib:
static int my_stdio_putchar( char c, FILE *stream )
{
	if ( c == '\n' ) {
		softuart_putchar( '\r' );
	}
	softuart_putchar( c );

	return 0;
}
FILE suart_stream = FDEV_SETUP_STREAM( my_stdio_putchar, NULL, _FDEV_SETUP_WRITE );





int main (void)
{
	// DDR setzen
	DDRB &= ~(1<<TASTER1);		// Prot B ist Eingang
	DDRB |= (1<<LED1);			// Port B ist Ausgang
	DDRB |= (1<<LED2);

	// Grundzustand initialisieren
	LED_AUS(LED1);
	LED_AUS(LED2);
	LED_AUS(SUART_TX);

	blinken = 1;

 	// Interrupts aktivieren
	sei();

	// setup timer
	//init_timer();
	//timer_wert = 500;

	// setup suart
	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */

	// Main Loop
	char c;
	while (1)
	{
	
		if ( softuart_kbhit() ) {
			c = softuart_getchar();
			softuart_putchar( '[' );
			softuart_putchar( c );
			softuart_putchar( ']' );
		}

		softuart_puts_P( " Hello " );
	}
 
	return(0);
}
