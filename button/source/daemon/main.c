#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h> 

#include <string.h>


#include <string>


using namespace std;
 
int main(int argc,char** argv)
{
	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;
	int tty_fd;
	unsigned char c='D';

	std::string terminalName = "/dev/ttyUSB0";

	tcgetattr(STDOUT_FILENO,&old_stdio);
 
	memset(&stdio,0,sizeof(stdio));
	stdio.c_iflag=0;
	stdio.c_oflag=0;
	stdio.c_cflag=0;
	stdio.c_lflag=0;
	stdio.c_cc[VMIN]=1;
	stdio.c_cc[VTIME]=0;
	tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
 
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;	   // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;
 
	tty_fd=open(terminalName.c_str (), O_RDWR | O_NONBLOCK);      
	cfsetospeed(&tio,B2400);	    // 2400 baud
	cfsetispeed(&tio,B2400);	    // 2400 baud
 
	tcsetattr(tty_fd,TCSANOW,&tio);

	// holds input message
	char msgBuffer;
	char msgInput[3];
	int msgBufferPos = 0;
	bool msgBufferFull = false;

	// holds timer
	clock_t timerStart, timerBuffer;
	timerStart = clock ();
	timerBuffer = clock ();

	// global variable
	int buttonAlive = 0;

	// Main Loop
	while (c!='q')
	{
		// 1 char arrived
		if (read (tty_fd, &msgBuffer, 1) > 0)
		{
			msgInput[msgBufferPos] = msgBuffer;
			timerBuffer = clock ();

			if (msgBufferPos >= 2)
			{
				msgBufferPos = 0;
				msgBufferFull = true;
			} else {
				msgBufferPos++;
			};
		};

		// is buffer full
		if (msgBufferFull)
		{
			// clear buffer
			msgBufferFull = false;
			msgBufferPos = 0;

			//write(STDOUT_FILENO,&buffer,3);
			if (msgInput[0] == 'T')				// Status arrived
			{
				// send ACK
				write (tty_fd, "ACK",3);
				printf ("Sending ACK\n");

				// check for Button
				if (msgInput[2] == '1')
				{
					// TODO: button pressed
				};

				buttonAlive = 0;
			} else if (msgInput[0] == 'A' && msgInput[1] == 'C' && msgInput[2] == 'K' ) {
				buttonAlive = 0;
			} else if (msgInput[0] == 'N' && msgInput[1] == 'A' && msgInput[2] == 'K' ) {
				buttonAlive = 0;
			};
		};

		// buffer is not full, put timerBuffer is set
		if (!msgBufferFull && (int)(clock ()-timerBuffer)/CLOCKS_PER_SEC > 2)
		{
			// clear buffer
			msgBufferFull = false;
			msgBufferPos = 0;
			
			// reset timer
			timerBuffer = clock ();
		};

		// send Alive packet after some time
		if ((int)(clock ()-timerStart)/CLOCKS_PER_SEC > 10)
		{
			// send Get Status
			write (tty_fd, "G  ", 3);
			printf ("Sending Get Status\n");

			// reset timer
			timerStart = clock ();

			// increase counter
			buttonALive++;

			// is button Alive?
			if (buttonAlive >= 4)
			{
				printf ("Button is dead\n");
			};
		};

		if (read(STDIN_FILENO,&c,1)>0)  write(tty_fd,&c,1);		     // if new data is available on the console, send it to the serial port
	}
 
	close(tty_fd);
	tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
 
	return EXIT_SUCCESS;
} 
