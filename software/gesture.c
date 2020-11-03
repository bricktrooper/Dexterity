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

static char * CONTROLS [NUM_CONTROLS] = {
	"MOUSE",
	"ZOOM",
	"SCROLL",
	"VOLUME"
};


static char * ACTIONS [NUM_ACTIONS] = {
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

static struct Gesture * gestures = NULL;
static int gesture_count = 0;
static int gesture_phase = 0;

static bool gesture_valid(struct Gesture * gesture)
{
	if (gesture == NULL ||
		gesture->action >= NUM_ACTIONS ||
		gesture->action >= ACTION_UNKNOWN ||
		gesture->phases <= 0 ||
		gesture->criteria == NULL)
	{
		return false;
	}

	return true;
}

void gesture_destroy(struct Gesture * gesture)
{
	if (gesture == NULL)
	{
		return;
	}

	free(gesture->criteria);
}

int gesture_import(char * file_name, struct Gesture * gesture)
{
	if (file_name == NULL || gesture == NULL)
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
	char name [MAX_ACTION_NAME_LENGTH + 1];

	if (fscanf(file, "ACTION=%s\n", name) != 1)
	{
		log_print(LOG_ERROR, "%s(): Gesture action was incorrectly parsed\n", __func__, file_name);
		goto EXIT;
	}

	gesture->action = ACTION_UNKNOWN;

	for (enum Action action = 0; action < NUM_ACTIONS; action++)
	{
		if (strncmp(name, ACTIONS[action], MAX_ACTION_NAME_LENGTH) == 0)
		{
			gesture->action = action;
			break;
		}
	}

	if (gesture->action == ACTION_UNKNOWN)
	{
		log_print(LOG_ERROR, "%s(): Parsed an unknown action '%s'\n", __func__, name);
		goto EXIT;
	}

	if (fscanf(file, "PHASES=%d\n", &(gesture->phases)) != 1)
	{
		log_print(LOG_ERROR, "%s(): Phase count was incorrectly parsed\n", __func__);
		goto EXIT;
	}

	if (gesture->phases < 0)
	{
		log_print(LOG_ERROR, "%s(): Parsed an negative phase count '%d'\n", __func__, file_name, gesture->phases);
		goto EXIT;
	}

	gesture->criteria = malloc(gesture->phases * sizeof(struct Criteria));

	if (gesture->criteria == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d phases\n", __func__, gesture->phases);
		goto EXIT;
	}

	int tokens = fscanf(file, "IGNORE X=%hhd Y=%hhd Z=%hhd THUMB=%hhd INDEX=%hhd MIDDLE=%hhd RING=%hhd PINKY=%hhd\n",
	                    (S8 *)&(gesture->ignore.accel[X]),
	                    (S8 *)&(gesture->ignore.accel[Y]),
	                    (S8 *)&(gesture->ignore.accel[Z]),
	                    (S8 *)&(gesture->ignore.flex[THUMB]),
	                    (S8 *)&(gesture->ignore.flex[INDEX]),
	                    (S8 *)&(gesture->ignore.flex[MIDDLE]),
	                    (S8 *)&(gesture->ignore.flex[RING]),
	                    (S8 *)&(gesture->ignore.flex[PINKY]));

	if (tokens != NUM_DIRECTIONS + NUM_FINGERS)
	{
		log_print(LOG_ERROR, "%s(): Sensor ignores were incorrectly parsed\n", __func__, file_name);
		goto EXIT;
	}

	for (int i = 0; i < gesture->phases; i++)
	{
		int phase = 0;
		int tokens = fscanf(file, "%d X=%hd Y=%hd Z=%hd THUMB=%hd INDEX=%hd MIDDLE=%hd RING=%hd PINKY=%hd\n",
		                    &phase,
		                    &(gesture->criteria[i].accel[X]),
		                    &(gesture->criteria[i].accel[Y]),
		                    &(gesture->criteria[i].accel[Z]),
		                    &(gesture->criteria[i].flex[THUMB]),
		                    &(gesture->criteria[i].flex[INDEX]),
		                    &(gesture->criteria[i].flex[MIDDLE]),
		                    &(gesture->criteria[i].flex[RING]),
		                    &(gesture->criteria[i].flex[PINKY]));

		if (tokens != 1 + NUM_DIRECTIONS + NUM_FINGERS)
		{
			log_print(LOG_ERROR, "%s(): Sensor criteria were incorrectly parsed\n", __func__, file_name);
			goto EXIT;
		}

		if (phase != i)
		{
			log_print(LOG_ERROR, "%s(): Incorrect phase label: Expected '%d' but parsed '%d'\n", __func__, i, phase);
			goto EXIT;
		}
	}

	log_print(LOG_SUCCESS, "%s(): Imported gesture from file '%s'\n", __func__, file_name);
	rc = SUCCESS;

EXIT:
	fclose(file);
	return rc;
}

int gesture_export(char * file_name, struct Gesture * gesture)
{
	if (file_name == NULL || gesture == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!gesture_valid(gesture))
	{
		log_print(LOG_ERROR, "%s(): Invalid gesture\n", __func__, file_name);
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	fprintf(file, "ACTION=%s\n", ACTIONS[gesture->action]);
	fprintf(file, "PHASES=%d\n", gesture->phases);

	fprintf(file, "IGNORE");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		fprintf(file, " %s=%hhd", DIRECTIONS[direction], (S8)gesture->ignore.accel[direction]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		fprintf(file, " %s=%hhd", FINGERS[finger], (S8)gesture->ignore.flex[finger]);
	}

	fprintf(file, "\n");

	for (int i = 0; i < gesture->phases; i++)
	{
		fprintf(file, "%6d", i);

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			fprintf(file, " %s=%hd", DIRECTIONS[direction], gesture->criteria[i].accel[direction]);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			fprintf(file, " %s=%hd", FINGERS[finger], gesture->criteria[i].flex[finger]);
		}

		fprintf(file, "\n");
	}

	log_print(LOG_SUCCESS, "%s(): Exported gesture to file '%s'\n", __func__, file_name);
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

//bool gesture_compare(enum Action action, struct Hand * hand)
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

//enum Action action_identify(struct Hand * hand)
//{
//	if (hand == NULL)
//	{
//		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
//		return GESTURE_UNKNOWN;
//	}

//	//if (hand->accel[X])
//	return GESTURE_UNKNOWN;
//}

int gesture_print(struct Gesture * gesture)
{
	if (!gesture_valid(gesture))
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	printf("=============================================================================\n");
	printf("ACTION: %s\n", ACTIONS[gesture->action]);
	printf("PHASES: %d\n", gesture->phases);
	printf("IGNORE:");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf(" %s=%hhd", DIRECTIONS[direction], (S8)gesture->ignore.accel[direction]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf(" %s=%hhd", FINGERS[finger], (S8)gesture->ignore.flex[finger]);
	}

	printf("\n");
	printf("-----------------------------------------------------------------------------\n");

	for (int i = 0; i < gesture->phases; i++)
	{
		printf("%6d:", i);

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			printf(" %s=%hd", DIRECTIONS[direction], gesture->criteria[i].accel[direction]);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			printf(" %s=%hd", FINGERS[finger], gesture->criteria[i].flex[finger]);
		}

		printf("\n");
	}

	printf("=============================================================================\n");
}
