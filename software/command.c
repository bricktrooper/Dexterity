#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "serial.h"
#include "calibration.h"
#include "colours.h"
#include "gesture.h"
#include "control.h"

#include "command.h"

#define MAX_COMMAND_SIZE      15
#define MODE_COMMAND_RAW      "raw"
#define MODE_COMMAND_SCALED   "scaled"

static char * PROGRAM = "";

static int command_run(char * calibration_file, char * gestures_file);
static int command_sample(void);
static int command_calibrate(char * calibration_file);
static int command_upload(char * calibration_file);
static int command_download(char * calibration_file);
static int command_mode(char * mode);
static int command_record(char * gesture_file);

char * COMMANDS [NUM_COMMANDS] = {
	"run",
	"sample",
	"calibrate",
	"upload",
	"download",
	"mode",
	"record",
};

static int command_run(char * calibration_file, char * gestures_file)
{
	if (calibration_file == NULL)
	{
		log_print(LOG_ERROR, "%s: No calibration file provided\n", PROGRAM);
		return ERROR;
	}

	if (gestures_file == NULL)
	{
		log_print(LOG_ERROR, "%s: No gestures file provided\n", PROGRAM);
		return ERROR;
	}

	int rc = ERROR;
	struct Gesture * gestures = NULL;
	int num_gestures = 0;

	if (gesture_import(gestures_file, &gestures, &num_gestures) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to import gestures from '%s'\n", PROGRAM, gestures_file);
		goto EXIT;
	}

	//if (num_gestures != NUM_ACTIONS)
	//{
	//  you also need to add the tolerance into the file and struct
	//	log_print(LOG_ERROR, "%s: Incorrect number of gestures imported from '%s': Expected %d but imported %d\n",
	//	                     PROGRAM, file_name, NUM_ACTIONS, num_gestures);
	//}

	//for (enum Action expected = 0; expected < NUM_ACTIONS; expected++)
	//{
	//	enum Action actual = gestures[expected].action;

	//	if (actual != expected)
	//	{
	//		log_print(LOG_ERROR, "%s: Incorrect action for gesture #%d: Expected '%s' but parsed '%s'\n",
	//		                     PROGRAM, expected, ACTIONS[expected], ACTIONS[actual]);
	//		goto EXIT;
	//	}
	//}

	log_print(LOG_SUCCESS, "%s: Imported %d gestures from '%s'\n", PROGRAM, num_gestures, gestures_file);

	for (int i = 0; i < num_gestures; i++)
	{
		gesture_print(&gestures[i]);
	}

	if (command_upload(calibration_file) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Upload failed\n", PROGRAM);
		goto EXIT;
	}

	if (scaled() == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to set device to scaled sampling mode\n", PROGRAM);
		goto EXIT;
	}

	struct Hand hand;
	enum Control control = CONTROL_MOUSE;
	bool disabled = false;

	while (1)
	{
		// READ SENSORS //

		if (sample(&hand) == ERROR)
		{
			log_print(LOG_ERROR, "%s(): Failed to sample sensors\n", PROGRAM);
			goto EXIT;
		}

		// ENABLE / DISABLE //

		if (hand.button == BUTTON_PRESSED)
		{
			// wait until the button is released
			do
			{
				if (sample(&hand) == ERROR)
				{
					log_print(LOG_ERROR, "%s(): Failed to sample sensors\n", PROGRAM);
					goto EXIT;
				}
			}
			while (hand.button != BUTTON_RELEASED);

			if (disabled)
			{
				printf("ENABLED\n");
				disabled = false;   // bring the device back from a disabled state
			}
			else
			{
				printf("DISABLED\n");
				disabled = true;   // ignore all input from device until re-enabled
			}

			continue;
		}

		if (disabled)
		{
			continue;   // don't run program if disabled
		}

		// We also need to deal with the freeing of the gestures when control C happens

		// you should apply any deadzone before doing any inference
		// apply the deadzones to the entire hand here before doing anything else


		// start with mouse mode.  Run the current mode.  If while in the mode you detetch a switch, exit the function
		// and cycle, then run the new gesture.  gesture execute should return cycle when ?

		if (gesture_matches(ACTION_CYCLE, gestures, &hand))
		{
			control = (control + 1) % NUM_CONTROLS;   // cycle to next control
			continue;
		}

		if (control_execute(control, gestures, &hand) == ERROR)
		{
			log_print(LOG_ERROR, "%s(): Failed to control %s\n", PROGRAM, CONTROLS[control]);
			goto EXIT;
		}
	}

	rc = SUCCESS;

EXIT:
	gesture_destroy(gestures, num_gestures);
	gestures = NULL;
	return rc;
}

static int command_sample(void)
{
	struct Hand hand;
	int result;
	double latency;
	clock_t begin;
	clock_t end;

	while (1)
	{
		begin = clock();
		result = sample(&hand);
		end = clock();

		if (result == ERROR)
		{
			log_print(LOG_ERROR, "%s: Failed to sample sensors\n", PROGRAM);
			return result;
		}

		latency = ((double)(end - begin) / CLOCKS_PER_SEC) * 1000;   // calculate sampling latency in milliseconds

		printf("\r");

		printf("[" BLUE "%0.3f ms" WHITE "] |", latency);
		printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[X],   hand.accel[X]);
		printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[Y],   hand.accel[Y]);
		printf(RED    " %s" WHITE " : % 5hd |", DIRECTIONS[Z],   hand.accel[Z]);
		printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[THUMB],  hand.flex[THUMB]);
		printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[INDEX],  hand.flex[INDEX]);
		printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[MIDDLE], hand.flex[MIDDLE]);
		printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[RING],   hand.flex[RING]);
		printf(YELLOW " %s" WHITE " : % 5hd |", FINGERS[PINKY],  hand.flex[PINKY]);
		printf(GREEN  " %s" WHITE " : % hhd |", "BUTTON",        hand.button);
		printf(GREEN  " %s" WHITE " : % hhd |", "LED",           hand.led);

		fflush(stdout);
	}

	return SUCCESS;
}

static int command_calibrate(char * calibration_file)
{
	if (calibration_file == NULL)
	{
		log_print(LOG_ERROR, "%s: No output file provided for calibration data\n", PROGRAM);
		return ERROR;
	}

	struct Calibration calibration;

	if (calibration_interactive(&calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Interactive calibration failed\n", PROGRAM);
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_export(calibration_file, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_upload(char * calibration_file)
{
	if (calibration_file == NULL)
	{
		log_print(LOG_ERROR, "%s: No calibration file provided\n", PROGRAM);
		return ERROR;
	}

	struct Calibration calibration;

	if (calibration_import(calibration_file, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to import calibration from '%s'\n", PROGRAM, calibration_file);
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_upload(&calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to upload calibration file '%s'\n", PROGRAM, calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_download(char * calibration_file)
{
	struct Calibration calibration;

	if (calibration_download(&calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to download calibration from device\n", PROGRAM);
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_file == NULL)
	{
		return SUCCESS;
	}

	if (calibration_export(calibration_file, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_mode(char * mode)
{
	if (mode == NULL)
	{
		log_print(LOG_ERROR, "%s: No argument provided for scaling mode\n", PROGRAM);
		return ERROR;
	}

	if (strncasecmp(mode, MODES[RAW], strlen(MODES[RAW])) == 0)
	{
		if (raw() == ERROR)
		{
			log_print(LOG_SUCCESS, "%s: Failed to set device to %s mode\n", PROGRAM, mode);
			return ERROR;
		}
	}
	else if (strncasecmp(mode, MODES[SCALED], strlen(MODES[SCALED])) == 0)
	{
		if (scaled() == ERROR)
		{
			log_print(LOG_SUCCESS, "%s: Failed to set device to %s mode\n", PROGRAM, mode);
			return ERROR;
		}
	}
	else
	{
		log_print(LOG_ERROR, "%s: Unknown scaling mode '%s'\n", PROGRAM, mode);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s: Set device to %s mode\n", PROGRAM, mode);
	return SUCCESS;
}

static int command_record(char * gesture_file)
{
	struct Gesture gesture;

	if (gesture_record(&gesture) == ERROR)
	{
		log_print(LOG_SUCCESS, "%s: Failed to record gesture\n", PROGRAM);
		return ERROR;
	}

	gesture_print(&gesture);

	if (gesture_file == NULL)
	{
		return SUCCESS;
	}

	if (gesture_export(gesture_file, &gesture, 1) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to export gesture to '%s'\n", PROGRAM, gesture_file);
	}

	log_print(LOG_SUCCESS, "%s: Recorded gesture\n", PROGRAM);
	return SUCCESS;
}

enum Command command_identify(char * subcommand)
{
	if (subcommand == NULL)
	{
		return COMMAND_RUN;   // default command
	}

	for (enum Command command = 0; command < NUM_COMMANDS; command++)
	{
		if (strncmp(subcommand, COMMANDS[command], MAX_COMMAND_SIZE) == 0)
		{
			return command;
		}
	}

	return COMMAND_UNKNOWN;
}

int command_execute(char * program, enum Command command, char ** arguments, int count)
{
	if (count < 0)
	{
		log_print(LOG_ERROR, "%s: Argument count is negative\n", PROGRAM);
		return ERROR;
	}

	if (program != NULL)
	{
		PROGRAM = program;
	}

	usleep(1000);   // give the serial port some time to initialize

	switch (command)
	{
		case COMMAND_RUN:
			return command_run(arguments[0], arguments[1]);

		case COMMAND_SAMPLE:
			return command_sample();

		case COMMAND_CALIBRATE:
			return command_calibrate(arguments[0]);

		case COMMAND_UPLOAD:
			return command_upload(arguments[0]);

		case COMMAND_DOWNLOAD:
			return command_download(arguments[0]);

		case COMMAND_MODE:
			return command_mode(arguments[0]);

		case COMMAND_RECORD:
			return command_record(arguments[0]);

		default:
			log_print(LOG_ERROR, "%s: Invalid command\n", PROGRAM);
			return ERROR;
	}
}
