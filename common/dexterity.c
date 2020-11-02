#include "dexterity.h"

char * DIRECTIONS [NUM_DIRECTIONS] = {
	"X",
	"Y",
	"Z"
};

char * FINGERS [NUM_FINGERS] = {
	"THUMB",
	"INDEX",
	"MIDDLE",
	"RING",
	"PINKY"
};

char * MESSAGES [NUM_MESSAGES] = {
	"SAMPLE",
	"UPLOAD",
	"DOWNLOAD",
	"RAW",
	"SCALED",
	"ERROR",
	"SUCCESS",
	"UNKNOWN"
};

char * PARAMETERS [NUM_PARAMETERS] = {
	"MIN",
	"MAX",
	"CENTRE"
};

char * MODES [NUM_MODES] = {
	"RAW",
	"SCALED",
};

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 centre)
{
	if (max == min)  // avoid divide by 0
	{
		return reading;
	}

	return ((range * ((S32)reading - min)) / (max - min)) - centre;
}
