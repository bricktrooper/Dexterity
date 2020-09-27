#include <stdio.h>

#include "log.h"

int main(void)
{
	printf("Hello World!\n");

	log_suppress(ERROR, false);
	log_suppress(WARNING, false);
	log_suppress(SUCCESS, false);
	log_suppress(DEBUG, false);
	log_suppress(INFO, false);

	log_print(ERROR, "Hello World!\n");
	log_print(WARNING, "Hello World!\n");
	log_print(SUCCESS, "Hello World!\n");
	log_print(DEBUG, "Hello World!\n");
	log_print(INFO , "Hello World!\n");
	return 0;
}
