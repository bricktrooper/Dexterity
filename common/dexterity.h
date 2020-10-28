#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define BUTTON_PRESSED      1
#define BUTTON_RELEASED     0

#define MAX_MESSAGE_SIZE    15

#define ACCEL_DEFAULT_SCALE_RANGE   20
#define FLEX_DEFAULT_SCALE_RANGE    10

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
	Z,   // Z-Axis acceleration

	NUM_DIRECTIONS
};

enum Finger
{
	F1,   // Thumb
	F2,   // Index
	F3,   // Middle
	F4,   // Ring
	F5,   // Pinky

	NUM_FINGERS
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
	MESSAGE_UNKNOWN,     // message is not found in MESSAGES list

	NUM_MESSAGES
};

enum Parameter
{
	ANALOGUE_MIN,    // Maximum raw analogue value for scaling
	ANALOGUE_MAX,    // Minimum raw analogue value for scaling
	ANALOGUE_ZERO,   // Centre scaled analogue value for adjustment

	NUM_PARAMETERS
};

struct Hand
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
	S8 button;
	S8 led;
};

struct Parameters
{
	S16 min;
	S16 max;
	S16 zero;
};

struct Accel
{
	S16 range;
	struct Parameters params [NUM_DIRECTIONS];
};

struct Flex
{
	S16 range;
	struct Parameters params [NUM_FINGERS];
};

struct Calibration
{
	struct Accel accel;
	struct Flex flex;
};

extern char * DIRECTIONS [NUM_DIRECTIONS];
extern char * FINGERS [NUM_FINGERS];
extern char * DIRECTION_NAMES [NUM_DIRECTIONS];
extern char * FINGER_NAMES [NUM_FINGERS];
extern char * MESSAGES [NUM_MESSAGES];
extern char * PARAMETERS [NUM_PARAMETERS];

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 zero);

#endif /* DEXTERITY_H */
