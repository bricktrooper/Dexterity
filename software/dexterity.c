#include <stdio.h>

#include "messages.h"

int main(void)
{
	printf("Hello World!\n");
	print_error("Hello World!\n");
	print_warning("Hello World!\n");
	print_success("Hello World!\n");
	print_debug("Hello World!\n");
	print_info("Hello World!\n");
	return 0;
}
