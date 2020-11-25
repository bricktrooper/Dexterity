#ifndef CONTROL_H
#define CONTROL_H

#include "dexterity.h"
#include "gesture.h"

enum Control
{
	CONTROL_MOUSE,     // move the mouse cursor and click mouse buttons
	CONTROL_ZOOM,      // zoom +/-
	CONTROL_SCROLL,    // scroll up/down
	CONTROL_VOLUME,    // volume +/-
	CONTROL_SWIPE,     // command + left/right arrow keys

	NUM_CONTROLS
};

extern char * CONTROLS [NUM_CONTROLS];

int control_execute(enum Control control, struct Gesture * gestures, int quantity, struct Hand * hand);

#endif /* CONTROL_H */
