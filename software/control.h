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
	CONTROL_MUSIC,     // change, pause, and play songs/media
	CONTROL_SWIPE,     // command + left/right arrow keys

	NUM_CONTROLS
};

typedef enum Control Control;

char * control_string(Control control);
int control_execute(Control control, Gesture * gestures, int quantity, Hand * hand);

#endif /* CONTROL_H */
