/* 
	AVR Software-Uart Demo-Application 
	Version 0.3, 4/2007
	
	by Martin Thomas, Kaiserslautern, Germany
	<eversmith@heizung-thomas.de>
	http://www.siwawi.arubi.uni-kl.de/avr_projects
*/

/* 
Test environment/settings: 
- avr-gcc 4.1.1/avr-libc 1.4.5 (WinAVR 1/2007)
- Atmel ATtiny85 @ 1MHz internal R/C
- 2400bps

AVR Memory Usage (-Os)
----------------
Device: attiny85

Program:     874 bytes (10.7% Full)
(.text + .data + .bootloader)

Data:         52 bytes (10.2% Full)
(.data + .bss + .noinit)

*/

// #define WITH_STDIO_DEMO

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "softuart.h"
#include "main.h"




int main(void)
{
	DDRB &= ~(1<<TASTER1);          // Port B ist Eingang
	DDRB &= ~(1<<TASTER2);          // Port B ist Eingang
	DDRB |= (1<<LED1);              // Port B ist Ausgang

	// initialisiere softuart
	softuart_init();
	
	// init variables
	buttonPressed = 0;

	// interrupts ein
	sei ();

	// Activate Watchdog
	wdt_enable (7);

	// sende Alive Paket
	softuart_sendStatus (0);
	status = 2;

	for (;;) 
	{
		// reset watchdog
		wdt_reset();

		// ein softuart char liegt an
		if (softuart_kbhit ())
		{
			// char abholen
			softuart_buffer ();
		};

		if (softuart_BufferFull == 1)
		{
			// ACK received
			if (softuart_Buffer[0] == 'A' && softuart_Buffer[1] == 'C' && softuart_Buffer[2] == 'K')
			{
				if (status != 0)
				{
					blinkenLED (0);
					status = 0;
				};
			};

			// Get Status received
			if (softuart_Buffer[0] == 'G')
			{
				// Send Status
				softuart_sendStatus (0);
				status = 1;
			};

			// Set Status received
			if (softuart_Buffer[0] == 'S')
			{
				if (softuart_Buffer[2] == '2')
				{
					blinkenLED (1);
					softuart_sendACK ();
				} else if (softuart_Buffer[2] == '1') {
					blinkenLED (0);
					LED_AN (LED1);
					softuart_sendACK ();
				} else if (softuart_Buffer[2] == '0') {
					blinkenLED (0);
					softuart_sendACK ();
				} else {
					softuart_sendNCK ();
				};

				status = 0;
			};

			softuart_BufferFull = 0;
		};

		// this should be in the timer callback
		// however, sending fails from the callback, so it goes here
		if (timer_prescalerResend >= 240)
		{
			timer_prescalerResend = 0;

			if (status != 0) 				// Get Status
			{
				// Send Status
				softuart_sendStatus (1);
			};
		};


		// check if the button is pressed...
		if (IS_HIGH (TASTER1) && buttonPressed == 0 && status != 2)
		{
			status = 2;
			buttonPressed = 1;

			softuart_sendStatus (0);
		};
		if (!IS_HIGH (TASTER1))
		{
			buttonPressed = 0;
		};
	};
	
	return 0;
}





void blinkenLED (uint8_t mode)
{
	if (mode == 0)
	{
		LED_AUS (LED1);
		blinken = 0;
	} else {
		blinken = 1;
	};
}
