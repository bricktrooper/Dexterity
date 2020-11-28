#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#include "dexterity.h"
#include "log.h"
#include "gesture.h"
#include "mouse.h"

#include "control.h"

#define DOUBLE_CLICK_DELAY_US   20000

char * CONTROLS [NUM_CONTROLS] = {
	"MOUSE",
	"ZOOM",
	"SCROLL",
	"VOLUME",
	"SWIPE"
};




#include "keyboard.h" //////////////////////////////////////////////////////////////////////////



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
			printf("double click\n");
			return action_double_click();
		}
		else
		{
			printf("left click\n");
			return action_left_click();
		}
	}
	else if (gesture_matches(ACTION_RIGHT_CLICK, gestures, quantity, hand))
	{
		printf("right click\n");
		return action_right_click();
		//enum Key stroke [] = {KEY_COMMAND, KEY_F3};
	//return keyboard_combo(stroke, 2);

	// maybe we need to flush it or something
	}
	//else if (gesture_matches(ACTION_OVERVIEW, gestures, quantity, hand))
	//{
	//	printf("overview");
	//	return action_overview();
	//}
	else if (!pressed && gesture_matches(ACTION_PRESS, gestures, quantity, hand))
	{
		printf("press\n");
		mouse_press(MOUSE_BUTTON_LEFT);
		pressed = true;
	}
	else if (pressed && gesture_matches(ACTION_RELEASE, gestures, quantity, hand))
	{
		printf("release\n");
		mouse_release(MOUSE_BUTTON_LEFT);
		pressed = false;
	}

	if (pressed)
	{
		printf("drag\n");
		return action_drag(hand);
	}
	else
	{
		printf("cursor\n");
		return action_move(hand);
	}
}

static int control_zoom(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] > 0 && gesture_matches(ACTION_ZOOM_IN, gestures, quantity, hand))
	{
		printf("in\n");
		return action_zoom_in();
	}
	else if (hand->accel[Z] < 0 && gesture_matches(ACTION_ZOOM_OUT, gestures, quantity, hand))
	{
		printf("out\n");
		return action_zoom_out();
	}

	printf("idle\n");
	return SUCCESS;
}

static int control_swipe(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] > 0 && gesture_matches(ACTION_SWIPE_LEFT, gestures, quantity, hand))
	{
		printf("left\n");
		return action_swipe_left();
	}
	else if (hand->accel[Z] < 0 && gesture_matches(ACTION_SWIPE_RIGHT, gestures, quantity, hand))
	{
		printf("right\n");
		return action_swipe_right();
	}

	printf("idle\n");
	return SUCCESS;
}

static int control_scroll(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (hand->accel[Z] < 0 && gesture_matches(ACTION_SCROLL_DOWN, gestures, quantity, hand))
	{
		printf("down\n");
		return action_scroll_down(hand);
	}
	else if (hand->accel[Z] > 0 && gesture_matches(ACTION_SCROLL_UP, gestures, quantity, hand))
	{
		printf("up\n");
		return action_scroll_up(hand);
	}

	printf("idle\n");
	return SUCCESS;
}
static int control_volume(struct Gesture * gestures, int quantity, struct Hand * hand)
{
	printf("idle\n");
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

	printf("%s ", CONTROLS[control]);

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

		case CONTROL_MOUSE:
		default:
			result = control_mouse(gestures, quantity, hand);
			break;
	}

	return result;
}
