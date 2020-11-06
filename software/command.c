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

#include "mouse.h"
#include "keyboard.h"
#include "gesture.h"

#include "command.h"

#define MAX_COMMAND_SIZE      15
#define MODE_COMMAND_RAW      "raw"
#define MODE_COMMAND_SCALED   "scaled"

static char * PROGRAM = "";

char * COMMANDS [NUM_COMMANDS] = {
	"run",
	"sample",
	"calibrate",
	"upload",
	"download",
	"mode",
	"record",
};

static int command_run(char * file_name)
{
	if (file_name == NULL)
	{
		log_print(LOG_ERROR, "%s(): No gesture file was provided\n", __func__);
		return ERROR;
	}

	// TODO: we also need to upload the calibration ad set the scaled mode

	int rc = ERROR;
	struct Gesture * gestures = NULL;
	int num_gestures = 0;

	if (gesture_import(file_name, &gestures, &num_gestures) == ERROR)
	{
		log_print(LOG_ERROR, "%s(): Failed to import gestures from '%s'\n", __func__, file_name);
		goto EXIT;
	}

	//if (num_gestures != NUM_ACTIONS)
	//{
	//  you also need to add the tolerance into the file and struct
	//	log_print(LOG_ERROR, "%s(): Incorrect number of gestures imported from '%s': Expected %d but imported %d\n",
	//	                     __func__, file_name, NUM_ACTIONS, num_gestures);
	//}

	//for (enum Action expected = 0; expected < NUM_ACTIONS; expected++)
	//{
	//	enum Action actual = gestures[expected].action;

	//	if (actual != expected)
	//	{
	//		log_print(LOG_ERROR, "%s(): Incorrect action for gesture #%d: Expected '%s' but parsed '%s'\n",
	//		                     __func__, expected, ACTIONS[expected], ACTIONS[actual]);
	//		goto EXIT;
	//	}
	//}

	log_print(LOG_SUCCESS, "%s(): Imported %d gestures from '%s'\n", __func__, num_gestures, file_name);

	for (int i = 0; i < num_gestures; i++)
	{
		gesture_print(&gestures[i]);
	}

	struct Hand hand;
	enum Control control = 0;
	int phase = 0;
	//bool disabled = false;

	while (1)
	{
		if (sample(&hand) == ERROR)
		{
			log_print(LOG_ERROR, "%s(): Failed to sample sensors\n", __func__);
			goto EXIT;
		}

		// CHECK FOR ENABLE DISABLE HERE

		// you should apply any deadzone before doing any inference
		// apply the deadzones to the entire hand here before doing anything else


		// start with mouse mode.  Run the current mode.  If while in the mode you detetch a switch, exit the function
		// and cycle, then run the new gesture.  gesture execute should return cycle when ?

		//&gestures[ACTION_CYCLE]
		if (gesture_compare(&gestures[0], &hand, phase))
		{
			phase++;   // increment the phase

			if (phase == gestures[0].phases)   // check if the last phase has been reached
			{
				control = (control + 1) % NUM_CONTROLS;   // cycle to next control
				phase = 0;                                // reset phase once a gesture is recongized
			}

			continue;
		}

		switch (control)
		{
			case CONTROL_ZOOM:
				printf("Controlling ZOOM\n");
				break;

			case CONTROL_SCROLL:
				printf("Controlling SCROLL\n");
				break;

			case CONTROL_VOLUME:
				printf("Controlling VOLUME\n");
				break;

			case CONTROL_MOUSE:
			default:
				printf("Controlling MOUSE\n");
				break;
		}

		//int deadzone = 5;
		//int x = -hand.accel[Z];
		//int y = -hand.accel[X];

		//if (abs(x) < deadzone)
		//{
		//	x = 0;
		//}
		//else if (x > 0)
		//{
		//	x -= deadzone;
		//}
		//else if (x < 0)
		//{
		//	x += deadzone;
		//}

		//if (abs(y) < deadzone)
		//{
		//	y = 0;
		//}
		//else if (x > 0)
		//{
		//	y -= deadzone;
		//}
		//else if (x < 0)
		//{
		//	y += deadzone;
		//}

		//mouse_move(x, y);
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

	usleep(1000);   // give the serial port some time to initialize

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

static int command_calibrate(char * file_name)
{
	if (file_name == NULL)
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

	if (calibration_export(file_name, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, file_name);
		return ERROR;
	}

	return SUCCESS;
}

static int command_upload(char * file_name)
{
	if (file_name == NULL)
	{
		log_print(LOG_ERROR, "%s: No calibration file provided\n", PROGRAM);
		return ERROR;
	}

	struct Calibration calibration;

	if (calibration_import(file_name, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to import calibration from '%s'\n", PROGRAM, file_name);
		return ERROR;
	}

	calibration_print(&calibration);

	if (calibration_upload(&calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to upload calibration to device\n", PROGRAM);
		return ERROR;
	}

	return SUCCESS;
}

static int command_download(char * file_name)
{
	struct Calibration calibration;

	if (calibration_download(&calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to download calibration from device\n", PROGRAM);
		return ERROR;
	}

	calibration_print(&calibration);

	if (file_name == NULL)
	{
		return SUCCESS;
	}

	if (calibration_export(file_name, &calibration) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, file_name);
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

static int command_record(char * file_name)
{
	struct Gesture gesture;

	if (gesture_record(&gesture) == ERROR)
	{
		log_print(LOG_SUCCESS, "%s: Failed to record gesture\n", __func__);
		return ERROR;
	}

	gesture_print(&gesture);

	if (file_name == NULL)
	{
		return SUCCESS;
	}

	if (gesture_export(file_name, &gesture, 1) == ERROR)
	{
		log_print(LOG_ERROR, "%s: Failed to gesture to '%s'\n", PROGRAM, file_name);
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

int command_execute(char * program, enum Command command, char * argument)
{
	if (program == NULL)
	{
		PROGRAM = "";
	}
	else
	{
		PROGRAM = program;
	}

	switch (command)
	{
		case COMMAND_RUN:
			return command_run(argument);
			break;

		case COMMAND_SAMPLE:
			return command_sample();
			break;

		case COMMAND_CALIBRATE:
			return command_calibrate(argument);
			break;

		case COMMAND_UPLOAD:
			return command_upload(argument);
			break;

		case COMMAND_DOWNLOAD:
			return command_download(argument);
			break;

		case COMMAND_MODE:
			return command_mode(argument);
			break;

		case COMMAND_RECORD:
			return command_record(argument);
			break;

		default:
			log_print(LOG_ERROR, "%s(): Invalid command\n", __func__);
			return ERROR;
	}
}
