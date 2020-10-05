#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define MAX_MESSAGE_SIZE   15

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;

typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;

enum Direction
{
	X,
	Y,
	Z
};

enum Finger
{
	F1,
	F2,
	F3,
	F4,
	F5
};

enum Message
{
	MESSAGE_SAMPLE,      // read and send all sensor data
	MESSAGE_CALIBRATE,   // set the device calibration
	MESSAGE_SETTINGS,    // get the device calibration
	MESSAGE_RAW,         // return raw sampled data
	MESSAGE_SCALED,      // return scaled sample data based on calibration
	MESSAGE_ERROR,       // notify the host of an error
	MESSAGE_SUCCESS,     // notify the host of a successful operation
	MESSAGE_UNKNOWN      // message is not found in MESSAGES list
};

extern char * MESSAGES [];
extern int NUM_MESSAGES;

struct Hand
{
	S16 accel [3];
	S16 flex [5];
};

struct Calibration
{
	S16 max;
	S16 min;
	S16 zero;
};


#endif /* DEXTERITY_H */
