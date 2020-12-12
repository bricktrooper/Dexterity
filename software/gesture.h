#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"
#include "action.h"

typedef struct Ignores Ignores;
typedef struct Sensors Sensors;
typedef struct Gesture Gesture;

struct Ignores
{
	bool accel [NUM_DIRECTIONS];
	bool flex [NUM_FINGERS];
};

struct Sensors
{
	S16 accel [NUM_DIRECTIONS];
	S16 flex [NUM_FINGERS];
};

struct Gesture
{
	Action action;        // the gesture to execute when this binding is activated
	int phases;           // the number of phases in the gesture
	int state;            // tracks the current phase of the gesture (only used at runtime)
	float tolerance;      // how much deviation is tolerated in the sensor readings
	Ignores ignores;      // which sensors to ignore
	Sensors * criteria;   // array of criteria for multiphase gestures
};

Gesture * gesture_create(int quantity);
int gesture_destroy(Gesture * gestures, int quantity);
bool gesture_valid(Gesture * gesture);
int gesture_import(char * file_name, Gesture ** gestures, int * quantity);
int gesture_export(char * file_name, Gesture * gestures, int quantity);
int gesture_record(Gesture * gesture);
float gesture_compare(Gesture * gesture, Hand * hand);
bool gesture_matches(Action action, Gesture * gestures, int quantity, Hand * hand);
int gesture_reset(Gesture * gestures, int quantity);
int gesture_print(Gesture * gesture);

#endif /* GESTURE_H */
