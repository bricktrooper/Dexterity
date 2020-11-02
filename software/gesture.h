#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"

#define MAX_GESTURE_NAME_LENGTH   18

enum Gesture
{
	GESTURE_MOVE,
	GESTURE_DRAG,
	GESTURE_LEFT_CLICK,
	GESTURE_RIGHT_CLICK,
	GESTURE_DOUBLE_CLICK,
	GESTURE_ZOOM_IN,
	GESTURE_ZOOM_OUT,
	GESTURE_SCROLL_UP,
	GESTURE_SCROLL_DOWN,
	GESTURE_VOLUME_UP,
	GESTURE_VOLUME_DOWN,
	GESTURE_CYCLE,        // switch between controls
	GESTURE_DISABLE,      // ignore all input from device until re-enabled
	GESTURE_ENABLE,       // bring the device back from a disabled state
	GESTURE_UNKNOWN,

	NUM_GESTURES
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

struct Binding
{
	enum Gesture gesture;         // the gesture to execute when this binding is activated
	int phases;                   // the number of phases in the gesture
	struct Ignore ignore;         // which sensors to ignore
	struct Criteria * criteria;   // array of profiles for multiphase gestures
	////// make a static array of these and call it from there instead using binding->gesture //int (* action)(struct Hand * hand);   // the function to call when this gesture is registered
};

extern char * CONTROLS [NUM_CONTROLS];
extern char * GESTURES [NUM_GESTURES];

//int gesture_record(char * file_name);
void gesture_binding_destroy(struct Binding * binding);
int gesture_export(char * file_name, struct Binding * binding);
int gesture_import(char * file_name, struct Binding * binding);
bool gesture_compare(enum Gesture gesture, struct Hand * hand);
enum Gesture gesture_identify(struct Hand * hand);
int gesture_execute(enum Gesture gesture, struct Hand * hand);

#endif /* GESTURE_H */
