#include <stdio.h>
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

	if (serial_purge() != SUCCESS ||
		serial_write_message(MESSAGE_SAMPLE) != SUCCESS ||
		serial_read((char *)hand, sizeof(struct Hand)) != sizeof(struct Hand))
	{
		log_print(LOG_ERROR, "%s(): Failed to sample sensor data\n", __func__);
		return ERROR;
	}

	log_print(LOG_DEBUG, "X: %d Y: %d Z: %d F1: %d F2: %d F3: %d F4: %d F5: %d BUTTON: %d LED: %d\n",
							hand->accel[X], hand->accel[Y], hand->accel[Z],
							hand->flex[F1], hand->flex[F2], hand->flex[F3], hand->flex[F4], hand->flex[F5],
							hand->button, hand->led);

	return SUCCESS;
}

int read_file(char * filename, char * data, int size)
{
	if (filename == NULL || data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	int file = open(filename, O_RDONLY);

	if (file < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, filename, strerror(errno), errno);
		return ERROR;
	}

	int retrieved = 0;

	do
	{
		int result = read(file, data + retrieved, size - retrieved);

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
	while (retrieved < size);

	if (retrieved < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to read from file '%s': %s (%d)\n", __func__, filename, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Read %dB from file '%s'\n", __func__, filename, retrieved);
	return retrieved;
}


int write_file(char * filename, char * data, int size)
{
	if (filename == NULL || data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	int file = open(filename, O_WRONLY | O_CREAT | O_EXCL);

	if (file < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, filename, strerror(errno), errno);
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
		log_print(LOG_ERROR, "%s(): Failed to write to file '%s': %s (%d)\n", __func__, filename, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Wrote %dB to file '%s'\n", __func__, filename, written);
	return written;
}
