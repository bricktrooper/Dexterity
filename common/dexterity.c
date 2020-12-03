#include "dexterity.h"

char * direction_string(enum Direction direction)
{
	switch (direction)
	{
		case X:    return "X";
		case Y:    return "Y";
		case Z:    return "Z";
		default:   return "INVALID";
	}
}

char * finger_string(enum Finger finger)
{
	switch (finger)
	{
		case THUMB:    return "THUMB";
		case INDEX:    return "INDEX";
		case MIDDLE:   return "MIDDLE";
		case RING:     return "RING";
		case PINKY:    return "PINKY";
		default:       return "INVALID";
	}
}

char * message_string(enum Message message)
{
	switch (message)
	{
		case MESSAGE_SAMPLE:     return "SAMPLE";
		case MESSAGE_UPLOAD:     return "UPLOAD";
		case MESSAGE_DOWNLOAD:   return "DOWNLOAD";
		case MESSAGE_RAW:        return "RAW";
		case MESSAGE_SCALED:     return "SCALED";
		case MESSAGE_ERROR:      return "ERROR";
		case MESSAGE_SUCCESS:    return "SUCCESS";
		case MESSAGE_UNKNOWN:    return "UNKNOWN";
		default:                 return "INVALID";
	}
}

char * parameter_string(enum Parameter parameter)
{
	switch (parameter)
	{
		case MIN:      return "MIN";
		case MAX:      return "MAX";
		case CENTRE:   return "CENTRE";
		default:       return "INVALID";
	}
}

char * mode_string(enum Mode mode)
{
	switch (mode)
	{
		case RAW:      return "RAW";
		case SCALED:   return "SCALED";
		default:       return "INVALID";
	}
}

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 centre)
{
	if (max == min)  // avoid divide by 0
	{
		return reading;
	}

	return ((range * ((S32)reading - min)) / (max - min)) - centre;
}
