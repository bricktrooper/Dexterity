#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "serial.h"

#include "calibration.h"

extern int errno;

int calibration_import(char * file_name, struct Calibration * calibration)
{
	if (file_name == NULL || calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	FILE * file = fopen(file_name, "r");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		int tokens = fscanf(file, "%*s %*s min=%hd max=%hd zero=%hd\n",
							&(calibration->accel[direction].min),
							&(calibration->accel[direction].max),
							&(calibration->accel[direction].zero));

		if (tokens != 3)
		{
			log_print(LOG_ERROR, "%s(): Calibration file was incorrectly parsed\n", __func__, file_name);
			return ERROR;
		}
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		int tokens = fscanf(file, "%*s %*s min=%hd max=%hd zero=%hd\n",
							&(calibration->flex[finger].min),
							&(calibration->flex[finger].max),
							&(calibration->flex[finger].zero));

		if (tokens != 3)
		{
			log_print(LOG_ERROR, "%s(): Calibration file was incorrectly parsed\n", __func__, file_name);
			return ERROR;
		}
	}

	fclose(file);
	log_print(LOG_SUCCESS, "%s(): Imported calibration from file '%s'\n", __func__, file_name);

	return SUCCESS;
}

int calibration_export(char * file_name, struct Calibration * calibration)
{
	if (file_name == NULL || calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		fprintf(file, "%-2s %-8s min=%hd max=%hd zero=%hd\n",
				DIRECTIONS[direction],
				DIRECTION_NAMES[direction],
				calibration->accel[direction].min,
				calibration->accel[direction].max,
				calibration->accel[direction].zero);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		fprintf(file, "%-2s %-8s min=%hd max=%hd zero=%hd\n",
				FINGERS[finger],
				FINGER_NAMES[finger],
				calibration->flex[finger].min,
				calibration->flex[finger].max,
				calibration->flex[finger].zero);
	}

	fclose(file);
	log_print(LOG_SUCCESS, "%s(): Exported calibration to file '%s'\n", __func__, file_name);
	return SUCCESS;
}

int calibration_download(struct Calibration * calibration)
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

	if (serial_purge() != SUCCESS ||
		serial_write_message(MESSAGE_SETTINGS) != SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Failed to request calibration settings from device\n", __func__);
		return ERROR;
	}

	if (serial_read((char *)calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
	{
		log_print(LOG_ERROR, "%s(): Failed to receive calibration settings from device\n", __func__);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s(): Calibration downloaded from device\n", __func__);
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

	if (serial_purge() != SUCCESS ||
		serial_write_message(MESSAGE_CALIBRATE) != SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Failed to send calibration request to device\n", __func__);
		return ERROR;
	}

	enum Message response = serial_read_message();

	if (response != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device refused calibration request\n", __func__);
		return ERROR;
	}

	if (serial_purge() != SUCCESS ||
		serial_write((char *)calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
	{
		log_print(LOG_ERROR, "%s(): Failed to send calibration data to device\n", __func__);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s(): Calibration uploaded to device\n", __func__);
	return SUCCESS;
}

int calibration_interactive(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (serial_purge() != SUCCESS ||
	    serial_write_message(MESSAGE_RAW) != SUCCESS)
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

				if (parameter == ANALOGUE_ZERO)
				{
					hand.accel[direction] = scale(hand.accel[direction], ACCEL_SCALE_RANGE, calibration->accel[direction].min, calibration->accel[direction].max, 0);
				}

				printf("\r%-2s  %-8s : %-12hd", DIRECTIONS[direction], DIRECTION_NAMES[direction], hand.accel[direction]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			S16 * param = (S16 *)(&calibration->accel[direction]);
			param[parameter] = hand.accel[direction];
			printf("\r%-2s  %-8s : %-12hd ~\n", DIRECTIONS[direction], DIRECTION_NAMES[direction], hand.accel[direction]);

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
					log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s %s'\n", __func__, FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}

				if (parameter == ANALOGUE_ZERO)
				{
					hand.flex[finger] = scale(hand.flex[finger], FLEX_SCALE_RANGE, calibration->flex[finger].min, calibration->flex[finger].max, 0);
				}

				printf("\r%-2s  %-8s : %-12hd", FINGERS[finger], FINGER_NAMES[finger], hand.flex[finger]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			S16 * param = (S16 *)(&calibration->flex[finger]);
			param[parameter] = hand.flex[finger];
			printf("\r%-2s  %-8s : %-12hd ~\n", FINGERS[finger], FINGER_NAMES[finger], hand.flex[finger]);

			if (calibration->flex[finger].min == calibration->flex[finger].max)
			{
				log_print(LOG_WARNING, "%s(): MIN and MAX are the same for '%s'\n", __func__, FINGERS[finger]);
			}

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

int calibration_print(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	printf("============ CALIBRATION ============\n");
	printf("| %-2s  %-8s |  MIN |  MAX | ZERO |\n", "", "");
	printf("-------------------------------------\n");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf("| %-2s  %-8s | %4hd | %4hd | %4hd |\n",
				DIRECTIONS[direction],
				DIRECTION_NAMES[direction],
				calibration->accel[direction].min,
				calibration->accel[direction].max,
				calibration->accel[direction].zero);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf("| %-2s  %-8s | %4hd | %4hd | %4hd |\n",
				FINGERS[finger],
				FINGER_NAMES[finger],
				calibration->flex[finger].min,
				calibration->flex[finger].max,
				calibration->flex[finger].zero);
	}

	printf("=====================================\n");
	return SUCCESS;
}
