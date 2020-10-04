#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dexterity.h"
#include "log.h"
#include "serial.h"

int init(void);
void end(int signal);

char * MESSAGES [] = {
	"SAMPLE",
	"CALIBRATE",
	"SETTINGS",
	"ERROR",
	"SUCCESS",
	"UNKNOWN"
};

int NUM_MESSAGES = sizeof(MESSAGES);

int main(void)
{
	init();

	if (serial_open() != SUCCESS)
	{
		log_print(LOG_ERROR, "Initialization failed\n");
		return ERROR;
	}

	struct Hand hand;

	while (1)
	{
		serial_purge();   // Discard any old data from RX buffer before making a new request
		serial_write_message(MESSAGE_SAMPLE);
		serial_read((char *)&hand, sizeof(hand));

		log_print(LOG_DEBUG, "X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d\n",
							hand.accel[X], hand.accel[Y], hand.accel[Z],
							hand.flex[F1], hand.flex[F2], hand.flex[F3], hand.flex[F4], hand.flex[F5]);
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

	return SUCCESS;
}

void end(int signal)
{
	serial_purge();
	serial_close();
	log_print(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(0);
}
