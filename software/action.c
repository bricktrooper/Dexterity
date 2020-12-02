#include <stdlib.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "mouse.h"
#include "keyboard.h"

#include "action.h"

char * ACTIONS [NUM_ACTIONS] = {
	"CYCLE",
	"PRESS",
	"RELEASE",
	"LEFT_CLICK",
	"RIGHT_CLICK",
	"OVERVIEW",
	"ZOOM_IN",
	"ZOOM_OUT",
	"SCROLL_UP",
	"SCROLL_DOWN",
	"CHANGE_VOLUME",
	"SWIPE_LEFT",
	"SWIPE_RIGHT",
	"IDLE"
};

int action_move(struct Hand * hand)
{
	deadzone(hand, CURSOR_DEADZONE_RADIUS);   // apply deadzone to accel values
	int x = -hand->accel[Z];
	int y = -hand->accel[X];
	return mouse_move(x, y);
}

int action_drag(struct Hand * hand)
{
	deadzone(hand, CURSOR_DEADZONE_RADIUS);   // apply deadzone to accel values
	int x = -hand->accel[Z];
	int y = -hand->accel[X];
	return mouse_drag(MOUSE_BUTTON_LEFT, x, y);
}

int action_left_click(void)
{
	return mouse_single_click(MOUSE_BUTTON_LEFT);
}

int action_right_click(void)
{
	return mouse_single_click(MOUSE_BUTTON_RIGHT);
}

int action_double_click(void)
{
	return mouse_double_click(MOUSE_BUTTON_LEFT);
}

int action_overview(void)
{
	return keyboard_tap(KEY_MISSION_CONTROL);
}

int action_zoom_in(void)
{
	enum Key stroke [] = {KEY_COMMAND, KEY_EQUAL};
	return keyboard_combo(stroke, sizeof(stroke) / sizeof(enum Key));
}

int action_zoom_out(void)
{
	enum Key stroke [] = {KEY_COMMAND, KEY_MINUS};
	return keyboard_combo(stroke, sizeof(stroke) / sizeof(enum Key));
}

int action_scroll_up(struct Hand * hand)
{
	return mouse_scroll(MOUSE_SCROLL_UP, abs(hand->accel[Z]));
}

int action_scroll_down(struct Hand * hand)
{
	return mouse_scroll(MOUSE_SCROLL_DOWN, abs(hand->accel[Z]));
}

int action_volume_up(void)
{
	return keyboard_tap(KEY_VOLUME_UP);
}
int action_volume_down(void)
{
	return keyboard_tap(KEY_VOLUME_DOWN);
}

int action_swipe_left(void)
{
	enum Key stroke [] = {KEY_COMMAND, KEY_LEFT_ARROW};
	return keyboard_combo(stroke, sizeof(stroke) / sizeof(enum Key));
}

int action_swipe_right(void)
{
	enum Key stroke [] = {KEY_COMMAND, KEY_RIGHT_ARROW};
	return keyboard_combo(stroke, sizeof(stroke) / sizeof(enum Key));
}
