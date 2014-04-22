#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h> 

#include <string.h>


#include <QtGui/QApplication>
#include <QtWebKit/QWebView>



using namespace std;
 
int main(int argc,char** argv)
{
	QApplication app(argc, argv);

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
	int msgInputPos = 0;
	bool msgInputFull = false;
	char msgOutput[3];
	int msgOutputPos = 0;

	// holds timer
	clock_t timerAlive, timerBuffer, timerSend;
	timerAlive = clock ();					// send alive get package after some time
	timerBuffer = clock ();					// reset buffer after certain time
	timerSend = clock ();					// launch only possible every amount of minutes...
	bool timerSendFirsttime = true;

	// global variable
	int buttonAlive = 0;

	// Main Loop
	while (c!='q')
	{
		// 1 char arrived
		if (read (tty_fd, &msgBuffer, 1) > 0)
		{
			//printf ("%c\r\n", msgBuffer);

			msgInput[msgInputPos] = msgBuffer;
			timerBuffer = clock ();

			if (msgInputPos >= 2)
			{
				msgInputPos = 0;
				msgInputFull = true;
			} else {
				msgInputPos++;
			};
		};

		// is buffer full
		if (msgInputFull)
		{
			// clear buffer
			msgInputFull = false;
			msgInputPos = 0;

			//write(STDOUT_FILENO,&buffer,3);
			if (msgInput[0] == 'T')				// Status arrived
			{
				printf (">> Status Arrived");

				if (msgInput[1] == '0')
				{
					printf ("\tLED 0");
				} else if (msgInput[1] == '1') {
					printf ("\tLED 1");
				} else if (msgInput[1] == '2'){
					printf ("tLED 2");
				};
				// check for Button
				if (msgInput[2] == '1')
				{
					// Button pressed
					printf ("\tButton 1 X\r\n");

					// only send one message every two hours
					printf ("Timer: %i\r\n", (int)(clock ()-timerSend)/CLOCKS_PER_SEC);
					if ((int)(clock ()-timerSend)/CLOCKS_PER_SEC > 3600 || timerSendFirsttime == true)
					{
						printf ("-- Connecting to Internet Server\r\n");

						// reset timer
						timerSend = clock ();

						// set first time hit to false
						timerSendFirsttime = false;

						// calling web page
						/*QWebView* m_pWebView;
						m_pWebView = new QWebView;
						m_pWebView->load(QUrl("http://fortknox.physik3.gwdg.de/lunchlauncher/index.php?name=LunchLauncherButton&text=Lunch"));*/
					};

					// sleep for 2 sec
					usleep(2000000);

					// send ACK
					write (tty_fd, "ACK",3);
					printf ("<< Sending ACK\r\n");
				} else {
					printf ("\tButton 0\r\n");

					// send ACK
					write (tty_fd, "ACK",3);
					printf ("<< Sending ACK\r\n");
				};

				buttonAlive = 0;
			} else if (msgInput[0] == 'A' && msgInput[1] == 'C' && msgInput[2] == 'K' ) {
				buttonAlive = 0;
			} else if (msgInput[0] == 'N' && msgInput[1] == 'A' && msgInput[2] == 'K' ) {
				buttonAlive = 0;
			};
		};

		// buffer is not full, put timerBuffer is set
		if (!msgInputFull && (int)(clock ()-timerBuffer)/CLOCKS_PER_SEC > 2)
		{
			// clear buffer
			msgInputFull = false;
			msgInputPos = 0;
			
			// reset timer
			timerBuffer = clock ();
		};

		// send Alive packet after some time
		if ((int)(clock ()-timerAlive)/CLOCKS_PER_SEC > 600)
		{
			// send Get Status
			write (tty_fd, "G  ", 3);
			printf ("<< Sending Get Status\r\n");

			// reset timer
			timerAlive = clock ();

			// increase counter
			buttonAlive++;

			// is button Alive?
			if (buttonAlive >= 4)
			{
				printf ("-- Button is dead\r\n");
			};
		};

		if (read (STDIN_FILENO, &c, 1) > 0)
		{
			msgOutput[msgOutputPos] = c;
			msgOutputPos++;

			if (msgOutputPos == 3)
			{
				printf ("<< Sending %c%c%c\r\n", msgOutput[0], msgOutput[1], msgOutput[2]);
				write (tty_fd, &msgOutput[0], 1);
				write (tty_fd, &msgOutput[1], 1);
				write (tty_fd, &msgOutput[2], 1);

				msgOutput[0] = 0;
				msgOutput[1] = 0;
				msgOutput[2] = 0;

				msgOutputPos = 0;
			}
		}
	}
 
	close(tty_fd);
	tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
 
	return EXIT_SUCCESS;
} 
