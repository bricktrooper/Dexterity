#include <stdio.h>

#include "log.h"

int main(void)
{
	printf("Hello World!\n");
	log_error("Hello World!\n");
	log_warning("Hello World!\n");
	log_success("Hello World!\n");
	log_debug("Hello World!\n");
	log_info("Hello World!\n");
	return 0;
}
