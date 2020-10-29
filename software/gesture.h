#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"

enum Gesture
{
	GESTURE_DISABLE,   // ignore all input from device until re-enabled
	GESTURE_ENABLE,    // bring the device back from a disabled state
	GESTURE_MOVE,      // move the mouse cursor
	GESTURE_CLICK,     // click the mouse
	GESTURE_DRAG,      // drag the cursor
	GESTURE_ZOOM,      // zoom +/-
	GESTURE_SCROLL,    // volume +/-
	GESTURE_VOLUME,
	GESTURE_KEYBOARD,  //
	GESTURE_UNKNOWN,

	NUM_GESTURES
};

struct Ignore
{
	bool accel [NUM_DIRECTIONS];
	bool flex [NUM_FINGERS];
};

struct Profile
{
	struct Hand hand;
	struct Ignore ignore;
	////////////////////////int tolerance;
};

struct Binding
{
	char * name;
	struct Hand hand;
	enum Gesture gesture;

	// I feel like this can be more generalized and scalable.
	// In a file
	// save the info from this struct
	// Each gesture can have two types of actions:
	// 1. a mouse action
	// 2. a keyboard action
	// specify this in the file
	// basically you either CLICK, DRAG, MOVE, SCROLL, or press a KEY.  everything else is a keystroke
	// you can bind a gesture of n phases to any of these actions.


	// int phases;
	// struct Hand * hand; // array of readings for multiphase gestures
	// int (* action)(struct Hand * hand);
};

int gesture_load_profiles(void);
bool gesture_compare(enum Gesture gesture, struct Hand * hand);
enum Gesture gesture_identify(struct Hand * hand);

#endif /* GESTURE_H */
