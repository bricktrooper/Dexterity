#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "serial.h"
#include "calibration.h"

int init(void);
void end(int signal);

// when we calibrate we should 1. have the devie send an ack to affim that it is ready,
// and 2. check the data we just updloaded by requesting a download and comparing it immediately after
// 3. send an ack after the device applies the data

int main(void)
{
	init();

	struct Hand hand;
	struct Calibration calibration;

	if (serial_open() != SUCCESS)
	{
		log_print(LOG_ERROR, "Initialization failed\n");
		return ERROR;
	}

	serial_purge();   // Discard any old data from RX buffer before making a new request
	serial_write_message(MESSAGE_SCALED);

	// if (calibration_interactive(&calibration) != SUCCESS)
	// {
		// end(ERROR);
	// }

	// calibration_export("calibration.txt", &calibration);

	calibration_import("calibration.txt", &calibration);
	calibration_print(&calibration);
	calibration_upload(&calibration);

	calibration_download(&calibration);
	calibration_print(&calibration);

	while (1)
	{
		if (sample(&hand) != SUCCESS)
		{
			break;
		}

		// printf("\rX: %hd Y: %hd Z: %hd F1: %hd F2: %hd F3: %hd F4: %hd F5: %hd BUTTON: %hd LED: %hd",
		// 					hand.accel[X], hand.accel[Y], hand.accel[Z],
		// 					hand.flex[F1], hand.flex[F2], hand.flex[F3], hand.flex[F4], hand.flex[F5],
		// 					hand.button, hand.led);
		// fflush(stdout);
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
	printf("\n");
	serial_purge();
	serial_close();
	log_print(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(signal);
}
