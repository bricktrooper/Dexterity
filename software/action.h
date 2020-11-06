#ifndef ACTION_H
#define ACTION_H

#include "dexterity.h"

#define MAX_ACTION_NAME_LENGTH   18

enum Action
{
	ACTION_MOVE,           // move the mouse cursor
	ACTION_DRAG,           // drag left click
	ACTION_LEFT_CLICK,     // single left click
	ACTION_RIGHT_CLICK,    // single right click
	ACTION_DOUBLE_CLICK,   // double left click
	ACTION_ZOOM_IN,        // zoom in
	ACTION_ZOOM_OUT,       // zoom out
	ACTION_SCROLL_UP,      // scroll up
	ACTION_SCROLL_DOWN,    // scroll down
	ACTION_VOLUME_UP,      // increase the volume
	ACTION_VOLUME_DOWN,    // decrease the volume
	ACTION_CYCLE,          // cycle between controls

	NUM_ACTIONS,

	ACTION_UNKNOWN,        // the gesture could not be interpreted
	ACTION_DISABLE,        // ignore all input from device until re-enabled (button triggered)
	ACTION_ENABLE          // bring the device back from a disabled state (button triggered)
};

#endif /* ACTION_H */

extern char * ACTIONS [NUM_ACTIONS];

int action_move(struct Hand * hand);
int action_drag(struct Hand * hand);
int action_left_click(struct Hand * hand);
int action_right_click(struct Hand * hand);
int action_double_click(struct Hand * hand);
int action_zoom_in(struct Hand * hand);
int action_zoom_out(struct Hand * hand);
int action_scroll_up(struct Hand * hand);
int action_scroll_down(struct Hand * hand);
int action_volume_up(struct Hand * hand);
int action_volume_down(struct Hand * hand);
int action_disable(struct Hand * hand);
int action_enable(struct Hand * hand);
