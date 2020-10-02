#ifndef DEXTERITY_H
#define DEXTERITY_H

#include <stdint.h>

#define ERROR     -1
#define WARNING    1
#define SUCCESS    0

#define MESSAGE_BUFFER_SIZE   10
#define MESSAGE_SAMPLE        "SAMPLE"
#define MESSAGE_CALIBRATE     "CALIBRATE"

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;

typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;

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
