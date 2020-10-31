#include <stdio.h>
#include <math.h>

#include "dexterity.h"
#include "utils.h"
#include "log.h"

#include "gesture.h"

#define TOLERANCE   2.0   // how much deviation is tolerated in the confidence (average of the individual deviations)

static struct Profile profiles [NUM_GESTURES];

int gesture_load_profiles(void)
{
	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		profiles[GESTURE_MOVE].ignore.accel[direction] = true;
		profiles[GESTURE_MOVE].hand.accel[direction] = 0;
	}

	// all fingers need to be straight to move cursor
	// don't care about accel
	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		profiles[GESTURE_MOVE].ignore.flex[finger] = false;
		profiles[GESTURE_MOVE].hand.flex[finger] = 0;
	}

	return SUCCESS;
}

bool gesture_compare(enum Gesture gesture, struct Hand * hand)
{
	if (hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return false;
	}

	// hardcoded for hand right now.  Don't consider aceleration
	int deviation [NUM_FINGERS];

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		if (profiles[GESTURE_MOVE].ignore.flex[finger])
		{
			deviation[finger] = 0;
			continue;
		}

		deviation[finger] = profiles[GESTURE_MOVE].hand.flex[finger] - hand->flex[finger];
	}

	float confidence = TOLERANCE + 1;   // assume it is not a match

	if (average(deviation, NUM_FINGERS, &confidence) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to calulate average deviation\n", __func__);
		return false;
	}

	printf("%d %d %d %d %d -> %f\n", deviation[THUMB], deviation[INDEX], deviation[MIDDLE], deviation[RING], deviation[PINKY], confidence);

	if (fabs(confidence) < TOLERANCE)
	{
		return true;
	}

	return false;
}

enum Gesture gesture_identify(struct Hand * hand)
{
	if (hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return GESTURE_UNKNOWN;
	}

	//if (hand->accel[X])
	return GESTURE_UNKNOWN;
}
