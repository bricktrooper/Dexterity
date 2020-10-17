#include "dexterity.h"

char * DIRECTIONS [NUM_DIRECTIONS] = {
	"X",
	"Y",
	"Z"
};

char * FINGERS [NUM_FINGERS] = {
	"F1",
	"F2",
	"F3",
	"F4",
	"F5"
};

char * DIRECTION_NAMES [NUM_DIRECTIONS] = {
	"ACCEL",
	"ACCEL",
	"ACCEL"
};

char * FINGER_NAMES [NUM_FINGERS] = {
	"THUMB",
	"INDEX",
	"MIDDLE",
	"RING",
	"PINKY"
};

char * MESSAGES [NUM_MESSAGES] = {
	"SAMPLE",
	"CALIBRATE",
	"SETTINGS",
	"RAW",
	"SCALED",
	"ERROR",
	"SUCCESS",
	"UNKNOWN"
};

char * PARAMETERS [NUM_PARAMETERS] = {
	"MIN",
	"MAX",
	"ZERO"
};

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 zero)
{
	if (max == min)  // avoid divide by 0
	{
		return reading;
	}

	return ((range * ((S32)reading - min)) / (max - min)) - zero;
}
