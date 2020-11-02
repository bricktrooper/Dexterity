#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define BUTTON_PRESSED    1
#define BUTTON_RELEASED   0

#define MAX_MESSAGE_SIZE      15
#define MAX_LABEL_LENGTH      6

#define ACCEL_DEFAULT_ZERO    100
#define FLEX_DEFAULT_ZERO     0

#define ACCEL_DEFAULT_RANGE   100
#define FLEX_DEFAULT_RANGE    20

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
	THUMB,    // 1st flex sensor
	INDEX,    // 2nd flex sensor
	MIDDLE,   // 3rd flex sensor
	RING,     // 4th flex sensor
	PINKY,    // 5th flex sensor

	NUM_FINGERS
};

enum Message
{
	MESSAGE_SAMPLE,      // read and send all sensor data
	MESSAGE_UPLOAD,      // set the device calibration
	MESSAGE_DOWNLOAD,    // get the device calibration
	MESSAGE_RAW,         // send the raw sampled data (no scaling)
	MESSAGE_SCALED,      // scale the sample data based on calibration
	MESSAGE_ERROR,       // notify the host of an error
	MESSAGE_SUCCESS,     // notify the host of a successful operation
	MESSAGE_UNKNOWN,     // message is not found in MESSAGES list

	NUM_MESSAGES
};

enum Parameter
{
	MIN,      // Maximum raw analogue value for scaling
	MAX,      // Minimum raw analogue value for scaling
	CENTRE,   // Centre scaled analogue value for adjustment

	NUM_PARAMETERS
};

enum Mode
{
	RAW,      // use raw adc readings
	SCALED,   // scale adc readings using calibration

	NUM_MODES
};

struct Hand
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
	S8 button;
	S8 led;
};

struct AccelCalibration
{
	S16 range;
	S16 params [NUM_DIRECTIONS][NUM_PARAMETERS];
};

struct FlexCalibration
{
	S16 range;
	S16 params [NUM_FINGERS][NUM_PARAMETERS];
};

struct Calibration
{
	struct AccelCalibration accel;
	struct FlexCalibration flex;
};

extern char * DIRECTIONS [NUM_DIRECTIONS];
extern char * FINGERS [NUM_FINGERS];
extern char * MESSAGES [NUM_MESSAGES];
extern char * PARAMETERS [NUM_PARAMETERS];
extern char * MODES [NUM_MODES];

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 centre);

#endif /* DEXTERITY_H */
