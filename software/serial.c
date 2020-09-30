#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "colours.h"
#include "serial.h"
#include "log.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define SERIAL_PORT  "/dev/cu.usbserial-AL065BVB"   // Use 'cu' instead of 'tty' to prevent DCD (data-carrier-detect)
// +++++++++++=======++++++++++++++++++++++++++++++++++ASDASDAS+++++++++++++++++++++++ chane the device name back
extern int errno;

static int serial_port = -1;
static struct termios settings;

int serial_open(void)
{
	int rc = ERROR;

	// OPEN SERIAL PORT //
	serial_port = open(SERIAL_PORT, O_RDWR);
	tcflush(serial_port, TCIOFLUSH);   // Discard any old data from RX and TX buffer

	if (serial_port < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to open serial port '%s': %s (%d)\n", __func__, SERIAL_PORT, strerror(errno), errno);
		goto EXIT;
	}

	log_print(LOG_SUCCESS, "%s(): Opened serial port '%s'\n", __func__, SERIAL_PORT);

	// GET SERIAL PORT ATTRIBUTES //

	cfmakeraw(&settings);

	// if (tcgetattr(serial_port, &settings) != 0)
	// {
	// 	log_print(LOG_ERROR, "%s(): Failed to get serial port attributes: %s (%d)\n", __func__, strerror(errno), errno);
	// 	goto EXIT;
	// }

	// APPLY SETTINGS //

	// The serial settings are specified by applying the following masks to the appropriate flags.
	// Use these masks to either set or clear settings.
	// Code adapted from https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#overview

	// local modes
	settings.c_lflag &= ~ICANON;
	settings.c_lflag &= ~ISIG;
	settings.c_lflag &= ~IEXTEN;
	settings.c_lflag &= ~ECHO;
	settings.c_lflag &= ~ECHOE;
	settings.c_lflag &= ~ECHOK;
	settings.c_lflag &= ~ECHOKE;
	settings.c_lflag &= ~ECHONL;
	settings.c_lflag &= ~ECHOCTL;
	settings.c_lflag &= ~ECHOPRT;
	settings.c_lflag &= ~ALTWERASE;
	settings.c_lflag &= ~NOFLSH;
	settings.c_lflag &= ~TOSTOP;
	settings.c_lflag &= ~FLUSHO;
	settings.c_lflag &= ~PENDIN;
	settings.c_lflag &= ~NOKERNINFO;
	settings.c_lflag &= ~EXTPROC;

	// input modes
	settings.c_iflag &= ~ISTRIP;
	settings.c_iflag &= ~ICRNL;
	settings.c_iflag &= ~INLCR;
	settings.c_iflag &= ~IGNCR;
	settings.c_iflag &= ~IXON;
	settings.c_iflag &= ~IXOFF;
	settings.c_iflag &= ~IXANY;
	settings.c_iflag &= ~IMAXBEL;
	settings.c_iflag &= ~IUTF8;
	settings.c_iflag &= ~IGNBRK;
	settings.c_iflag &= ~BRKINT;
	settings.c_iflag &= ~INPCK;
	settings.c_iflag &= ~IGNPAR;
	settings.c_iflag &= ~PARMRK;

	// output modes
	settings.c_oflag &= ~OPOST;
	settings.c_oflag &= ~ONLCR;
	settings.c_oflag &= ~OXTABS;
	settings.c_oflag &= ~ONOCR;
	settings.c_oflag &= ~ONLRET;

	// control modes
	settings.c_cflag &= ~PARENB;
	settings.c_cflag &= ~PARODD;
	settings.c_cflag &= ~CSTOPB;
	settings.c_cflag &= ~CRTSCTS;
	settings.c_cflag &= ~CSIZE;
	// settings.c_cflag &= ~DSRFLOW;
	// settings.c_cflag &= ~DTRFLOW;
	settings.c_cflag &= ~CDSR_OFLOW;
	settings.c_cflag &= ~CDTR_IFLOW;
	settings.c_cflag &= ~MDMBUF;

	settings.c_cflag |= CREAD;
	settings.c_cflag |= CS8;
	settings.c_cflag |= HUPCL;
	settings.c_cflag |= CLOCAL;

	// settings.c_lflag &= ~ICANON;                   // disable canonical mode (newline delimiter)
	// settings.c_lflag &= ~ISIG;                     // disable interpretation of INTR, QUIT and SUSP characters
	// settings.c_lflag &= ~IEXTEN;                   // disable input processing
	// settings.c_lflag &= ~ECHO;                     // disable echo
	// settings.c_lflag &= ~ECHOE;                    // disable erasure
	// settings.c_lflag &= ~ECHOK;

	// settings.c_lflag &= ~ECHONL;                   // disable newline echo
	// settings.c_iflag &= ~(IXON | IXOFF | IXANY);   // disable software flow ctrl

	// settings.c_cflag &= ~PARENB;                   // disable parity
	// settings.c_cflag &= ~CSTOPB;                   // use one stop bit
	// settings.c_cflag &= ~CSIZE;                    // clear data size (bits per byte)
	// settings.c_cflag |= CS8;                       // set data size to 8 bits per byte
	// settings.c_cflag &= ~CRTSCTS;                  // Disable RTS/CTS
	// settings.c_cflag |= CREAD | CLOCAL;            // Enable reading and ignore control signals

	// settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);   // disable special handling of received bytes (gives us raw data)

	// settings.c_oflag &= ~OPOST;                    // Disable special interpretation of output bytes (e.g. newline)
	// settings.c_oflag &= ~ONLCR;                    // Disable conversion of newline to carriage return/line feed
	// settings.c_oflag &= ~OXTABS;                   // Disable conversion of tabs to spaces
	// settings.c_oflag &= ~ONOEOT;                   // Disable removal of C-d chars (0x004) in output

	settings.c_cc[VTIME] = 1;                      // Timeout in deciseconds (10^-1 s) before timeout
	settings.c_cc[VMIN] = 0;                       // blocking

	// SET BAUD RATE //

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
	if (serial_port < 0)
	{
		log_print(LOG_WARNING, "%s(): Invalid serial port file descriptor '%d'\n", __func__, serial_port);
		return WARNING;
	}

	close(serial_port);
	serial_port = -1;
	log_print(LOG_SUCCESS, "%s(): Closed serial port '%s'\n", __func__, SERIAL_PORT);

	return SUCCESS;
}

int serial_read(char * data, int size)
{
	if (data == NULL || size < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (serial_port < 0)
	{
		log_print(LOG_ERROR, "%s(): Serial port is not open\n", __func__);
		return ERROR;
	}

	int received = read(serial_port, data, size);

	if (received < 0)
	{
		log_print(LOG_ERROR, "%s(): Failed to read from serial port: %s (%d)\n", __func__, strerror(errno), errno);
		return ERROR;
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

	if (serial_port < 0)
	{
		log_print(LOG_ERROR, "%s(): Serial port is not open\n", __func__);
		return ERROR;
	}

	int transmitted = 0;

	for (int i = 0; i < size; i++)
	{
		int ret = write(serial_port, data + i, 1);
		tcdrain(serial_port);
		usleep(15000);

		if (ret == 0)
		{
			break;
		}
		else if (ret < 0)
		{
			transmitted = ret;
		}
		else
		{
			transmitted += ret;
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
