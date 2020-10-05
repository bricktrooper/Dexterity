#include <stdio.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "serial.h"

#include "calibration.h"

int calibration_import(char * filename, struct Calibration * calibration)
{
	if (filename == NULL || calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int calibration_export(char * filename, struct Calibration * calibration)
{
	if (filename == NULL || calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int calibration_download(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int calibration_upload(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!serial_is_open())
	{
		log_print(LOG_ERROR, "%s(): The serial port is not open\n", __func__);
		return ERROR;
	}
	// we should probably send the calibration message so that the device reverts to raw data (no scaling)
	// then send it again to apply the data?
	// maybe some more messages for RAW and SCALE mode?
	// int size = sizeof(*calibration)

	// if (serial_purge() != SUCCESS ||
	// 	serial_write_message(MESSAGE_CALIBRATE) != SUCCESS ||
	// 	serial_write((char *)calibration, size) != size)
	// {
	// 	log_print(LOG_ERROR, "%s(): Failed to send calibration\n", __func__);
	// 	return ERROR;
	// }

	log_print(LOG_SUCCESS, "%s(): Sent calibration but we should probably wait for the device to ACK.....\n", __func__);
	return SUCCESS;
}

int calibration_interactive(struct Settings * settings)
{
	if (settings == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	struct Hand hand;

	printf("======= CALIBRATION =======\n");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		do
		{
			if (sample(&hand) != SUCCESS)
			{
				log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s'\n", __func__, DIRECTIONS[direction]);
				return ERROR;
			}

			printf("\r");
			printf("[%s: %d] - ", DIRECTIONS[direction], hand.accel[direction]);
			fflush(stdout);
		}
		while (hand.button == BUTTON_RELEASED);

		// wait for user to release button
		printf("\n");

		do
		{
			if (sample(&hand) != SUCCESS)
			{
				log_print(LOG_ERROR, "%s(): Sample failed while waiting for button release after calibrating '%s'\n", __func__, DIRECTIONS[direction]);
				return ERROR;
			}
		}
		while (hand.button == BUTTON_PRESSED);
	}

	log_print(LOG_SUCCESS, "%s(): Interactive calibration complete\n", __func__);
	return SUCCESS;
}
