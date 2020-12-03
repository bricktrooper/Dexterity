#include <stdlib.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "mouse.h"
#include "keyboard.h"

#include "action.h"

char * action_string(enum Action action)
{
	switch (action)
	{
		case ACTION_CYCLE:           return "CYCLE";
		case ACTION_PRESS:           return "PRESS";
		case ACTION_RELEASE:         return "RELEASE";
		case ACTION_LEFT_CLICK:      return "LEFT_CLICK";
		case ACTION_RIGHT_CLICK:     return "RIGHT_CLICK";
		case ACTION_OVERVIEW:        return "OVERVIEW";
		case ACTION_ZOOM_IN:         return "ZOOM_IN";
		case ACTION_ZOOM_OUT:        return "ZOOM_OUT";
		case ACTION_SCROLL_UP:       return "SCROLL_UP";
		case ACTION_SCROLL_DOWN:     return "SCROLL_DOWN";
		case ACTION_VOLUME_CHANGE:   return "VOLUME_CHANGE";
		case ACTION_VOLUME_MUTE:     return "VOLUME_MUTE";
		case ACTION_SWIPE_LEFT:      return "SWIPE_LEFT";
		case ACTION_SWIPE_RIGHT:     return "SWIPE_RIGHT";
		case ACTION_FAST_FORWARD:    return "FAST_FORWARD";
		case ACTION_REWIND:          return "REWIND";
		case ACTION_PAUSE_PLAY:      return "PAUSE_PLAY";
		case ACTION_IDLE:            return "IDLE";
		case ACTION_UNKNOWN:         return "UNKNOWN";
		default:                     return "INVALID";
	}
}

int action_move(struct Hand * hand)
{
	deadzone(hand, CURSOR_DEADZONE);   // apply deadzone to accel values
	int x = -hand->accel[Z];
	int y = -hand->accel[X];
	return mouse_move(x, y);
}

int action_drag(struct Hand * hand)
{
	deadzone(hand, CURSOR_DEADZONE);   // apply deadzone to accel values
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

int action_volume_mute(void)
{
	return keyboard_tap(KEY_MUTE);
}

int action_fast_forward(void)
{
	return keyboard_tap(KEY_FAST_FORWARD);
}

int action_rewind(void)
{
	return keyboard_tap(KEY_REWIND);
}

int action_pause_play(void)
{
	return keyboard_tap(KEY_PAUSE_PLAY);
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
