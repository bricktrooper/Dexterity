#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dexterity.h"
#include "log.h"
#include "serial.h"

int init(void);
void end(int signal);

int main(void)
{
	init();

	struct Hand hand;

	while (1)
	{
		serial_purge();   // Discard any old data from RX buffer before making a new request
		serial_write(MESSAGE_SAMPLE"\r", strlen(MESSAGE_SAMPLE"\r"));
		serial_read((char *)&hand, sizeof(hand));


		// WE ALSO NEED TO MAKE A common.h. or maybe let's call it dexteroty.h.  Yeah that sounds fitting.
		printf("X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d\n",
						hand.accel[0],
						hand.accel[1],
						hand.accel[2],
						hand.flex[0],
						hand.flex[1],
						hand.flex[2],
						hand.flex[3],
						hand.flex[4]
						);
	}

	serial_close();
	return SUCCESS;
}

int init(void)
{
	signal(SIGINT, end);   // register signal handler for CTRL+C

	log_suppress(LOG_ERROR, false);
	log_suppress(LOG_WARNING, false);
	log_suppress(LOG_SUCCESS, false);
	log_suppress(LOG_DEBUG, false);
	log_suppress(LOG_INFO, true);

	if (serial_open() != SUCCESS)
	{
		log_print(LOG_ERROR, "Initialization failed\n");
		exit(ERROR);
	}

	return SUCCESS;
}

void end(int signal)
{
	serial_purge();
	serial_close();
	log_print(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(0);
}
