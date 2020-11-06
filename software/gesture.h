#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"
#include "action.h"

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

struct Ignores
{
	bool accel [NUM_DIRECTIONS];
	bool flex [NUM_FINGERS];
};

struct Sensors
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
};

struct Gesture
{
	enum Action action;          // the gesture to execute when this binding is activated
	int phases;                  // the number of phases in the gesture
	// float tolerance/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Ignores ignores;      // which sensors to ignore
	struct Sensors * criteria;   // array of criteria for multiphase gestures
};

extern char * CONTROLS [NUM_CONTROLS];

struct Gesture * gesture_create(int quantity);
int gesture_destroy(struct Gesture * gestures, int quantity);
bool gesture_valid(struct Gesture * gesture);
int gesture_import(char * file_name, struct Gesture ** gestures, int * quantity);
int gesture_export(char * file_name, struct Gesture * gestures, int quantity);
int gesture_record(struct Gesture * gesture);
bool gesture_compare(struct Gesture * gesture, struct Hand * hand, int phase);

enum Action gesture_identify(struct Gesture * gesture, struct Hand * hand, int phase);

int gesture_control(enum Control control, struct Hand * hand);
int gesture_print(struct Gesture * gesture);

#endif /* GESTURE_H */
