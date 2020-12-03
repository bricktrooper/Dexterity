#ifndef ACTION_H
#define ACTION_H

#include "dexterity.h"

#define MAX_ACTION_NAME_LENGTH   18

enum Action
{
	ACTION_CYCLE,           // cycle between controls
	ACTION_PRESS,           // press left click
	ACTION_RELEASE,         // release left click
	ACTION_LEFT_CLICK,      // single left click (click fast for double click)
	ACTION_RIGHT_CLICK,     // single right click
	ACTION_OVERVIEW,        // show all windows
	ACTION_ZOOM_IN,         // zoom in
	ACTION_ZOOM_OUT,        // zoom out
	ACTION_SCROLL_UP,       // scroll up
	ACTION_SCROLL_DOWN,     // scroll down
	ACTION_VOLUME_CHANGE,   // increase/decrease the volume
	ACTION_VOLUME_MUTE,     // mute/unmute the volume
	ACTION_SWIPE_LEFT,      // command + left arrow
	ACTION_SWIPE_RIGHT,     // command + right arrow
	ACTION_FAST_FORWARD,    // next song (fast forward fuction key)
	ACTION_REWIND,          // previous song (rewind fuction key)
	ACTION_PAUSE_PLAY,      // pause/play media (pause/play fuction key)
	ACTION_IDLE,            // do nothing (this is a dummy action)

	NUM_ACTIONS,

	ACTION_UNKNOWN          // the gesture could not be interpreted
};

extern char * ACTIONS [NUM_ACTIONS];

char * action_string(enum Action action);

int action_move(struct Hand * hand);
int action_drag(struct Hand * hand);
int action_left_click(void);
int action_right_click(void);
int action_double_click(void);
int action_overview(void);
int action_zoom_in(void);
int action_zoom_out(void);
int action_scroll_up(struct Hand * hand);
int action_scroll_down(struct Hand * hand);
int action_volume_up(void);
int action_volume_down(void);
int action_volume_mute(void);
int action_fast_forward(void);
int action_rewind(void);
int action_pause_play(void);
int action_swipe_left(void);
int action_swipe_right(void);

#endif /* ACTION_H */
