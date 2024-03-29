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

Gesture * gesture_create(int quantity)
{
	Gesture * gestures = malloc(quantity * sizeof(Gesture));

	if (gestures == NULL)
	{
		log(LOG_ERROR, "Failed to allocate memory for %d gestures\n", quantity);
		return NULL;
	}

	for (int i = 0; i < quantity; i++)
	{
		gestures[i].action = ACTION_UNKNOWN;
		gestures[i].phases = -1;
		gestures[i].state = 0;
		gestures[i].tolerance = DEFAULT_TOLERANCE;
		memset(&gestures[i].ignores, 0, sizeof(Ignores));
		gestures[i].criteria = NULL;
	}

	return gestures;
}

int gesture_destroy(Gesture * gestures, int quantity)
{
	if (quantity < 0)
	{
		log(LOG_ERROR, "Invalid arguments\n");
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

bool gesture_valid(Gesture * gesture)
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

int gesture_import(char * file_name, Gesture ** gestures, int * quantity)
{
	if (file_name == NULL || gestures == NULL || quantity == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	FILE * file = fopen(file_name, "r");

	if (file == NULL)
	{
		log(LOG_ERROR, "Failed to open file '%s': %s (%d)\n", file_name, strerror(errno), errno);
		return ERROR;
	}

	int result = ERROR;
	Gesture * elements = NULL;
	int count = 0;

	if (fscanf(file, "QUANTITY=%d\n\n", &count) != 1)
	{
		log(LOG_ERROR, "Gesture quantity was incorrectly parsed\n");
		goto EXIT;
	}

	if (count < 1)
	{
		log(LOG_ERROR, "Parsed invalid gesture quantity '%d'\n", count);
		goto EXIT;
	}

	elements = gesture_create(count);

	if (elements == NULL)
	{
		log(LOG_ERROR, "Failed to allocate memory for %d gestures\n", count);
		goto EXIT;
	}

	for (int i = 0; i < count; i++)
	{
		Gesture * gesture = (Gesture *)&elements[i];
		char name [MAX_ACTION_NAME_LENGTH + 1];

		if (fscanf(file, "ACTION=%s\n", name) != 1)
		{
			log(LOG_ERROR, "Gesture action was incorrectly parsed for gesture #%d\n", i + 1);
			goto EXIT;
		}

		gesture->action = ACTION_UNKNOWN;

		for (Action action = 0; action < NUM_ACTIONS; action++)
		{
			char const * string = action_string(action);
			if (strncmp(name, string, strlen(string)) == 0)
			{
				gesture->action = action;
				break;
			}
		}

		if (gesture->action == ACTION_UNKNOWN)
		{
			log(LOG_ERROR, "Parsed an invalid action '%s'\n", name);
			goto EXIT;
		}

		if (fscanf(file, "PHASES=%d\n", &gesture->phases) != 1)
		{
			log(LOG_ERROR, "Phase count was incorrectly parsed for gesture #%d\n", i + 1);
			goto EXIT;
		}

		if (gesture->phases < 0)
		{
			log(LOG_ERROR, "Parsed an negative phase count '%d'\n", gesture->phases);
			goto EXIT;
		}

		if (fscanf(file, "TOLERANCE=%f\n", &gesture->tolerance) != 1)
		{
			log(LOG_ERROR, "Tolerance was incorrectly parsed for gesture #%d\n", i + 1);
			goto EXIT;
		}

		if (gesture->tolerance < 0)
		{
			log(LOG_ERROR, "Parsed an negative tolerance '%f'\n", gesture->tolerance);
			goto EXIT;
		}

		gesture->criteria = malloc(gesture->phases * sizeof(Sensors));

		if (gesture->criteria == NULL)
		{
			log(LOG_ERROR, "Failed to allocate memory for %d phases\n", gesture->phases);
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
			log(LOG_ERROR, "Sensor ignores were incorrectly parsed for gesture #%d\n", i + 1);
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
				log(LOG_ERROR, "Sensor criteria were incorrectly parsed for gesture #%d\n", i + 1);
				goto EXIT;
			}

			if (phase_label != phase)
			{
				log(LOG_ERROR, "Incorrect phase label for gesture #%d: Expected '%d' but parsed '%d'\n", i + 1, phase, phase_label);
				goto EXIT;
			}
		}

		if (fscanf(file, "\n") < 0)
		{
			log(LOG_ERROR, "Failed to parse break between gestures\n");
			goto EXIT;
		}
	}

	log(LOG_SUCCESS, "Imported %d gestures from file '%s'\n", count, file_name);
	*gestures = elements;
	*quantity = count;
	result = SUCCESS;

EXIT:
	if (result == ERROR)
	{
		gesture_destroy(elements, count);
	}

	fclose(file);
	return result;
}

int gesture_export(char * file_name, Gesture * gestures, int quantity)
{
	if (file_name == NULL || gestures == NULL || quantity < 1)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	FILE * file = fopen(file_name, "w");

	if (file == NULL)
	{
		log(LOG_ERROR, "Failed to open file '%s': %s (%d)\n", file_name, strerror(errno), errno);
		return ERROR;
	}

	fprintf(file, "QUANTITY=%d\n\n", quantity);
	Gesture * gesture = NULL;

	for (int i = 0; i < quantity; i++)
	{
		gesture = (Gesture *)&gestures[i];

		if (!gesture_valid(gesture))
		{
			log(LOG_ERROR, "Invalid gesture\n");
			return ERROR;
		}

		fprintf(file, "ACTION=%s\n", action_string(gesture->action));
		fprintf(file, "PHASES=%d\n", gesture->phases);
		fprintf(file, "TOLERANCE=%f\n", gesture->tolerance);

		fprintf(file, "IGNORE");

		for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			fprintf(file, " %s=%hhd", direction_string(direction), (S8)gesture->ignores.accel[direction]);
		}

		for (Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			fprintf(file, " %s=%hhd", finger_string(finger), (S8)gesture->ignores.flex[finger]);
		}

		fprintf(file, "\n");

		for (int phase = 0; phase < gesture->phases; phase++)
		{
			fprintf(file, "%6d", phase);

			for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
			{
				fprintf(file, " %s=%hd", direction_string(direction), gesture->criteria[phase].accel[direction]);
			}

			for (Finger finger = 0; finger < NUM_FINGERS; finger++)
			{
				fprintf(file, " %s=%hd", finger_string(finger), gesture->criteria[phase].flex[finger]);
			}

			fprintf(file, "\n");
		}

		fprintf(file, "\n");
	}

	log(LOG_SUCCESS, "Exported %d gestures to file '%s'\n", quantity, file_name);
	fclose(file);
	return SUCCESS;
}

int gesture_record(Gesture * gesture)
{
	if (gesture == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (scaled() == ERROR)
	{
		log(LOG_ERROR, "Failed to set device to scaled sampling mode\n");
		return ERROR;
	}

	// The purpose of this function is just to record an N-phase gesture.
	// The gesture action and ignores should be manually set in the file.

	gesture->action = ACTION_IDLE;
	gesture->tolerance = DEFAULT_TOLERANCE;
	memset(&gesture->ignores, 0, sizeof(Ignores));

	printf("Enter the number of phases: ");

	if (scanf("%d", &gesture->phases) < 1)
	{
		log(LOG_ERROR, "Failed to input phase count\n");
		return ERROR;
	}

	printf("%d\n", gesture->phases);

	if (gesture->phases < 1)
	{
		log(LOG_ERROR, "The phase count cannot be less than 1\n");
		return ERROR;
	}

	gesture->criteria = malloc(gesture->phases * sizeof(Sensors));

	if (gesture->criteria == NULL)
	{
		log(LOG_ERROR, "Failed to allocate memory for %d phases\n", gesture->phases);
		return ERROR;
	}

	printf("Press the button to confirm a measurement\n\n");

	Hand hand;

	for (int phase = 0; phase < gesture->phases; phase++)
	{
		// press button to calibrate
		do
		{
			if (sample(&hand) == ERROR)
			{
				log(LOG_ERROR, "Sample failed while recording phase #%d\n", phase);
				return ERROR;
			}

			printf("\r");
			printf("[" BLUE "Phase %d" RESET "] |", phase);
			printf(RED    " %s" RESET " : % 5hd |", direction_string(X),   hand.accel[X]);
			printf(RED    " %s" RESET " : % 5hd |", direction_string(Y),   hand.accel[Y]);
			printf(RED    " %s" RESET " : % 5hd |", direction_string(Z),   hand.accel[Z]);
			printf(YELLOW " %s" RESET " : % 5hd |", finger_string(THUMB),  hand.flex[THUMB]);
			printf(YELLOW " %s" RESET " : % 5hd |", finger_string(INDEX),  hand.flex[INDEX]);
			printf(YELLOW " %s" RESET " : % 5hd |", finger_string(MIDDLE), hand.flex[MIDDLE]);
			printf(YELLOW " %s" RESET " : % 5hd |", finger_string(RING),   hand.flex[RING]);
			printf(YELLOW " %s" RESET " : % 5hd |", finger_string(PINKY),  hand.flex[PINKY]);
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

		printf(" " GREEN "~" RESET "\n");

		// wait for user to release button
		do
		{
			if (sample(&hand) == ERROR)
			{
				log(LOG_ERROR, "Sample failed while waiting for button release after recording phase #%d\n", phase);
				return ERROR;
			}
		}
		while (hand.button == BUTTON_PRESSED);
	}

	log(LOG_SUCCESS, "Recorded %d phase gesture\n", gesture->phases);
	return SUCCESS;
}

float gesture_compare(Gesture * gesture, Hand * hand)
{
	if (gesture == NULL || hand == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	if (!gesture_valid(gesture))
	{
		log(LOG_ERROR, "Invalid gesture\n");
		return ERROR;
	}

	Ignores * ignores = (Ignores *)&gesture->ignores;
	Sensors * criteria = (Sensors *)&gesture->criteria[gesture->state];
	int total = 0;
	int sum = 0;

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		if (!ignores->accel[direction])
		{
			// accumulate deviations
			sum += abs(criteria->accel[direction] - hand->accel[direction]);
			total++;
		}
	}

	for (Finger finger = 0; finger < NUM_FINGERS; finger++)
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
		log(LOG_WARNING, "All sensor readings are ignored\n");
		return 0.0;   // "exact" match
	}

	float deviation = (float)sum / total;
	log(LOG_DEBUG, "Average gesture deviation: %f\n", deviation);
	return deviation;
}

bool gesture_matches(Action action, Gesture * gestures, int quantity, Hand * hand)
{
	if (action >= NUM_ACTIONS || gestures == NULL || quantity < 0 || hand == NULL)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return false;
	}

	if (gesture_compare(&gestures[action], hand) <= gestures[action].tolerance)
	{
		(gestures[action].state)++;   // increment the current phase

		if (gestures[action].state == gestures[action].phases)   // check if the last phase has been reached
		{
			gesture_reset(gestures, quantity);   // reset all gestures states once a gesture is recongized
			log(LOG_DEBUG, "Gesture matches '%s'\n", action_string(action));
			return true;
		}
	}

	return false;
}

int gesture_reset(Gesture * gestures, int quantity)
{
	if (gestures == NULL || quantity < 0)
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	for (int i = 0; i < quantity; i++)
	{
		gestures[i].state = 0;   // reset phase state for all gestures
	}

	log(LOG_DEBUG, "Reset all gesture states to phase 0\n");
	return SUCCESS;
}

int gesture_print(Gesture * gesture)
{
	if (!gesture_valid(gesture))
	{
		log(LOG_ERROR, "Invalid arguments\n");
		return ERROR;
	}

	printf("=============================================================================\n");
	printf("ACTION: %s\n", action_string(gesture->action));
	printf("PHASES: %d\n", gesture->phases);
	printf("TOLERANCE: %f\n", gesture->tolerance);
	printf("IGNORE:");

	for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
	{
		printf(" %s=%hhd", direction_string(direction), (S8)gesture->ignores.accel[direction]);
	}

	for (Finger finger = 0; finger < NUM_FINGERS; finger++)
	{
		printf(" %s=%hhd", finger_string(finger), (S8)gesture->ignores.flex[finger]);
	}

	printf("\n");
	printf("-----------------------------------------------------------------------------\n");

	for (int phase = 0; phase < gesture->phases; phase++)
	{
		printf("%6d:", phase);

		for (Direction direction = 0; direction < NUM_DIRECTIONS; direction++)
		{
			printf(" %s=%hd", direction_string(direction), gesture->criteria[phase].accel[direction]);
		}

		for (Finger finger = 0; finger < NUM_FINGERS; finger++)
		{
			printf(" %s=%hd", finger_string(finger), gesture->criteria[phase].flex[finger]);
		}

		printf("\n");
	}

	printf("=============================================================================\n");
	return SUCCESS;
}
