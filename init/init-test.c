/* Test code for PIC16F690 startup */

#include <xc.h>
#include <pic16f690.h>

#include "init.h"

void main(void)
{
	init_hardware();

#ifdef PIC_TEST_BOARD
	while (1)
	{
		PORTCbits.RC0 = RA5;
		PORTCbits.RC1 = RA4;
	}
#endif
}
