#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "serial.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

void end(int signal);   // signal handler

int main(void)
{
	signal(SIGINT, end);   // register signal handler for CTRL+C
	serial_open();
	char buffer [62];

	log_suppress(LOG_ERROR, false);
	log_suppress(LOG_WARNING, false);
	log_suppress(LOG_SUCCESS, false);
	log_suppress(LOG_DEBUG, false);
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

void end(int signal)
{
	printf("\n");
	serial_purge();
	serial_close();
	log_print(LOG_SUCCESS, "Terminated Dexterity\n");
	exit(0);
}
