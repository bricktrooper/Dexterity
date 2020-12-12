#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "gesture.h"
#include "mouse.h"

#include "control.h"

char * control_string(enum Control control)
{
	switch (control)
	{
		case CONTROL_MOUSE:    return "MOUSE";
		case CONTROL_ZOOM:     return "ZOOM";
		case CONTROL_SCROLL:   return "SCROLL";
		case CONTROL_VOLUME:   return "VOLUME";
		case CONTROL_MUSIC:    return "MUSIC";
		case CONTROL_SWIPE:    return "SWIPE";
		default:               return "INVALID";
	}
}

static int control_mouse(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	static bool pressed = false;

	if (gesture_matches(ACTION_LEFT_CLICK, gestures, quantity, hand))
	{
		static clock_t previous = 0;   // records the time of the previous click

		clock_t delay = clock() - previous;
		previous = clock();

		if (delay < DOUBLE_CLICK_DELAY_US)
		{
			printf("%-*s", MAX_ACTION_NAME_LENGTH, "double click");
			return action_double_click();
		}
		else
		{
			printf("%-*s", MAX_ACTION_NAME_LENGTH, "left click");
			return action_left_click();
		}
	}
	else if (gesture_matches(ACTION_RIGHT_CLICK, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "right click");
		return action_right_click();
	}
	else if (gesture_matches(ACTION_OVERVIEW, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "overview");
		return action_overview();
	}
	else if (!pressed && gesture_matches(ACTION_PRESS, gestures, quantity, hand))
	{
		//printf("%-*s", MAX_ACTION_NAME_LENGTH, "press");
		mouse_press(MOUSE_BUTTON_LEFT);
		pressed = true;
	}
	else if (pressed && gesture_matches(ACTION_RELEASE, gestures, quantity, hand))
	{
		//printf("%-*s", MAX_ACTION_NAME_LENGTH, "release");
		mouse_release(MOUSE_BUTTON_LEFT);
		pressed = false;
	}

	if (pressed)
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "drag");
		return action_drag(hand);
	}
	else
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "cursor");
		return action_move(hand);
	}
}

static int control_zoom(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] > 0 && gesture_matches(ACTION_ZOOM_IN, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "in");
		return action_zoom_in();
	}
	else if (hand->accel[Z] < 0 && gesture_matches(ACTION_ZOOM_OUT, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "out");
		return action_zoom_out();
	}

	printf("%-*s", MAX_ACTION_NAME_LENGTH, "idle");
	return SUCCESS;
}

static int control_swipe(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] > SWIPE_ACTIVATION && gesture_matches(ACTION_SWIPE_LEFT, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "left");
		return action_swipe_left();
	}
	else if (hand->accel[Z] < -SWIPE_ACTIVATION && gesture_matches(ACTION_SWIPE_RIGHT, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "right");
		return action_swipe_right();
	}

	printf("%-*s", MAX_ACTION_NAME_LENGTH, "idle");
	return SUCCESS;
}

static int control_scroll(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] < -SCROLL_ACTIVATION && gesture_matches(ACTION_SCROLL_DOWN, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "down");
		return action_scroll_down(hand);
	}
	else if (hand->accel[Z] > SCROLL_ACTIVATION && gesture_matches(ACTION_SCROLL_UP, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "up");
		return action_scroll_up(hand);
	}

	printf("%-*s", MAX_ACTION_NAME_LENGTH, "idle");
	return SUCCESS;
}

static int control_volume(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	static bool reset = false;

	if (abs(hand->accel[Z]) < VOLUME_DEADZONE)
	{
		reset = false;
	}

	if (!reset && gesture_matches(ACTION_VOLUME_CHANGE, gestures, quantity, hand))
	{
		if (hand->accel[Z] < -VOLUME_ACTIVATION)
		{
			reset = true;
			printf("%-*s", MAX_ACTION_NAME_LENGTH, "up");
			return action_volume_up();
		}
		else if (hand->accel[Z] > VOLUME_ACTIVATION)
		{
			reset = true;
			printf("%-*s", MAX_ACTION_NAME_LENGTH, "down");
			return action_volume_down();
		}
		else
		{
			printf("%-*s", MAX_ACTION_NAME_LENGTH, "ready");
		}
	}
	else if (!reset && gesture_matches(ACTION_VOLUME_MUTE, gestures, quantity, hand))
	{
		reset = true;
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "mute");
		return action_volume_mute();   // toggles between muted and unmuted
	}
	else
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "idle");
	}

	return SUCCESS;
}

static int control_music(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (gesture_matches(ACTION_PAUSE_PLAY, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "pause/play");
		return action_pause_play();
	}
	else if (hand->accel[Z] > MUSIC_ACTIVATION && gesture_matches(ACTION_FAST_FORWARD, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "fast forward");
		return action_fast_forward();
	}
	else if (hand->accel[Z] < -MUSIC_ACTIVATION && gesture_matches(ACTION_REWIND, gestures, quantity, hand))
	{
		printf("%-*s", MAX_ACTION_NAME_LENGTH, "rewind");
		return action_rewind();
	}

	printf("%-*s", MAX_ACTION_NAME_LENGTH, "idle");
	return SUCCESS;
}

int control_execute(enum Control control, struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (control >= NUM_CONTROLS || gestures == NULL || quantity < 0 || hand == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	int result = ERROR;
printf("\r");
	printf("%-*s ", MAX_CONTROL_NAME_LENGTH, control_string(control));

	switch (control)
	{
		case CONTROL_ZOOM:
			result = control_zoom(gestures, quantity, hand);
			break;

		case CONTROL_SCROLL:
			result = control_scroll(gestures, quantity, hand);
			break;

		case CONTROL_VOLUME:
			result = control_volume(gestures, quantity, hand);
			break;

		case CONTROL_SWIPE:
			result = control_swipe(gestures, quantity, hand);
			break;

		case CONTROL_MUSIC:
			result = control_music(gestures, quantity, hand);
			break;

		case CONTROL_MOUSE:
		default:
			result = control_mouse(gestures, quantity, hand);
			break;
	}
fflush(stdout);
	return result;
}
