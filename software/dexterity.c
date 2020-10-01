#include <stdio.h>
#include <string.h>
#include <unistd.h>


extern int serial_port;



#include <termios.h>

#include "log.h"
#include "serial.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

int main(void)
{
	serial_open();
	char buffer [62];

	log_suppress(LOG_INFO, true);

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		serial_purge();   // Discard any old data from RX buffer before making a new request
		serial_write("sample\r", strlen("sample\r"));
		serial_read(buffer, sizeof(buffer) - 1);
		printf("%s\n", buffer);
	}

	serial_close();
	return SUCCESS;
}
