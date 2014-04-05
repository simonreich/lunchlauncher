/**
 * Atmel ATtiny13-20PU Lunch Launcher Button
 *
 *  Copyright 2014 by Simon Reich <spam@simonreich.de>
 *
 * This file is part of LunchLauncherButton.
 * 
 * LunchLauncherButton is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public 
 * License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * LunchLauncherButton is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Set Fuses:
 * avrdude -p t13 -c usbasp -U lfuse:w:0x7a:m -U hfuse:w:0xfb:m
*/

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "softuart.h"
#include "main.h"




int main(void)
{
	DDRB &= ~(1<<TASTER1);          // Taster 1 is Input
	DDRB &= ~(1<<TASTER2);          // Taster 2 is Input, free in air
	DDRB |= (1<<LED1);              // LED 1 is Output

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

	// Main Loop
	for (;;) 
	{
		// reset watchdog
		wdt_reset();

		// is there a softuart char available
		if (softuart_kbhit ())
		{
			// get char
			softuart_buffer ();
		};

		// did we receive 3 chars?
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

			if (buttonPressed == 2)
			{
				buttonPressed = 0;
			};
		};


		// check if the button is pressed...
		if (!IS_HIGH (TASTER1) && buttonPressed == 0)
		{
			buttonPressed = 1;

			softuart_sendStatus (0);
			status = 2;
		};
		if (IS_HIGH (TASTER1) && buttonPressed == 1)
		{
			buttonPressed = 2;
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
