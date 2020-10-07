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
	// int size = sizeof(struct Calibration)

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

	if (serial_purge() != SUCCESS || serial_write_message(MESSAGE_RAW) != SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Failed to set device to raw sampling mode\n");
		return ERROR;
	}

	printf("Press the button to confirm a measurement\n\n");

	struct Hand hand;

	for (enum Parameter parameter = 0; parameter < NUM_PARAMETERS; parameter++)
	{
		printf("=============================\n");
		printf("%s\n", PARAMETERS[parameter]);
		printf("-----------------------------\n");

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			// press button to calibrate
			do
			{
				if (sample(&hand) != SUCCESS)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s %s'\n",
										__func__, DIRECTIONS[direction], PARAMETERS[parameter]);
					return ERROR;
				}

				printf("\r%-12s : %-12d", DIRECTIONS[direction], hand.accel[direction]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Settings struct
			S16 * param = (S16 *)(&settings->accel[direction]);
			param[parameter] = hand.accel[direction];
			printf("\r%-12s : %-12d ~\n", DIRECTIONS[direction], param[parameter]);

			// wait for user to release button
			do
			{
				if (sample(&hand) != SUCCESS)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while waiting for button release after calibrating '%s %s'\n",
										__func__, DIRECTIONS[direction], PARAMETERS[parameter]);
					return ERROR;
				}
			}
			while (hand.button == BUTTON_PRESSED);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			// press button to calibrate
			do
			{
				if (sample(&hand) != SUCCESS)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s %s'\n",
										__func__, FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}

				printf("\r%-12s : %-12d", FINGERS[finger], hand.flex[finger]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Settings struct
			S16 * param = (S16 *)(&settings->flex[finger]);
			param[parameter] = hand.flex[finger];
			printf("\r%-12s : %-12d ~\n", FINGERS[finger], param[parameter]);

			// wait for user to release button
			do
			{
				if (sample(&hand) != SUCCESS)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while waiting for button release after calibrating '%s %s'\n",
										__func__, FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}
			}
			while (hand.button == BUTTON_PRESSED);
		}

		printf("=============================\n");
	}

	log_print(LOG_SUCCESS, "%s(): Interactive calibration complete\n", __func__);
	return SUCCESS;
}

int calibration_print(struct Settings * settings)
{
	if (settings == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	printf("============ CALIBRATION ============\n");
	printf("| %-12s |  MIN |  MAX | ZERO |\n", "");
	printf("-------------------------------------\n");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf("| %-12s | %4d | %4d | %4d |\n", DIRECTIONS[direction],
				settings->accel[direction].min, settings->accel[direction].max, settings->accel[direction].zero);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf("| %-12s | %4d | %4d | %4d |\n", FINGERS[finger],
				settings->flex[finger].min, settings->flex[finger].max, settings->flex[finger].zero);
	}

	printf("=====================================\n");
	return LOG_SUCCESS;
}
