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
#include <avr/pgmspace.h>
#include "softuart.h"
#include "main.h"




int main(void)
{
	DDRB &= ~(1<<TASTER1);          // Port B ist Eingang
	DDRB |= (1<<LED1);              // Port B ist Ausgang
	DDRB |= (1<<LED2);

	LED_AUS(LED1);
	LED_AUS(LED2);

	unsigned short cnt = 0;

	// initialisiere softuart
	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */
	
	sei();
	
	softuart_puts( "--\r\n" );  // string "from RAM"

	// set status
	status = 0;			// keep alive sent
	softuart_sendAlive ();
	LED_AN (LED1);


	
	for (;;) 
	{
		// ein softuart char liegt an
		if (softuart_kbhit ())
		{
			// char abholen
			softuart_buffer ();
		};

		if (softuart_BufferFull == 1)
		{
			softuart_putchar( '[' );
			softuart_puts( softuart_Buffer );
			softuart_putchar( ']' );

			// ACK received
			if (softuart_Buffer[0] == 'A' && softuart_Buffer[1] == 'C' && softuart_Buffer[2] == 'K')
			{
				if (status == 0)			// ALive
				{
					status = -1;
				} else if (status == 1) {		// Get Status
					status = -1;
				};
			};

			// Get Status received
			if (softuart_Buffer[0] == 'G' && softuart_Buffer[1] == ' ' && softuart_Buffer[2] == ' ')
			{
				// Send Status
				softuart_sendStatus ();
				status = 1;
			};

			// Set Status received
			if (softuart_Buffer[0] == 'S')
			{
				if (softuart_Buffer[2] == '2')
				{
					blinken = 1;
					softuart_sendACK ();
				} else if (softuart_Buffer[2] == '1') {
					LED_AN (LED1);
					softuart_sendACK ();
				} else if (softuart_Buffer[2] == '0') {
					LED_AUS (LED1);
					softuart_sendACK ();
				} else {
					softuart_sendNAK ();
				};


				status = -1;
			};
			softuart_BufferFull = 0;
		};
	
		/*if ( softuart_kbhit() ) {
			c = softuart_getchar();
			softuart_putchar( '[' );
			softuart_putchar( c );
			softuart_putchar( ']' );
		}*/

		cnt++;
		if (cnt >= 400000) {
			cnt = 0;
			softuart_puts_P( " Hello " );
		};
		
	}
	
	return 0;
}
