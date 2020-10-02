#include <xc.h>
#include <pic16f690.h>

#include "dexterity.h"
#include "utils.h"

#define DELAY_1S    1000000
#define DELAY_1MS   1000
#define DELAY_1US   1

int delay_s(long duration)
{
	if (duration < 0)
	{
		return ERROR;
	}

	for (long i = 0; i < duration; i++)
	{
		_delay(DELAY_1S);
	}

	return SUCCESS;
}

int delay_ms(long duration)
{
	if (duration < 0)
	{
		return ERROR;
	}

	for (long i = 0; i < duration; i++)
	{
		_delay(DELAY_1MS);
	}

	return SUCCESS;
}

int delay_us(long duration)
{
	if (duration < 0)
	{
		return ERROR;
	}

	for (long i = 0; i < duration; i++)
	{
		_delay(DELAY_1US);
	}

	return SUCCESS;
}
