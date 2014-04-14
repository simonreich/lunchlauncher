/*
 * daemonize.c
 * This example daemonizes a process, writes a few log messages,
 * sleeps 20 seconds and terminates afterwards.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>


#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h> 
#include <string.h>









static void skeleton_daemon()
{
	pid_t pid;

	/* Fork off the parent process */
	pid = fork();

	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);

	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* On success: The child process becomes session leader */
	if (setsid() < 0)
		exit(EXIT_FAILURE);

	/* Catch, ignore and handle signals */
	//TODO: Implement a working signal handler */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();

	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);

	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");

	/* Close all open file descriptors */
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
	{
		close (x);
	}

	/* Open the log file */
	openlog ("lunchlauncherdaemon", LOG_PID, LOG_DAEMON);
}



int main()
{
	skeleton_daemon();

	//QApplication app(argc, argv);

	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;
	int tty_fd;
	unsigned char c='D';

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
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	   // make the reads non-blocking
 
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;	   // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;
 
	tty_fd=open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);	  
	cfsetospeed(&tio,B2400);		// 2400 baud
	cfsetispeed(&tio,B2400);		// 2400 baud
 
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

	// write to syslog
	syslog (LOG_NOTICE, "LunchLauncher daemon started.");

	// Main Loop
	while (1)
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
				//printf (">> Status Arrived");

				if (msgInput[1] == '0')
				{
					//printf ("\tLED 0");
				} else if (msgInput[1] == '1') {
					//printf ("\tLED 1");
				} else if (msgInput[1] == '2'){
					//printf ("tLED 2");
				};
				// check for Button
				if (msgInput[2] == '1')
				{
					// Button pressed
					//printf ("\tButton 1 X\r\n");

					// only send one message every two hours
					//printf ("Timer: %i\r\n", (int)(clock ()-timerSend)/CLOCKS_PER_SEC);
					if ((int)(clock ()-timerSend)/CLOCKS_PER_SEC > 3600 || timerSendFirsttime == true)
					{
						//printf ("-- Connecting to Internet Server\r\n");

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
					//printf ("<< Sending ACK\r\n");
				} else {
					//printf ("\tButton 0\r\n");

					// send ACK
					write (tty_fd, "ACK",3);
					//printf ("<< Sending ACK\r\n");
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
			//printf ("<< Sending Get Status\r\n");

			// reset timer
			timerAlive = clock ();

			// increase counter
			buttonAlive++;

			// is button Alive?
			if (buttonAlive >= 4)
			{
				//printf ("-- Button is dead\r\n");
			};
		};

		if (read (STDIN_FILENO, &c, 1) > 0)
		{
			msgOutput[msgOutputPos] = c;
			msgOutputPos++;

			if (msgOutputPos == 3)
			{
				//printf ("<< Sending %c%c%c\r\n", msgOutput[0], msgOutput[1], msgOutput[2]);
				write (tty_fd, &msgOutput[0], 1);
				write (tty_fd, &msgOutput[1], 1);
				write (tty_fd, &msgOutput[2], 1);

				msgOutput[0] = 0;
				msgOutput[1] = 0;
				msgOutput[2] = 0;

				msgOutputPos = 0;
			};
		};
	};

	close(tty_fd);
	tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);

	syslog (LOG_NOTICE, "LunchLauncher terminated.");
	closelog();

	return EXIT_SUCCESS;
}
