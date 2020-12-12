#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "dexterity.h"
#include "log.h"
#include "serial.h"

#include "utils.h"

int sample(Hand * hand)
{
	if (hand == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (serial_write_message(MESSAGE_SAMPLE) == ERROR)
	{
		log(LOG_ERROR, "Failed to send sample request to device\n");
		return ERROR;
	}

	if (serial_read(hand, sizeof(Hand)) != sizeof(Hand))
	{
		log(LOG_ERROR, "Failed to receive sensor data\n");
		return ERROR;
	}

	return SUCCESS;
}

int raw(void)
{
	if (serial_write_message(MESSAGE_RAW) == ERROR)
	{
		log(LOG_ERROR, "Failed to send raw mode request to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log(LOG_ERROR, "Device did not acknowledge raw mode request\n");
		return ERROR;
	}

	return SUCCESS;
}

int scaled(void)
{
	if (serial_write_message(MESSAGE_SCALED) == ERROR)
	{
		log(LOG_ERROR, "Failed to send scaled mode request to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log(LOG_ERROR, "Device did not acknowledge scaled mode request\n");
		return ERROR;
	}

	return SUCCESS;
}

int upload(Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (serial_write_message(MESSAGE_UPLOAD) == ERROR)
	{
		log(LOG_ERROR, "Failed to send calibration upload request to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log(LOG_ERROR, "Device did not acknowledge calibration upload request\n");
		return ERROR;
	}

	if (serial_write(calibration, sizeof(Calibration)) != sizeof(Calibration))
	{
		log(LOG_ERROR, "Failed to send calibration data to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log(LOG_ERROR, "Device did not acknowledge the result of calibration upload\n");
		return ERROR;
	}

	return SUCCESS;
}

int download(Calibration * calibration)
{
	if (calibration == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (serial_write_message(MESSAGE_DOWNLOAD) == ERROR)
	{
		log(LOG_ERROR, "Failed to send calibration download request to device\n");
		return ERROR;
	}

	if (serial_read(calibration, sizeof(Calibration)) != sizeof(Calibration))
	{
		log(LOG_ERROR, "Failed to receive calibration data from device\n");
		return ERROR;
	}

	return SUCCESS;
}

int deadzone(Hand * hand, int radius)
{
	if (hand == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		if (abs(hand->accel[direction]) <= radius)
		{
			hand->accel[direction] = 0;
		}
		else if (hand->accel[direction] > 0)
		{
			hand->accel[direction] -= radius;
		}
		else if (hand->accel[direction] < 0)
		{
			hand->accel[direction] += radius;
		}
	}

	return SUCCESS;
}
