#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define BUTTON_PRESSED     1
#define BUTTON_RELEASED    0

#define NUM_DIRECTIONS     3
#define NUM_FINGERS        5
#define NUM_MESSAGES       8
#define NUM_PARAMETERS     3

#define MAX_MESSAGE_SIZE   15

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;

typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;

enum Direction
{
	X,   // X-Axis acceleration
	Y,   // Y-Axis acceleration
	Z    // Z-Axis acceleration
};

enum Finger
{
	F1,   // Thumb
	F2,   // Index
	F3,   // Middle
	F4,   // Ring
	F5    // Pinky
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

enum Parameter
{
	ANALOGUE_MIN,   // Maximum analogue value for sclaing
	ANALOGUE_MAX,   // Minimum analogue value for sclaing
	ANALOGUE_ZERO   // Centre analogue value for scaling
};

struct Hand
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
	S8 button;
	S8 led;
};

struct Analogue
{
	S16 min;
	S16 max;
	S16 zero;
};

struct Calibration
{
	struct Analogue accel [NUM_DIRECTIONS];
	struct Analogue flex [NUM_FINGERS];
};

extern char * DIRECTIONS [];
extern char * FINGERS [];
extern char * MESSAGES [];
extern char * PARAMETERS [];

#endif /* DEXTERITY_H */
