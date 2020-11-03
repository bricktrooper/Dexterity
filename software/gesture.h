#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"

#define MAX_ACTION_NAME_LENGTH   18

enum Action
{
	ACTION_MOVE,
	ACTION_DRAG,
	ACTION_LEFT_CLICK,
	ACTION_RIGHT_CLICK,
	ACTION_DOUBLE_CLICK,
	ACTION_ZOOM_IN,
	ACTION_ZOOM_OUT,
	ACTION_SCROLL_UP,
	ACTION_SCROLL_DOWN,
	ACTION_VOLUME_UP,
	ACTION_VOLUME_DOWN,
	ACTION_CYCLE,        // switch between controls
	ACTION_DISABLE,      // ignore all input from device until re-enabled
	ACTION_ENABLE,       // bring the device back from a disabled state
	ACTION_UNKNOWN,

	NUM_ACTIONS
};

enum Control
{
	CONTROL_MOUSE,     // move the mouse cursor and click mouse buttons
	CONTROL_ZOOM,      // zoom +/-
	CONTROL_SCROLL,    // scroll up/down
	CONTROL_VOLUME,    // volume +/-

	NUM_CONTROLS
};

// OKAY NEVERMID
// this is what we're gonna do
// the glove is very effective when it's only doing one type of gesture
// just use the button (praise the button) to cycle between gestures
// display the active gesture in the terminal
// would be cool if you could put it in the task bar or something
// for disable/enable, try to do a long button press.
// every time you sample, check the button.
// if it's pressed, wait for a release
// have a timeout for release.
// if release never comes, disable.
// wait for next short press and release.
// during disable, flash the LED via a special disabled message which you send every couple <------ NVM forget this, it's super extra anyways
// just display enabled/disabled in the terminal
// clicks/drags should be dealt with under mouse mode via a state machine that takes the hand as an input (provided disabled or gesture switch does not happen)
// zoom, scroll, and volume can be separate button switc
// no need for profiles anymore? - no we still need it

// perhaps we can use the thumb ben to switch profiles instead?
// that way there are no conflicts for enable/disable

// switch controls by curlingall fingers into a fist and pressing with thumb

// plan
// 1.
// 2. switch gestures using a thumb press

struct Ignore
{
	bool accel [NUM_DIRECTIONS];
	bool flex [NUM_FINGERS];
};

struct Criteria
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
};

struct Gesture
{
	enum Action action;         // the gesture to execute when this binding is activated
	int phases;                   // the number of phases in the gesture
	struct Ignore ignore;         // which sensors to ignore
	struct Criteria * criteria;   // array of criteria for multiphase gestures
};

//int gesture_record(char * file_name);
void gesture_create(struct Gesture * gesture);
void gesture_destroy(struct Gesture * gesture);
int gesture_export(char * file_name, struct Gesture * gesture);
int gesture_import(char * file_name, struct Gesture * gesture);
bool gesture_compare(enum Action action, struct Hand * hand);
//enum Action action_identify(struct Hand * hand);
int gesture_execute(enum Action action, struct Hand * hand);
int gesture_print(struct Gesture * gesture);

#endif /* GESTURE_H */
