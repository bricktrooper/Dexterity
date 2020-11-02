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

	int rc = ERROR;

	int tokens = fscanf(file, "accel-range=%hd\n", &(calibration->accel.range))
			   + fscanf(file, "flex-range=%hd\n", &(calibration->flex.range));

	if (tokens != 2)
	{
		log_print(LOG_ERROR, "%s(): Calibration file was incorrectly parsed\n", __func__, file_name);
		goto EXIT;
	}

	char label [MAX_SENSOR_NAME_LENGTH + 1];

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		memset(label, 0, sizeof(label));

		if (fscanf(file, "%s", label) != 1)
		{
			log_print(LOG_ERROR, "%s(): Calibration file label was incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}

		if (strncmp(label, DIRECTIONS[direction], MAX_SENSOR_NAME_LENGTH) != 0)
		{
			log_print(LOG_ERROR, "%s(): Incorrect calibration file label: Expected '%s' but parsed '%s'\n",
								__func__, DIRECTIONS[direction], label);
			goto EXIT;
		}

		tokens = fscanf(file, " MIN=%hd MAX=%hd CENTRE=%hd\n",
						&(calibration->accel.params[direction][MIN]),
						&(calibration->accel.params[direction][MAX]),
						&(calibration->accel.params[direction][CENTRE]));

		if (tokens != NUM_PARAMETERS)
		{
			log_print(LOG_ERROR, "%s(): Calibration file parameters were incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		memset(label, 0, sizeof(label));

		if (fscanf(file, "%s", label) != 1)
		{
			log_print(LOG_ERROR, "%s(): Calibration file label was incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}

		if (strncmp(label, FINGERS[finger], MAX_SENSOR_NAME_LENGTH) != 0)
		{
			log_print(LOG_ERROR, "%s(): Incorrect calibration file label: Expected '%s' but parsed '%s'\n",
								__func__, FINGERS[finger], label);
			goto EXIT;
		}

		tokens = fscanf(file, " MIN=%hd MAX=%hd CENTRE=%hd\n",
						&(calibration->flex.params[finger][MIN]),
						&(calibration->flex.params[finger][MAX]),
						&(calibration->flex.params[finger][CENTRE]));

		if (tokens != NUM_PARAMETERS)
		{
			log_print(LOG_ERROR, "%s(): Calibration file parameters were incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}
	}

	log_print(LOG_SUCCESS, "%s(): Imported calibration from file '%s'\n", __func__, file_name);
	rc = SUCCESS;

EXIT:
	fclose(file);
	return rc;
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

	int rc = ERROR;

	if (fprintf(file, "accel-range=%hd\n", calibration->accel.range) < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		goto EXIT;
	}

	if (fprintf(file, "flex-range=%hd\n", calibration->flex.range) < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		goto EXIT;
	}

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		int written = fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
									MAX_SENSOR_NAME_LENGTH, DIRECTIONS[direction],
									calibration->accel.params[direction][MIN],
									calibration->accel.params[direction][MAX],
									calibration->accel.params[direction][CENTRE]);

		if (written < 0)
		{
			log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
			goto EXIT;
		}
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		int written = fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
									MAX_SENSOR_NAME_LENGTH, FINGERS[finger],
									calibration->flex.params[finger][MIN],
									calibration->flex.params[finger][MAX],
									calibration->flex.params[finger][CENTRE]);

		if (written < 0)
		{
			log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
			goto EXIT;
		}
	}

	log_print(LOG_SUCCESS, "%s(): Export calibration to file '%s'\n", __func__, file_name);
	rc = SUCCESS;

EXIT:
	fclose(file);
	return rc;
}

int calibration_download(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (download(calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to download calibration settings from device\n", __func__);
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

	if (upload(calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to upload calibration settings to device\n", __func__);
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

	if (raw() == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to set device to raw sampling mode\n");
		return ERROR;
	}

	// Use the default analogue ranges
	// This can be changed from the calibration file
	calibration->accel.range = ACCEL_DEFAULT_RANGE;
	calibration->flex.range = FLEX_DEFAULT_RANGE;

	printf("Press the button to confirm a measurement\n\n");

	struct Hand hand;

	for (enum Parameter parameter = 0; parameter < NUM_PARAMETERS; parameter++)
	{
		printf("=======================\n");
		printf("%s\n", PARAMETERS[parameter]);
		printf("-----------------------\n");

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			// press button to calibrate
			do
			{
				if (sample(&hand) == ERROR)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s %s'\n",
										__func__, DIRECTIONS[direction], PARAMETERS[parameter]);
					return ERROR;
				}

				if (parameter == CENTRE)
				{
					hand.accel[direction] = scale(hand.accel[direction],
												  calibration->accel.range,
												  calibration->accel.params[direction][MIN],
												  calibration->accel.params[direction][MAX],
												  0);
				}

				printf("\r%-6s : %-12hd", DIRECTIONS[direction], hand.accel[direction]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			calibration->accel.params[direction][parameter] = hand.accel[direction];
			printf("\r%-6s : %-12hd ~\n", DIRECTIONS[direction], calibration->accel.params[direction][parameter]);

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
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
				if (sample(&hand) == ERROR)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while calibrating '%s %s'\n",
										__func__, FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}

				if (parameter == CENTRE)
				{
					hand.flex[finger] = scale(hand.flex[finger],
											  calibration->flex.range,
											  calibration->flex.params[finger][MIN],
											  calibration->flex.params[finger][MAX],
											  0);
				}

				printf("\r%-6s : %-12hd", FINGERS[finger], hand.flex[finger]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			calibration->flex.params[finger][parameter] = hand.flex[finger];
			printf("\r%-6s : %-12hd ~\n", FINGERS[finger], calibration->flex.params[finger][parameter]);

			if (calibration->flex.params[finger][MIN] == calibration->flex.params[finger][MAX])
			{
				log_print(LOG_WARNING, "%s(): MIN and MAX are the same for '%s'\n", __func__, FINGERS[finger]);
			}

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
				{
					log_print(LOG_ERROR, "%s(): Sample failed while waiting for button release after calibrating '%s %s'\n",
										__func__, FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}
			}
			while (hand.button == BUTTON_PRESSED);
		}

		printf("=======================\n");
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
	printf("| Accelerometer range: %12hd |\n", calibration->accel.range);
	printf("| Flex sensor range:   %12hd |\n", calibration->flex.range);
	printf("-------------------------------------\n");
	printf("| %-*s | %*s | %*s | %*s |\n",
			MAX_SENSOR_NAME_LENGTH, "",
			MAX_SENSOR_NAME_LENGTH, PARAMETERS[MIN],
			MAX_SENSOR_NAME_LENGTH, PARAMETERS[MAX],
			MAX_SENSOR_NAME_LENGTH, PARAMETERS[CENTRE]);
	printf("-------------------------------------\n");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf("| %-*s | %*hd | %*hd | %*hd |\n",
				MAX_SENSOR_NAME_LENGTH, DIRECTIONS[direction],
				MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][MIN],
				MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][MAX],
				MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][CENTRE]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf("| %-*s | %*hd | %*hd | %*hd |\n",
				MAX_SENSOR_NAME_LENGTH, FINGERS[finger],
				MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][MIN],
				MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][MAX],
				MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][CENTRE]);
	}

	printf("=====================================\n");
	return SUCCESS;
}
