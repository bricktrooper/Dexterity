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
		log_print(LOG_ERROR, "%s(): Failed to send raw mode request to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge raw mode request\n");
		return ERROR;
	}

	return SUCCESS;
}

int scaled(void)
{
	if (serial_write_message(MESSAGE_SCALED) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to send scaled mode request to device\n");
		return ERROR;
	}

	if (serial_read_message() != MESSAGE_SUCCESS)
	{
		log_print(LOG_ERROR, "%s(): Device did not acknowledge scaled mode request\n");
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

int read_file(char * file_name, char ** data, int * size)
{
	if (file_name == NULL || data == NULL || size == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	*data = NULL;
	*size = -1;

	int file = open(file_name, O_RDONLY);

	if (file < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	// get file size
	if (lseek(file, 0, SEEK_SET) != 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to seek to beginning of file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	int file_size = lseek(file, 0, SEEK_SET);

	if (file_size < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to get size of file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}
	else if (file_size == 0)
	{
		log_print(LOG_WARNING, "%s(): The file '%s' is empty\n", __func__, file_name);
		return WARNING;
	}

	log_print(LOG_INFO, "%s(): Loading file '%s' of size %d\n", __func__, file_name, file_size);
	char * file_data = malloc(file_size);

	if (file_data == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for file data: %s (%d)\n", __func__, strerror(errno), errno);
		return ERROR;
	}

	int retrieved = 0;

	do
	{
		int result = read(file, file_data + retrieved, file_size - retrieved);

		if (result == 0)   // done reading
		{
			break;
		}
		else if (result < 0)   // error
		{
			retrieved = result;
			break;
		}

		retrieved += result;
	}
	while (retrieved < file_size);

	if (retrieved < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to read from file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Read %dB from file '%s'\n", __func__, file_name, retrieved);
	*data = file_data;
	*size = file_size;
	return retrieved;
}


int write_file(char * file_name, char * data, int size)
{
	if (file_name == NULL || data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	int file = open(file_name, O_WRONLY | O_CREAT | O_EXCL);

	if (file < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	int written = 0;

	do
	{
		int result = write(file, data + written, size - written);

		if (result == 0)   // done writing
		{
			break;
		}
		else if (result < 0)   // error
		{
			written = result;
			break;
		}
		else   // continue writing
		{
			written += result;
		}
	}
	while (written < size);

	if (written < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Wrote %dB to file '%s'\n", __func__, file_name, written);
	return written;
}

int average(int * data, int elements, float * avg)
{
	if (data == NULL || elements <= 0 || avg == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	int sum = 0;

	for (int i = 0; i < elements; i++)
	{
		sum += data[i];
	}

	*avg = (float)sum / elements;
	return SUCCESS;
}
