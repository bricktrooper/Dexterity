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

int sample(struct Hand * hand)
{
	if (hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (serial_write_message(MESSAGE_SAMPLE) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send sample request to device\n", __func__);
		return ERROR;
	}

	if (serial_read(hand, sizeof(struct Hand)) != sizeof(struct Hand))
	{
		log_print(LOG_ERROR, "%s(): Failed to receive sensor data\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int raw(void)
{
	if (serial_write_message(MESSAGE_RAW) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send raw mode request to device\n", __func__);
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge raw mode request\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int scaled(void)
{
	if (serial_write_message(MESSAGE_SCALED) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send scaled mode request to device\n", __func__);
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge scaled mode request\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int upload(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (serial_write_message(MESSAGE_UPLOAD) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send calibration upload request to device\n", __func__);
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge calibration upload request\n", __func__);
		return ERROR;
	}

	if (serial_write(calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
	{
		log_print(LOG_ERROR, "%s(): Failed to send calibration data to device\n", __func__);
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge the result of calibration upload\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int download(struct Calibration * calibration)
{
	if (calibration == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (serial_write_message(MESSAGE_DOWNLOAD) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send calibration download request to device\n", __func__);
		return ERROR;
	}

	if (serial_read(calibration, sizeof(struct Calibration)) != sizeof(struct Calibration))
	{
		log_print(LOG_ERROR, "%s(): Failed to receive calibration data from device\n", __func__);
		return ERROR;
	}

	return SUCCESS;
}

int deadzone(struct Hand * hand, int radius)
{
	if (hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
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
