#include <xc.h>
#include <pic16f690.h>

#include "pins.h"
#include "button.h"

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

void button_init(void)
{
	BUTTON_TRIS = 1;   // set button as input
}

int button_pressed(void)
{
	return BUTTON;
}

int button_released(void)
{
	return !BUTTON;
}

void button_wait_press(void)
{
	while (button_released());
}

void button_wait_release(void)
{
	while (button_pressed());
}

void button_wait_push(void)
{
	button_wait_press();
	button_wait_release();
}
