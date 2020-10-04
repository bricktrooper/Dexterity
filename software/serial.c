#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "dexterity.h"
#include "colours.h"
#include "serial.h"
#include "log.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define SERIAL_PORT        "/dev/cu.usbserial-AQ00PEW7"   // Use 'cu' instead of 'tty' to prevent DCD (data-carrier-detect)
#define SERIAL_CLOSED     -1                              // Invalid fie descriptor for closed serial port
#define RX_READ_TIMEOUT    1                              // timeout in deciseconds (10^-1 s)
#define TX_WRITE_DELAY     1500                           // delay in us between 1B writes to TX (used to avoid RX buffer overrun on PIC)

extern int errno;

static int serial_port = SERIAL_CLOSED;
static struct termios settings;

static int serial_is_open(void)
{
	return (serial_port != SERIAL_CLOSED);
}

int serial_open(void)
{
	int rc = ERROR;

	if (serial_is_open())
	{
		log_print(LOG_WARNING, "%s(): The serial port is already open\n", __func__);
		return WARNING;
	}

	// OPEN SERIAL PORT //

	serial_port = open(SERIAL_PORT, O_RDWR);
	tcflush(serial_port, TCIOFLUSH);   // Discard any old data from RX and TX buffer

	if (serial_port < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open serial port '%s': %s (%d)\n", __func__, SERIAL_PORT, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s(): Opened serial port '%s'\n", __func__, SERIAL_PORT);

	// GET SERIAL PORT ATTRIBUTES //

	if (tcgetattr(serial_port, &settings) != 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to get serial port attributes: %s (%d)\n", __func__, strerror(errno), errno);
		goto EXIT;
	}

	// APPLY SETTINGS //

	// The serial settings are specified by applying the following masks to the appropriate flags.
	// Use these masks to either set or clear settings.
	// Code adapted from https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#overview

	// LOCAL //
	settings.c_lflag &= ~ICANON;    // disable canonical mode (newline delimiter)
	settings.c_lflag &= ~ISIG;      // disable interpretation of INTR, QUIT and SUSP characters
	settings.c_lflag &= ~IEXTEN;    // disable input processing
	settings.c_lflag &= ~ECHO;      // disable echo
	settings.c_lflag &= ~ECHOE;     // disable erasure
	settings.c_lflag &= ~ECHOK;     // disable kill erasure
	settings.c_lflag &= ~ECHONL;    // disable newline echo
	settings.c_iflag &= ~IXON;      // disable output flow control
	settings.c_iflag &= ~IXOFF;     // disable input flow control
	settings.c_iflag &= ~IXANY;     // disable restart flow control

	// CONTROL //
	settings.c_cflag &= ~PARENB;    // disable parity
	settings.c_cflag &= ~CSTOPB;    // use one stop bit
	settings.c_cflag &= ~CSIZE;     // clear data size (bits per byte)
	settings.c_cflag &= ~CRTSCTS;   // Disable RTS/CTS
	settings.c_cflag |= CS8;        // set data size to 8 bits per byte
	settings.c_cflag |= CREAD;      // Enable reading
	settings.c_cflag |= CLOCAL;     // ignore control signals

	// OUTPUT //
	settings.c_oflag &= ~OPOST;     // Disable special interpretation of output bytes (e.g. newline)
	settings.c_oflag &= ~ONLCR;     // Disable conversion of newline to carriage return/line feed
	settings.c_oflag &= ~OXTABS;    // Disable conversion of tabs to spaces
	settings.c_oflag &= ~ONOEOT;    // Disable removal of C-d chars (0x004) in output

	// INPUT //
	// disable special handling of received bytes and get raw data instead
	settings.c_iflag &= ~IGNBRK;
	settings.c_iflag &= ~BRKINT;
	settings.c_iflag &= ~PARMRK;
	settings.c_iflag &= ~ISTRIP;
	settings.c_iflag &= ~INLCR;
	settings.c_iflag &= ~IGNCR;
	settings.c_iflag &= ~ICRNL;

	// SPECIAL CHARACTERS //
	settings.c_cc[VTIME] = RX_READ_TIMEOUT;   // Timeout in deciseconds (10^-1 s) before timeout
	settings.c_cc[VMIN] = 0;                  // blocking

	// BAUD RATE //

	if (cfsetispeed(&settings, B9600) < 0)   // Set input baud rate to 9600
	{
		log_print(LOG_ERROR, "%s(): Failed to set input baud rate to %d: %s (%d)\n", __func__, B9600, strerror(errno), errno);
		goto EXIT;
	}

	if (cfsetospeed(&settings, B9600) < 0)   // Set output baud rate to 9600
	{
		log_print(LOG_ERROR, "%s(): Failed to set output baud rate to %d: %s (%d)\n", __func__, B9600, strerror(errno), errno);
		goto EXIT;
	}

	// SAVE ATTRIBUTES //

	if (tcsetattr(serial_port, TCSANOW, &settings) != 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to set serial port attributes: %s (%d)\n", __func__, strerror(errno), errno);
	}

	rc = SUCCESS;

EXIT:
	if (rc == ERROR)
	{
		serial_close();
	}

	return rc;
}

int serial_close(void)
{
	if (!serial_is_open())
	{
		log_print(LOG_WARNING, "%s(): The serial port is not open\n", __func__);
		return WARNING;
	}

	close(serial_port);
	serial_port = -1;
	log_print(LOG_SUCCESS, "%s(): Closed serial port '%s'\n", __func__, SERIAL_PORT);

	return SUCCESS;
}

int serial_purge(void)
{
	if (!serial_is_open())
	{
		log_print(LOG_ERROR, "%s(): The serial port is not open\n", __func__);
		return ERROR;
	}

	if (tcflush(serial_port, TCIFLUSH) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to purge serial port RX buffer: %s (%d)\n", __func__, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Purged serial port RX buffer\n", __func__);
	return SUCCESS;
}

int serial_read(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!serial_is_open())
	{
		log_print(LOG_ERROR, "%s(): The serial port is not open\n", __func__);
		return ERROR;
	}

	int received = 0;

	do
	{
		int result = read(serial_port, data + received, size - received);

		if (result == 0)   // done reading
		{
			break;
		}
		else if (result < 0)   // error
		{
			received = result;
			break;
		}

		received += result;
	}
	while (received < size);

	if (received != size)
	{
		if (errno == ETIMEDOUT)
		{
			log_print(LOG_WARNING, "%s(): %s (%d)\n", __func__, strerror(errno), errno);
		}
		else
		{
			log_print(LOG_ERROR, "%s(): Failed to read from serial port: %s (%d)\n", __func__, strerror(errno), errno);
			return ERROR;
		}
	}

	log_print(LOG_INFO, "%s(): Read %dB from the serial port\n", __func__, received);
	return received;
}

int serial_write(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!serial_is_open())
	{
		log_print(LOG_ERROR, "%s(): The serial port is not open\n", __func__);
		return ERROR;
	}

	int transmitted = 0;

	for (int i = 0; i < size; i++)
	{
		int result = write(serial_port, data + i, 1);
		tcdrain(serial_port);
		usleep(TX_WRITE_DELAY);

		if (result == 0)   // done writing
		{
			break;
		}
		else if (result < 0)   // error
		{
			transmitted = result;
			break;
		}
		else   // continue writing
		{
			transmitted += result;
		}
	}

	if (transmitted != size)
	{
		log_print(LOG_ERROR, "%s(): Failed to write to serial port: %s (%d)\n", __func__, strerror(errno), errno);
		return ERROR;
	}

	log_print(LOG_INFO, "%s(): Wrote %dB to the serial port\n", __func__, transmitted);
	return transmitted;
}

enum Message serial_read_message(void)
{
	char next = 0;
	char data [MESSAGE_BUFFER_SIZE];
	int length = sizeof(data);

	memset(data, 0, length);

	for (int i = 0; i < length - 1; i++)
	{
		if (serial_read(&next, 1) != 1)  // read 1B at a time
		{
			log_print(LOG_ERROR, "%s(): Failed to read message character #%d\n", __func__, i);
			return MESSAGE_UNKNOWN;
		}

		if (next == '\r')   // carriage return [Enter] indicates end of transmission
		{
			break;
		}

		data[i] = next;
	}

	for (int message = 0; message < NUM_MESSAGES; message++)
	{
		if (strncmp(data, MESSAGES[message], MESSAGE_BUFFER_SIZE) == 0)
		{
			return message;
		}
	}

	return MESSAGE_UNKNOWN;
}

int serial_write_message(enum Message message)
{
	char * data = MESSAGES[message];
	int length = strlen(data);

	if (serial_write(data, length) != length)   // read 1B at a time
	{
		log_print(LOG_ERROR, "%s(): Failed to write message string\n", __func__);
		return ERROR;
	}

	if (serial_write("\r", 1) != 1)   // carriage return [Enter] indicates end of transmission
	{
		log_print(LOG_ERROR, "%s(): Failed to write message terminator\n", __func__);
		return ERROR;
	}

	return length + 1;
}
