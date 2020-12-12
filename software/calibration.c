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

int calibration_import(char * file_name, Calibration * calibration)
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

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		if (fscanf(file, "%s", sensor_label) != 1)
		{
			log(LOG_ERROR, "Sensor label was incorrectly parsed\n");
			goto EXIT;
		}

		char const * string = direction_string(direction);

		if (strncmp(sensor_label, string, strlen(string)) != 0)
		{
			log(LOG_ERROR, "Incorrect sensor label: Expected '%s' but parsed '%s'\n",
			                     string, sensor_label);
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

	for (Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		if (fscanf(file, "%s", sensor_label) != 1)
		{
			log(LOG_ERROR, "Sensor label was incorrectly parsed\n");
			goto EXIT;
		}

		char const * string = finger_string(finger);

		if (strncmp(sensor_label, string, strlen(string)) != 0)
		{
			log(LOG_ERROR, "Incorrect sensor label: Expected '%s' but parsed '%s'\n",
			                     string, sensor_label);
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

int calibration_export(char * file_name, Calibration * calibration)
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

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
		        MAX_SENSOR_NAME_LENGTH, direction_string(direction),
		        calibration->accel.params[direction][MIN],
		        calibration->accel.params[direction][MAX],
		        calibration->accel.params[direction][CENTRE]);
	}

	for (Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		fprintf(file, "%-*s MIN=%hd MAX=%hd CENTRE=%hd\n",
		        MAX_SENSOR_NAME_LENGTH, finger_string(finger),
		        calibration->flex.params[finger][MIN],
		        calibration->flex.params[finger][MAX],
		        calibration->flex.params[finger][CENTRE]);
	}

	log(LOG_SUCCESS, "Exported calibration to file '%s'\n", file_name);
	fclose(file);
	return SUCCESS;
}

int calibration_download(Calibration * calibration)
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

int calibration_upload(Calibration * calibration)
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

	// re-download the newly updated calibration settings from the device verification
	Calibration settings;

	if (download(&settings) == ERROR)
	{
		log(LOG_ERROR, "Failed to download calibration settings from device for verification\n");
		return ERROR;
	}

	// compare the expected and actual settings
	if (memcmp(calibration, &settings, sizeof(Calibration)) != 0)
	{
		// NOTE: This could occur due to noise in the UART.
		// We should inform the user so that they can try uploading the calibration again.
		log(LOG_ERROR, "The device settings do not match what was uploaded\n");
		return ERROR;
	}

	log(LOG_SUCCESS, "Calibration uploaded to device and verified\n");
	return SUCCESS;
}

int calibration_interactive(Calibration * calibration)
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

	Hand hand;

	for (Parameter parameter = 0; parameter < NUM_PARAMETERS; parameter++)
	{
		printf("=======================\n");
		printf("%s\n", parameter_string(parameter));
		printf("-----------------------\n");

		for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			// press button to calibrate
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while calibrating '%s %s'\n",
					                     direction_string(direction), parameter_string(parameter));
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

				printf("\r%-6s : %-12hd", direction_string(direction), hand.accel[direction]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			calibration->accel.params[direction][parameter] = hand.accel[direction];
			printf("\r%-6s : %-12hd " GREEN "~" WHITE "\n", direction_string(direction), calibration->accel.params[direction][parameter]);

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while waiting for button release after calibrating '%s %s'\n",
					                     direction_string(direction), parameter_string(parameter));
					return ERROR;
				}
			}
			while (hand.button == BUTTON_PRESSED);
		}

		for (Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			// press button to calibrate
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while calibrating '%s %s'\n",
					                     finger_string(finger), parameter_string(parameter));
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

				printf("\r%-6s : %-12hd", finger_string(finger), hand.flex[finger]);
				fflush(stdout);
			}
			while (hand.button == BUTTON_RELEASED);

			// save parameter to Calibration struct
			calibration->flex.params[finger][parameter] = hand.flex[finger];
			printf("\r%-6s : %-12hd " GREEN "~" WHITE "\n", finger_string(finger), calibration->flex.params[finger][parameter]);

			if (calibration->flex.params[finger][MIN] == calibration->flex.params[finger][MAX])
			{
				log(LOG_WARNING, "MIN and MAX are the same for '%s'\n", finger_string(finger));
			}

			// wait for user to release button
			do
			{
				if (sample(&hand) == ERROR)
				{
					log(LOG_ERROR, "Sample failed while waiting for button release after calibrating '%s %s'\n",
					                     finger_string(finger), parameter_string(parameter));
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

int calibration_print(Calibration * calibration)
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
			MAX_SENSOR_NAME_LENGTH, parameter_string(MIN),
			MAX_SENSOR_NAME_LENGTH, parameter_string(MAX),
			MAX_SENSOR_NAME_LENGTH, parameter_string(CENTRE));
	printf("-------------------------------------\n");

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf("| %-*s | %*hd | %*hd | %*hd |\n",
		       MAX_SENSOR_NAME_LENGTH, direction_string(direction),
		       MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][MIN],
		       MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][MAX],
		       MAX_SENSOR_NAME_LENGTH, calibration->accel.params[direction][CENTRE]);
	}

	for (Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf("| %-*s | %*hd | %*hd | %*hd |\n",
		       MAX_SENSOR_NAME_LENGTH, finger_string(finger),
		       MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][MIN],
		       MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][MAX],
		       MAX_SENSOR_NAME_LENGTH, calibration->flex.params[finger][CENTRE]);
	}

	printf("=====================================\n");
	return SUCCESS;
}
