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

extern struct Gesture * GESTURES;   // points to the list of gestures (freed on exit)
extern int NUM_GESTURES;            // the number of gestures in the list

static int command_run(char * calibration_file, char * gestures_file);
static int command_sample(void);
static int command_calibrate(char * calibration_file);
static int command_upload(char * calibration_file);
static int command_download(char * calibration_file);
static int command_mode(char * mode);
static int command_record(char * gesture_file);

char * command_string(enum Command command)
{
	switch (command)
	{
		case COMMAND_RUN:         return "run";
		case COMMAND_SAMPLE:      return "sample";
		case COMMAND_CALIBRATE:   return "calibrate";
		case COMMAND_UPLOAD:      return "upload";
		case COMMAND_DOWNLOAD:    return "download";
		case COMMAND_MODE:        return "mode";
		case COMMAND_RECORD:      return "record";
		case COMMAND_UNKNOWN:     return "UNKNOWN";
		default:                  return "INVALID";
	}
}

static int command_run(char * calibration_file, char * gestures_file)
{
	if (calibration_file == NULL)
	{
		log(LOG_ERROR, "No calibration file provided\n");
		return ERROR;
	}

	if (gestures_file == NULL)
	{
		log(LOG_ERROR, "No gestures file provided\n");
		return ERROR;
	}

	int rc = ERROR;
	GESTURES = NULL;
	NUM_GESTURES = 0;

	if (gesture_import(gestures_file, &GESTURES, &NUM_GESTURES) == ERROR)
	{
		log(LOG_ERROR, "Failed to import gestures from '%s'\n", gestures_file);
		goto EXIT;
	}

	if (NUM_GESTURES != NUM_ACTIONS)
	{
		log(LOG_ERROR, "Incorrect number of gestures imported from '%s': Expected %d but imported %d\n",
		                gestures_file, NUM_ACTIONS, NUM_GESTURES);
	}

	for (enum Action expected = 0; expected < NUM_ACTIONS; expected++)
	{
		enum Action actual = GESTURES[expected].action;

		if (actual != expected)
		{
			log(LOG_ERROR, "Incorrect action for gesture #%d: Expected '%s' but parsed '%s'\n",
			                expected, action_string(expected), action_string(actual));
			goto EXIT;
		}
	}

	log(LOG_SUCCESS, "Imported %d gestures from '%s'\n", NUM_GESTURES, gestures_file);

	for (int i = 0; i < NUM_GESTURES; i++)
	{
		gesture_print(&GESTURES[i]);
	}

	if (command_upload(calibration_file) == ERROR)
	{
		log(LOG_ERROR, "Upload failed\n");
		goto EXIT;
	}

	if (scaled() == ERROR)
	{
		log(LOG_ERROR, "Failed to set device to scaled sampling mode\n");
		goto EXIT;
	}

	struct Hand hand;
	enum Control control = CONTROL_MOUSE;
	bool disabled = true;

	printf("DISABLED\n");

	while (1)
	{
		// READ SENSORS //

		if (sample(&hand) == ERROR)
		{
			log(LOG_ERROR, "Failed to sample sensors\n");
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
					log(LOG_ERROR, "Failed to sample sensors\n");
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
			continue;   // don't proceed with gesture recognition if disabled
		}

		if (gesture_matches(ACTION_CYCLE, GESTURES, NUM_GESTURES, &hand))
		{
			control = (control + 1) % NUM_CONTROLS;   // cycle to next control
			continue;
		}

		if (control_execute(control, GESTURES, NUM_GESTURES, &hand) == ERROR)
		{
			log(LOG_ERROR, "Failed to control %s\n", control_string(control));
			goto EXIT;
		}
	}

	rc = SUCCESS;

EXIT:
	gesture_destroy(GESTURES, NUM_GESTURES);
	GESTURES = NULL;
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
			log(LOG_ERROR, "Failed to sample sensors\n");
			return result;
		}

		latency = ((double)(end - begin) / CLOCKS_PER_SEC) * 1000;   // calculate sampling latency in milliseconds

		printf("\r");

		printf("[" BLUE "%0.3f ms" WHITE "] |", latency);
		printf(RED    " %s" WHITE " : % 5hd |", direction_string(X),   hand.accel[X]);
		printf(RED    " %s" WHITE " : % 5hd |", direction_string(Y),   hand.accel[Y]);
		printf(RED    " %s" WHITE " : % 5hd |", direction_string(Z),   hand.accel[Z]);
		printf(YELLOW " %s" WHITE " : % 5hd |", finger_string(THUMB),  hand.flex[THUMB]);
		printf(YELLOW " %s" WHITE " : % 5hd |", finger_string(INDEX),  hand.flex[INDEX]);
		printf(YELLOW " %s" WHITE " : % 5hd |", finger_string(MIDDLE), hand.flex[MIDDLE]);
		printf(YELLOW " %s" WHITE " : % 5hd |", finger_string(RING),   hand.flex[RING]);
		printf(YELLOW " %s" WHITE " : % 5hd |", finger_string(PINKY),  hand.flex[PINKY]);
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
		log(LOG_ERROR, "No output file provided for calibration data\n");
		return ERROR;
	}

	struct Calibration calibration;

	if (calibration_interactive(&calibration) == ERROR)
	{
		log(LOG_ERROR, "Interactive calibration failed\n");
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_export(calibration_file, &calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to export calibration to '%s'\n", calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_upload(char * calibration_file)
{
	if (calibration_file == NULL)
	{
		log(LOG_ERROR, "No calibration file provided\n");
		return ERROR;
	}

	struct Calibration calibration;

	if (calibration_import(calibration_file, &calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to import calibration from '%s'\n", calibration_file);
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_upload(&calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to upload calibration file '%s'\n", calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_download(char * calibration_file)
{
	struct Calibration calibration;

	if (calibration_download(&calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to download calibration from device\n");
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_file == NULL)
	{
		return SUCCESS;
	}

	if (calibration_export(calibration_file, &calibration) == ERROR)
	{
		log(LOG_ERROR, "Failed to export calibration to '%s'\n", calibration_file);
		return ERROR;
	}

	return SUCCESS;
}

static int command_mode(char * mode)
{
	if (mode == NULL)
	{
		log(LOG_ERROR, "No argument provided for scaling mode\n");
		return ERROR;
	}

	if (strncasecmp(mode, mode_string(RAW), strlen(mode_string(RAW))) == 0)
	{
		if (raw() == ERROR)
		{
			log(LOG_ERROR, "Failed to set device to %s mode\n", mode);
			return ERROR;
		}
	}
	else if (strncasecmp(mode, mode_string(SCALED), strlen(mode_string(SCALED))) == 0)
	{
		if (scaled() == ERROR)
		{
			log(LOG_ERROR, "Failed to set device to %s mode\n", mode);
			return ERROR;
		}
	}
	else
	{
		log(LOG_ERROR, "Unknown scaling mode '%s'\n", mode);
		return ERROR;
	}

	log(LOG_SUCCESS, "Set device to %s mode\n", mode);
	return SUCCESS;
}

static int command_record(char * gesture_file)
{
	struct Gesture gesture;

	if (gesture_record(&gesture) == ERROR)
	{
		log(LOG_ERROR, "Failed to record gesture\n");
		return ERROR;
	}

	gesture_print(&gesture);

	if (gesture_file == NULL)
	{
		return SUCCESS;
	}

	if (gesture_export(gesture_file, &gesture, 1) == ERROR)
	{
		log(LOG_ERROR, "Failed to export gesture to '%s'\n", gesture_file);
	}

	log(LOG_SUCCESS, "Recorded gesture\n");
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
		if (strncmp(subcommand, command_string(command), MAX_COMMAND_SIZE) == 0)
		{
			return command;
		}
	}

	return COMMAND_UNKNOWN;
}

int command_execute(enum Command command, char ** arguments, int count)
{
	if (count < 0)
	{
		log(LOG_ERROR, "Argument count is negative\n");
		return ERROR;
	}

	// Assume no arguments are provided
	char * arg0 = NULL;
	char * arg1 = NULL;

	if (count > 0)
	{
		arg0 = arguments[0];
	}

	if (count > 1)
	{
		arg1 = arguments[1];
	}

	usleep(1000);   // give the serial port some time to initialize

	switch (command)
	{
		case COMMAND_RUN:
			return command_run(arg0, arg1);

		case COMMAND_SAMPLE:
			return command_sample();

		case COMMAND_CALIBRATE:
			return command_calibrate(arg0);

		case COMMAND_UPLOAD:
			return command_upload(arg0);

		case COMMAND_DOWNLOAD:
			return command_download(arg0);

		case COMMAND_MODE:
			return command_mode(arg0);

		case COMMAND_RECORD:
			return command_record(arg0);

		default:
			log(LOG_ERROR, "Invalid command\n");
			return ERROR;
	}
}
