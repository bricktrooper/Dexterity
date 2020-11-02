#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "dexterity.h"
#include "utils.h"
#include "log.h"

#include "gesture.h"

#define TOLERANCE   2.0   // how much deviation is tolerated in the confidence (average of the individual deviations)

char * CONTROLS [NUM_CONTROLS] = {
	"MOUSE",
	"ZOOM",
	"SCROLL",
	"VOLUME"
};


char * GESTURES [NUM_GESTURES] = {
	"MOVE",
	"DRAG",
	"LEFT_CLICK",
	"RIGHT_CLICK",
	"DOUBLE_CLICK",
	"ZOOM_IN",
	"ZOOM_OUT",
	"SCROLL_UP",
	"SCROLL_DOWN",
	"VOLUME_UP",
	"VOLUME_DOWN",
	"CYCLE",
	"DISABLE",
	"ENABLE",
	"UNKNOWN"
};

static bool binding_valid(struct Binding * binding)
{
	if (binding == NULL ||
		binding->gesture >= NUM_GESTURES ||
		binding->gesture >= GESTURE_UNKNOWN ||
		binding->phases <= 0 ||
		binding->criteria == NULL)
	{
		return false;
	}

	return true;
}

void gesture_binding_destroy(struct Binding * binding)
{
	if (binding == NULL)
	{
		return;
	}

	free(binding->criteria);
}

int gesture_import(char * file_name, struct Binding * binding)
{
	if (file_name == NULL || binding == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	FILE * file = fopen(file_name, "r");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	int rc = ERROR;
	char name [MAX_GESTURE_NAME_LENGTH + 1];

	if (fscanf(file, "GESTURE=%s\n", name) != 1)
	{
		log_print(LOG_ERROR, "%s(): Gesture name was incorrectly parsed\n", __func__, file_name);
		goto EXIT;
	}

	binding->gesture = GESTURE_UNKNOWN;

	for (enum Gesture gesture = 0; gesture < NUM_GESTURES; gesture++)
	{
		if (strncmp(name, GESTURES[gesture], MAX_GESTURE_NAME_LENGTH) == 0)
		{
			binding->gesture = gesture;
			break;
		}
	}

	if (binding->gesture == GESTURE_UNKNOWN)
	{
		log_print(LOG_ERROR, "%s(): An unknown gesture was parsed\n", __func__);
		goto EXIT;
	}

	if (fscanf(file, "PHASES=%d\n", &(binding->phases)) != 1)
	{
		log_print(LOG_ERROR, "%s(): Phase count was incorrectly parsed\n", __func__);
		goto EXIT;
	}

	if (binding->phases < 0)
	{
		log_print(LOG_ERROR, "%s(): Phase count cannot be negative\n", __func__, file_name);
		goto EXIT;
	}

	binding->criteria = malloc(binding->phases * sizeof(struct Criteria));

	if (binding->criteria == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d bindings\n", __func__, binding->criteria);
		goto EXIT;
	}

	int tokens = fscanf(file, "IGNORE X=%hhd Y=%hhd Z=%hhd THUMB=%hhd INDEX=%hhd MIDDLE=%hhd RING=%hhd PINKY=%hhd\n",
							(S8 *)&(binding->ignore.accel[X]),
							(S8 *)&(binding->ignore.accel[Y]),
							(S8 *)&(binding->ignore.accel[Z]),
							(S8 *)&(binding->ignore.flex[THUMB]),
							(S8 *)&(binding->ignore.flex[INDEX]),
							(S8 *)&(binding->ignore.flex[MIDDLE]),
							(S8 *)&(binding->ignore.flex[RING]),
							(S8 *)&(binding->ignore.flex[PINKY]));

	if (tokens != NUM_DIRECTIONS + NUM_FINGERS)
	{
		log_print(LOG_ERROR, "%s(): Sensor ignores were incorrectly parsed\n", __func__, file_name);
		goto EXIT;
	}

	for (int phase = 0; phase < binding->phases; phase++)
	{
		int phase_label = 0;
		int tokens = fscanf(file, "%d X=%hd Y=%hd Z=%hd THUMB=%hd INDEX=%hd MIDDLE=%hd RING=%hd PINKY=%hd\n",
							&phase_label,
							&(binding->criteria[phase].accel[X]),
							&(binding->criteria[phase].accel[Y]),
							&(binding->criteria[phase].accel[Z]),
							&(binding->criteria[phase].flex[THUMB]),
							&(binding->criteria[phase].flex[INDEX]),
							&(binding->criteria[phase].flex[MIDDLE]),
							&(binding->criteria[phase].flex[RING]),
							&(binding->criteria[phase].flex[PINKY]));

		if (tokens != 1 + NUM_DIRECTIONS + NUM_FINGERS)
		{
			log_print(LOG_ERROR, "%s(): Sensor criteria were incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}

		if (phase_label != phase)
		{
			log_print(LOG_ERROR, "%s(): Incorrect phase label: Expected '%d' but parsed '%d'\n", __func__, phase, phase_label);
			goto EXIT;
		}
	}

	log_print(LOG_SUCCESS, "%s(): Imported binding from file '%s'\n", __func__, file_name);
	rc = SUCCESS;

EXIT:
	fclose(file);
	return rc;
}

int gesture_export(char * file_name, struct Binding * binding)
{
	if (file_name == NULL || binding == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!binding_valid(binding))
	{
		log_print(LOG_ERROR, "%s(): Invalid binding\n", __func__, file_name);
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	fprintf(file, "GESTURE=%s\n", GESTURES[binding->gesture]);
	fprintf(file, "PHASES=%d\n", binding->phases);

	fprintf(file, "IGNORE ");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		fprintf(file, " %s=%hhd", DIRECTIONS[direction], (S8)binding->ignore.accel[direction]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		fprintf(file, " %s=%hhd", FINGERS[finger], (S8)binding->ignore.flex[finger]);
	}

	fprintf(file, "\n");

	for (int phase = 0; phase < binding->phases; phase++)
	{
		fprintf(file, "%-7d", phase);

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			fprintf(file, " %s=%hd", DIRECTIONS[direction], binding->criteria[phase].accel[direction]);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			fprintf(file, " %s=%hd", FINGERS[finger], binding->criteria[phase].flex[finger]);
		}

		fprintf(file, "\n");
	}

	log_print(LOG_SUCCESS, "%s(): Exported binding to file '%s'\n", __func__, file_name);
	fclose(file);
	return SUCCESS;
}

//int gesture_load_profiles(void)
//{
//	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
//	{
//		profiles[GESTURE_MOVE].ignore.accel[direction] = true;
//		profiles[GESTURE_MOVE].hand.accel[direction] = 0;
//	}

//	// all fingers need to be straight to move cursor
//	// don't care about accel
//	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
//	{
//		profiles[GESTURE_MOVE].ignore.flex[finger] = false;
//		profiles[GESTURE_MOVE].hand.flex[finger] = 0;
//	}

//	return SUCCESS;
//}

//bool gesture_compare(enum Gesture gesture, struct Hand * hand)
//{
//	if (hand == NULL)
//	{
//		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
//		return false;
//	}

//	// hardcoded for hand right now.  Don't consider aceleration
//	int deviation [NUM_FINGERS];

//	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
//	{
//		if (profiles[GESTURE_MOVE].ignore.flex[finger])
//		{
//			deviation[finger] = 0;
//			continue;
//		}

//		deviation[finger] = profiles[GESTURE_MOVE].hand.flex[finger] - hand->flex[finger];
//	}

//	float confidence = TOLERANCE + 1;   // assume it is not a match

//	if (average(deviation, NUM_FINGERS, &confidence) == ERROR)
//	{
//		log_print(LOG_ERROR, "%s(): Failed to calulate average deviation\n", __func__);
//		return false;
//	}

//	printf("%d %d %d %d %d -> %f\n", deviation[THUMB], deviation[INDEX], deviation[MIDDLE], deviation[RING], deviation[PINKY], confidence);

//	if (fabs(confidence) < TOLERANCE)
//	{
//		return true;
//	}

//	return false;
//}

//enum Gesture gesture_identify(struct Hand * hand)
//{
//	if (hand == NULL)
//	{
//		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
//		return GESTURE_UNKNOWN;
//	}

//	//if (hand->accel[X])
//	return GESTURE_UNKNOWN;
//}
