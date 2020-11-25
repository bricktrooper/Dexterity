#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>

#include "dexterity.h"
#include "action.h"

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
	enum Action action;          // the gesture to execute when this binding is activated
	int phases;                  // the number of phases in the gesture
	int state;                   // tracks the current phase of the gesture (only used at runtime)
	float tolerance;             // how much deviation is tolerated in the sensor readings
	struct Ignores ignores;      // which sensors to ignore
	struct Sensors * criteria;   // array of criteria for multiphase gestures
};

struct Gesture * gesture_create(int quantity);
int gesture_destroy(struct Gesture * gestures, int quantity);
bool gesture_valid(struct Gesture * gesture);
int gesture_import(char * file_name, struct Gesture ** gestures, int * quantity);
int gesture_export(char * file_name, struct Gesture * gestures, int quantity);
int gesture_record(struct Gesture * gesture);
float gesture_compare(struct Gesture * gesture, struct Hand * hand);
bool gesture_matches(enum Action action, struct Gesture * gestures, int quantity, struct Hand * hand);
int gesture_reset(struct Gesture * gestures, int quantity);
int gesture_print(struct Gesture * gesture);

#endif /* GESTURE_H */
