#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "dexterity.h"
#include "utils.h"
#include "log.h"
#include "colours.h"

#include "gesture.h"

#define DEFAULT_TOLERANCE   0.1   // how much deviation is tolerated in the confidence (average of the individual deviations)

struct Gesture * gesture_create(int quantity)
{
	struct Gesture * gestures = malloc(quantity * sizeof(struct Gesture));

	if (gestures == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d gestures\n", __func__, quantity);
		return NULL;
	}

	for (int i = 0; i < quantity; i++)
	{
		gestures[i].action = ACTION_UNKNOWN;
		gestures[i].phases = -1;
		gestures[i].state = 0;
		gestures[i].tolerance = DEFAULT_TOLERANCE;
		memset(&gestures[i].ignores, 0, sizeof(struct Ignores));
		gestures[i].criteria = NULL;
	}

	return gestures;
}

int gesture_destroy(struct Gesture * gestures, int quantity)
{
	if (quantity < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (gestures != NULL)
	{
		for (int i = 0; i < quantity; i++)
		{
			free(gestures[i].criteria);
		}
	}

	free(gestures);
	return SUCCESS;
}

bool gesture_valid(struct Gesture * gesture)
{
	if (gesture == NULL ||
		gesture->action >= NUM_ACTIONS ||
		gesture->phases <= 0 ||
		gesture->state < 0 ||
		gesture->tolerance < 0 ||
		gesture->criteria == NULL)
	{
		return false;
	}

	return true;
}

int gesture_import(char * file_name, struct Gesture ** gestures, int * quantity)
{
	if (file_name == NULL || gestures == NULL || quantity == NULL)
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
	struct Gesture * elements = NULL;
	int count = 0;

	if (fscanf(file, "QUANTITY=%d\n\n", &count) != 1)
	{
		log_print(LOG_ERROR, "%s(): Gesture quantity was incorrectly parsed\n", __func__);
		goto EXIT;
	}

	if (count < 1)
	{
		log_print(LOG_ERROR, "%s(): Parsed invalid gesture quantity '%d'\n", __func__, count);
		goto EXIT;
	}

	elements = gesture_create(count);

	if (elements == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d gestures\n", __func__, count);
		goto EXIT;
	}

	for (int i = 0; i < count; i++)
	{
		struct Gesture * gesture = (struct Gesture *)&elements[i];
		char name [MAX_ACTION_NAME_LENGTH + 1];

		if (fscanf(file, "ACTION=%s\n", name) != 1)
		{
			log_print(LOG_ERROR, "%s(): Gesture action was incorrectly parsed for gesture #%d\n", __func__, i + 1);
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
			log_print(LOG_ERROR, "%s(): Parsed an invalid action '%s'\n", __func__, name);
			goto EXIT;
		}

		if (fscanf(file, "PHASES=%d\n", &gesture->phases) != 1)
		{
			log_print(LOG_ERROR, "%s(): Phase count was incorrectly parsed for gesture #%d\n", __func__, i + 1);
			goto EXIT;
		}

		if (gesture->phases < 0)
		{
			log_print(LOG_ERROR, "%s(): Parsed an negative phase count '%d'\n", __func__, gesture->phases);
			goto EXIT;
		}

		if (fscanf(file, "TOLERANCE=%f\n", &gesture->tolerance) != 1)
		{
			log_print(LOG_ERROR, "%s(): Tolerance was incorrectly parsed for gesture #%d\n", __func__, i + 1);
			goto EXIT;
		}

		if (gesture->tolerance < 0)
		{
			log_print(LOG_ERROR, "%s(): Parsed an negative tolerance '%f'\n", __func__, gesture->tolerance);
			goto EXIT;
		}

		gesture->criteria = malloc(gesture->phases * sizeof(struct Sensors));

		if (gesture->criteria == NULL)
		{
			log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d phases\n", __func__, gesture->phases);
			goto EXIT;
		}

		int tokens = fscanf(file, "IGNORE X=%hhd Y=%hhd Z=%hhd THUMB=%hhd INDEX=%hhd MIDDLE=%hhd RING=%hhd PINKY=%hhd\n",
							(S8 *)&gesture->ignores.accel[X],
							(S8 *)&gesture->ignores.accel[Y],
							(S8 *)&gesture->ignores.accel[Z],
							(S8 *)&gesture->ignores.flex[THUMB],
							(S8 *)&gesture->ignores.flex[INDEX],
							(S8 *)&gesture->ignores.flex[MIDDLE],
							(S8 *)&gesture->ignores.flex[RING],
							(S8 *)&gesture->ignores.flex[PINKY]);

		if (tokens != NUM_DIRECTIONS + NUM_FINGERS)
		{
			log_print(LOG_ERROR, "%s(): Sensor ignores were incorrectly parsed for gesture #%d\n", __func__, i + 1);
			goto EXIT;
		}

		for (int phase = 0; phase < gesture->phases; phase++)
		{
			int phase_label = 0;
			int tokens = fscanf(file, "%d X=%hd Y=%hd Z=%hd THUMB=%hd INDEX=%hd MIDDLE=%hd RING=%hd PINKY=%hd\n",
								&phase_label,
								&gesture->criteria[phase].accel[X],
								&gesture->criteria[phase].accel[Y],
								&gesture->criteria[phase].accel[Z],
								&gesture->criteria[phase].flex[THUMB],
								&gesture->criteria[phase].flex[INDEX],
								&gesture->criteria[phase].flex[MIDDLE],
								&gesture->criteria[phase].flex[RING],
								&gesture->criteria[phase].flex[PINKY]);

			if (tokens != 1 + NUM_DIRECTIONS + NUM_FINGERS)
			{
				log_print(LOG_ERROR, "%s(): Sensor criteria were incorrectly parsed for gesture #%d\n", __func__, i + 1);
				goto EXIT;
			}

			if (phase_label != phase)
			{
				log_print(LOG_ERROR, "%s(): Incorrect phase label for gesture #%d: Expected '%d' but parsed '%d'\n", __func__, i + 1, phase, phase_label);
				goto EXIT;
			}
		}

		if (fscanf(file, "\n") < 0)
		{
			log_print(LOG_ERROR, "%s(): Failed to parse break between gestures\n", __func__);
			goto EXIT;
		}
	}

	log_print(LOG_SUCCESS, "%s(): Imported %d gestures from file '%s'\n", __func__, count, file_name);
	*gestures = elements;
	*quantity = count;
	rc = SUCCESS;

EXIT:
	if (rc == ERROR)
	{
		gesture_destroy(elements, count);
	}

	fclose(file);
	return rc;
}

int gesture_export(char * file_name, struct Gesture * gestures, int quantity)
{
	if (file_name == NULL || gestures == NULL || quantity < 1)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to open file '%s': %s (%d)\n", __func__, file_name, strerror(errno), errno);
		return ERROR;
	}

	fprintf(file, "QUANTITY=%d\n\n", quantity);
	struct Gesture * gesture = NULL;

	for (int i = 0; i < quantity; i++)
	{
		gesture = (struct Gesture *)&gestures[i];

		if (!gesture_valid(gesture))
		{
			log_print(LOG_ERROR, "%s(): Invalid gesture\n", __func__);
			return ERROR;
		}

		fprintf(file, "ACTION=%s\n", ACTIONS[gesture->action]);
		fprintf(file, "PHASES=%d\n", gesture->phases);
		fprintf(file, "TOLERANCE=%f\n", gesture->tolerance);

		fprintf(file, "IGNORE");

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			fprintf(file, " %s=%hhd", DIRECTIONS[direction], (S8)gesture->ignores.accel[direction]);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			fprintf(file, " %s=%hhd", FINGERS[finger], (S8)gesture->ignores.flex[finger]);
		}

		fprintf(file, "\n");

		for (int phase = 0; phase < gesture->phases; phase++)
		{
			fprintf(file, "%6d", phase);

			for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
			{
				fprintf(file, " %s=%hd", DIRECTIONS[direction], gesture->criteria[phase].accel[direction]);
			}

			for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
			{
				fprintf(file, " %s=%hd", FINGERS[finger], gesture->criteria[phase].flex[finger]);
			}

			fprintf(file, "\n");
		}

		fprintf(file, "\n");
	}

	log_print(LOG_SUCCESS, "%s(): Exported %d gestures to file '%s'\n", __func__, quantity, file_name);
	fclose(file);
	return SUCCESS;
}

int gesture_record(struct Gesture * gesture)
{
	if (gesture == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (scaled() == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to set device to scaled sampling mode\n", __func__);
		return ERROR;
	}

	// The purpose of this function is just to record an N-phase gesture.
	// The gesture action and ignores should be manually set in the file.

	gesture->action = ACTION_IDLE;
	gesture->tolerance = DEFAULT_TOLERANCE;
	memset(&gesture->ignores, 0, sizeof(struct Ignores));

	printf("Enter the number of phases: ");

	if (scanf("%d", &gesture->phases) < 1)
	{
		log_print(LOG_ERROR, "%s(): Failed to input phase count\n", __func__);
		return ERROR;
	}

	printf("%d\n", gesture->phases);

	if (gesture->phases < 1)
	{
		log_print(LOG_ERROR, "%s(): The phase count cannot be less than 1\n", __func__);
		return ERROR;
	}

	gesture->criteria = malloc(gesture->phases * sizeof(struct Sensors));

	if (gesture->criteria == NULL)
	{
		log_print(LOG_ERROR, "%s(): Failed to allocate memory for %d phases\n", __func__, gesture->phases);
		return ERROR;
	}

	printf("Press the button to confirm a measurement\n\n");

	struct Hand hand;

	for (int phase = 0; phase < gesture->phases; phase++)
	{
		// press button to calibrate
		do
		{
			if (sample(&hand) == ERROR)
			{
				log_print(LOG_ERROR, "%s(): Sample failed while recording phase #%d\n", __func__, phase);
				return ERROR;
			}

			printf("\r");
			printf("[" BLUE "Phase %d" WHITE "] |", phase);
			printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[X],   hand.accel[X]);
			printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[Y],   hand.accel[Y]);
			printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[Z],   hand.accel[Z]);
			printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[THUMB],  hand.flex[THUMB]);
			printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[INDEX],  hand.flex[INDEX]);
			printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[MIDDLE], hand.flex[MIDDLE]);
			printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[RING],   hand.flex[RING]);
			printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[PINKY],  hand.flex[PINKY]);
			fflush(stdout);
		}
		while (hand.button == BUTTON_RELEASED);

		// save phase readings to criteria array
		gesture->criteria[phase].accel[X] = hand.accel[X];
		gesture->criteria[phase].accel[Y] = hand.accel[Y];
		gesture->criteria[phase].accel[Z] = hand.accel[Z];
		gesture->criteria[phase].flex[THUMB] = hand.flex[THUMB];
		gesture->criteria[phase].flex[INDEX] = hand.flex[INDEX];
		gesture->criteria[phase].flex[MIDDLE] = hand.flex[MIDDLE];
		gesture->criteria[phase].flex[RING] = hand.flex[RING];
		gesture->criteria[phase].flex[PINKY] = hand.flex[PINKY];

		printf(" " GREEN "~" WHITE "\n");

		// wait for user to release button
		do
		{
			if (sample(&hand) == ERROR)
			{
				log_print(LOG_ERROR, "%s(): Sample failed while waiting for button release after recording phase #%d\n", __func__, phase);
				return ERROR;
			}
		}
		while (hand.button == BUTTON_PRESSED);
	}

	log_print(LOG_SUCCESS, "%s(): Recorded %d phase gesture\n", __func__, gesture->phases);
	return SUCCESS;
}

float gesture_compare(struct Gesture * gesture, struct Hand * hand)
{
	if (gesture == NULL || hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	if (!gesture_valid(gesture))
	{
		log_print(LOG_ERROR, "%s(): Invalid gesture\n", __func__);
		return ERROR;
	}

	struct Ignores * ignores = (struct Ignores *)&gesture->ignores;
	struct Sensors * criteria = (struct Sensors *)&gesture->criteria[gesture->state];
	int total = 0;
	int sum = 0;

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		if (!ignores->accel[direction])
		{
			// accumulate deviations
			sum += abs(criteria->accel[direction] - hand->accel[direction]);
			total++;
		}
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		if (!ignores->flex[finger])
		{
			// accumulate deviations
			sum += abs(criteria->flex[finger] - hand->flex[finger]);
			total++;
		}
	}

	if (total == 0)   // prevent divide by zero when averaging
	{
		log_print(LOG_WARNING, "%s(): All sensor readings are ignored\n", __func__);
		return 0.0;   // "exact" match
	}

	return (float)sum / total;
}

bool gesture_matches(enum Action action, struct Gesture * gestures, int quantity, struct Hand * hand)
{
	if (action >= NUM_ACTIONS || gestures == NULL || quantity < 0 || hand == NULL)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return false;
	}

	if (gesture_compare(&gestures[action], hand) <= gestures[action].tolerance)
	{
		(gestures[action].state)++;   // increment the current phase

		if (gestures[action].state == gestures[action].phases)   // check if the last phase has been reached
		{
			gesture_reset(gestures, quantity);   // reset all gestures states once a gesture is recongized
			return true;
		}
	}

	return false;
}

int gesture_reset(struct Gesture * gestures, int quantity)
{
	if (gestures == NULL || quantity < 0)
	{
		log_print(LOG_ERROR, "%s(): Invalid arguments\n", __func__);
		return ERROR;
	}

	for (int i = 0; i < quantity; i++)
	{
		gestures[i].state = 0;   // reset phase state for all gestures
	}

	return SUCCESS;
}

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
	printf("TOLERANCE: %f\n", gesture->tolerance);
	printf("IGNORE:");

	for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf(" %s=%hhd", DIRECTIONS[direction], (S8)gesture->ignores.accel[direction]);
	}

	for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf(" %s=%hhd", FINGERS[finger], (S8)gesture->ignores.flex[finger]);
	}

	printf("\n");
	printf("-----------------------------------------------------------------------------\n");

	for (int phase = 0; phase < gesture->phases; phase++)
	{
		printf("%6d:", phase);

		for (enum Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			printf(" %s=%hd", DIRECTIONS[direction], gesture->criteria[phase].accel[direction]);
		}

		for (enum Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			printf(" %s=%hd", FINGERS[finger], gesture->criteria[phase].flex[finger]);
		}

		printf("\n");
	}

	printf("=============================================================================\n");
	return SUCCESS;
}
