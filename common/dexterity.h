#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING   -2
#define SUCCESS    0

#define BUTTON_PRESSED    1
#define BUTTON_RELEASED   0

#define MAX_MESSAGE_SIZE         10

#define MAX_SENSOR_NAME_LENGTH    6
#define MAX_CONTROL_NAME_LENGTH   7
#define MAX_ACTION_NAME_LENGTH    18
#define ENABLE_DISABLE_PADDING    (MAX_CONTROL_NAME_LENGTH + MAX_ACTION_NAME_LENGTH + 1)

#define ACCEL_DEFAULT_ZERO    100
#define FLEX_DEFAULT_ZERO     0

#define ACCEL_DEFAULT_RANGE   100
#define FLEX_DEFAULT_RANGE    20

#define CURSOR_DEADZONE     3
#define VOLUME_DEADZONE     10

#define SCROLL_ACTIVATION   0
#define SWIPE_ACTIVATION    0
#define VOLUME_ACTIVATION   30
#define MUSIC_ACTIVATION    20

#define DOUBLE_CLICK_DELAY_US   15000

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;

typedef int8_t S8;
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

typedef enum Direction Direction;
typedef enum Finger Finger;
typedef enum Message Message;
typedef enum Parameter Parameter;
typedef enum Mode Mode;

typedef struct Hand Hand;
typedef struct AccelCalibration AccelCalibration;
typedef struct FlexCalibration FlexCalibration;
typedef struct Calibration Calibration;

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
	AccelCalibration accel;
	FlexCalibration flex;
};

char * direction_string(Direction direction);
char * finger_string(Finger finger);
char * message_string(Message message);
char * parameter_string(Parameter parameter);
char * mode_string(Mode mode);

S16 scale(S16 reading, S16 range, S16 min, S16 max, S16 centre);

#endif /* DEXTERITY_H */
