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
#include "colours.h"

#include "calibration.h"

extern int errno;

int calibration_import(char * file_name, struct Calibration * calibration)
{
	if (file_name == NULL || calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	FILE * file = fopen(file_name, "r");

	if (file == NULL)
	{
		log(LOG_ERROR, "Failed to open file '%s': %s (%d)\n", file_name, strerror(errno), errno);
		return ERROR;
	}

	int rc = ERROR;

	if (fscanf(file, "accel-range=%hd\n", &calibration->accel.range) != 1)
	{
		log(LOG_ERROR, "Accel range was incorrectly parsed\n");
		goto EXIT;
	}

	if (fscanf(file, "flex-range=%hd\n", &calibration->flex.range) != 1)
	{
		log(LOG_ERROR, "Flex range was incorrectly parsed\n");
		goto EXIT;
	}

	if (calibration->accel.range < 0 || calibration->flex.range < 0)
	{
		log(LOG_ERROR, "Analogue range cannot be negative\n");
		goto EXIT;
	}

	char sensor_label [MAX_SENSOR_NAME_LENGTH + 1];

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		if (fscanf(file, "%s", sensor_label) != 1)
		{
			log(LOG_ERROR, "sensor label was incorrectly parsed\n");
			goto EXIT;
		}

		if (strncmp(sensor_label, DIRECTIONS[direction], MAX_SENSOR_NAME_LENGTH) != 0)
		{
			log(LOG_ERROR, "Incorrect sensor label: Expected '%s' but parsed '%s'\n",
			                     DIRECTIONS[direction], sensor_label);
			goto EXIT;
		}

		int tokens = fscanf(file, " MIN=%hd MAX=%hd CENTRE=%hd\n",
		                    &calibration->accel.params[direction][MIN],
		                    &calibration->accel.params[direction][MAX],
		                    &calibration->accel.params[direction][CENTRE]);

		if (tokens != NUM_PARAMETERS)
		{
			log(LOG_ERROR, "Sensor parameters were incorrectly parsed\n");
			goto EXIT;
		}
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		if (fscanf(file, "%s", sensor_label) != 1)
		{
			log(LOG_ERROR, "sensor label was incorrectly parsed\n");
			goto EXIT;
		}

		if (strncmp(sensor_label, FINGERS[finger], MAX_SENSOR_NAME_LENGTH) != 0)
		{
			log(LOG_ERROR, "Incorrect sensor label: Expected '%s' but parsed '%s'\n",
			                     FINGERS[finger], sensor_label);
			goto EXIT;
		}

		int tokens = fscanf(file, " MIN=%hd MAX=%hd CENTRE=%hd\n",
		                    &calibration->flex.params[finger][MIN],
		                    &calibration->flex.params[finger][MAX],
		                    &calibration->flex.params[finger][CENTRE]);

		if (tokens != NUM_PARAMETERS)
		{
			log(LOG_ERROR, "Sensor parameters were incorrectly parsed\n");
			goto EXIT;
		}
	}

	log(LOG_SUCCESS, "Imported calibration from file '%s'\n", file_name);
	rc = SUCCESS;

EXIT:
	fclose(file);
	return rc;
}

int calibration_export(char * file_name, struct Calibration * calibration)
{
	if (file_name == NULL || calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log(LOG_ERROR, "Failed to open file '%s': %s (%d)\n", file_name, strerror(errno), errno);
		return ERROR;
	}

	fprintf(file, "accel-range=%hd\n", calibration->accel.range);
	fprintf(file, "flex-range=%hd\n", calibration->flex.range);

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
		        MAX_SENSOR_NAME_LENGTH, DIRECTIONS[direction],
		        calibration->accel.params[direction][MIN],
		        calibration->accel.params[direction][MAX],
		        calibration->accel.params[direction][CENTRE]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
		        MAX_SENSOR_NAME_LENGTH, FINGERS[finger],
		        calibration->flex.params[finger][MIN],
		        calibration->flex.params[finger][MAX],
		        calibration->flex.params[finger][CENTRE]);
	}

	log(LOG_SUCCESS, "Export calibration to file '%s'\n", file_name);
	fclose(file);
	return SUCCESS;
}

int calibration_download(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (download(calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to download calibration settings from device\n");
		return ERROR;
	}

	log(LOG_SUCCESS, "Calibration downloaded from device\n");
	return SUCCESS;
}

int calibration_upload(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (upload(calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to upload calibration settings to device\n");
		return ERROR;
	}

	log(LOG_SUCCESS, "Calibration uploaded to device\n");
	return SUCCESS;
}

int calibration_interactive(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (raw() == ERROR)
	{
		log(LOG_ERROR, "Failed to set device to raw sampling mode\n");
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
					log(LOG_ERROR, "Sample failed while calibrating '%s %s'\n",
					                     DIRECTIONS[direction], PARAMETERS[parameter]);
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
			printf("\r%-6s : %-12hd " GREEN "~" WHITE "\n", DIRECTIONS[direction], calibration->accel.params[direction][parameter]);

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while waiting for button release after calibrating '%s %s'\n",
					                     DIRECTIONS[direction], PARAMETERS[parameter]);
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
					log(LOG_ERROR, "Sample failed while calibrating '%s %s'\n",
					                     FINGERS[finger], PARAMETERS[parameter]);
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
				log(LOG_WARNING, "MIN and MAX are the same for '%s'\n", FINGERS[finger]);
			}

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while waiting for button release after calibrating '%s %s'\n",
					                     FINGERS[finger], PARAMETERS[parameter]);
					return ERROR;
				}
			}
			while (hand.button == BUTTON_PRESSED);
		}

		printf("=======================\n");
	}

	log(LOG_SUCCESS, "Interactive calibration complete\n");
	return SUCCESS;
}

int calibration_print(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
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
