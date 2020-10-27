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

	if (serial_purge() == ERROR ||
		serial_write_message(MESSAGE_SAMPLE) == ERROR ||
		serial_read((char *)hand, sizeof(struct Hand)) != sizeof(struct Hand))
	{
		log_print(LOG_ERROR, "%s(): Failed to sample sensor data\n", __func__);
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
