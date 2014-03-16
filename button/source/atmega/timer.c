// timer.c

#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "timer.h"
#include "softuart.h"





// initialize timer
void init_timer(void)
{
	// CTC Modus
	TCCR0A = (1<<WGM01);

	// Timerregister wird mit CPU-Takt/x inkrementiert
	//TCCR0B |= (1<<CS02) | (1<<CS00);	// Prescaler 1024
	TCCR0B |= (1<<CS01);			// Prescaler 8

	// Obergrenze für Register
	// 1/(CPU-Takt/Prescaler)*OCR0A = Zeit in ms je Aufruf
	// 1/(9.6e6/1024)*10 = 0.0010667
	OCR0A = 249;

	// Compare Interrupt erlauben
	TIMSK0 |= (1<<OCIE0A);

	// Variablen setzen
	timer_millisekunden = 0;
	timer_sekunde = 0;
	timer_minute = 0;
	timer_stunde = 0;

	return;
}





// Der Compare Interrupt Handler 
/*ISR( TIM0_COMPA_vect )
{

	timer_millisekunden++;

	if( timer_millisekunden >= timer_wert )
	{
		timer_millisekunden = 0;

		if( blinken == 1 )
		{
			//LED_TOGGLE(LED2);
		};
	};




	if( timer_millisekunden == 1000 )
	{
		timer_sekunde++;
		timer_millisekunden = 0;

		if( timer_sekunde == 60 )
		{
			timer_minute++;
			timer_sekunde = 0;
		};
		if( timer_minute == 60 )
		{
			timer_stunde++;
			timer_minute = 0;
		};
		if( timer_stunde == 24 )
		{
			timer_stunde = 0;
		};
	};

}*/
