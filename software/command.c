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

static Gesture * GESTURES = NULL;   // points to the list of gestures (freed on exit)
static int QUANTITY = 0;            // the number of gestures in the list

static char * subcommand_string(Command command);
static char * arguments_string(Command command);
static char * description_string(Command command);
static bool argument_is_help(char * argument);

static int command_help(void);
static int command_run(char * calibration_file, char * gestures_file);
static int command_sample(void);
static int command_calibrate(char * calibration_file);
static int command_upload(char * calibration_file);
static int command_download(char * calibration_file);
static int command_raw(void);
static int command_scaled(void);
static int command_record(char * gesture_file);

char * command_string(Command command)
{
	switch (command)
	{
		case COMMAND_RUN:         return "RUN";
		case COMMAND_SAMPLE:      return "SAMPLE";
		case COMMAND_CALIBRATE:   return "CALIBRATE";
		case COMMAND_UPLOAD:      return "UPLOAD";
		case COMMAND_DOWNLOAD:    return "DOWNLOAD";
		case COMMAND_RAW:         return "RAW";
		case COMMAND_SCALED:      return "SCALED";
		case COMMAND_RECORD:      return "RECORD";
		case COMMAND_HELP:        return "HELP";
		case COMMAND_DEXTERITY:   return "dexterity";
		default:                  return "INVALID";
	}
}

static char * subcommand_string(Command command)
{
	switch (command)
	{
		case COMMAND_RUN:         return "run";
		case COMMAND_SAMPLE:      return "sample";
		case COMMAND_CALIBRATE:   return "calibrate";
		case COMMAND_UPLOAD:      return "upload";
		case COMMAND_DOWNLOAD:    return "download";
		case COMMAND_RAW:         return "raw";
		case COMMAND_SCALED:      return "scaled";
		case COMMAND_RECORD:      return "record";
		case COMMAND_HELP:        return "--help";
		case COMMAND_DEXTERITY:   return "<subcommand>";
		default:                  return "INVALID";
	}
}

static char * arguments_string(Command command)
{
	switch (command)
	{
		case COMMAND_RUN:         return "<calibration_file> <gestures_file>";
		case COMMAND_SAMPLE:      return "";
		case COMMAND_CALIBRATE:   return "[output_file]";
		case COMMAND_UPLOAD:      return "<input_file>";
		case COMMAND_DOWNLOAD:    return "[output_file]";
		case COMMAND_RAW:         return "";
		case COMMAND_SCALED:      return "";
		case COMMAND_RECORD:      return "[output_file]";
		case COMMAND_HELP:        return "";
		case COMMAND_DEXTERITY:   return "<arguments>";
		default:                  return "INVALID";
	}
}

static char * description_string(Command command)
{
	switch (command)
	{
		case COMMAND_RUN:         return "Use the device as a computer peripheral";
		case COMMAND_SAMPLE:      return "Monitor the analogue sensor readings";
		case COMMAND_CALIBRATE:   return "Record a calibration for the device";
		case COMMAND_UPLOAD:      return "Upload a calibration to the device";
		case COMMAND_DOWNLOAD:    return "Download the current device calibration ";
		case COMMAND_RAW:         return "Disable scaling of analogue sensor readings";
		case COMMAND_SCALED:      return "Enable scaling of analogue sensor readings";
		case COMMAND_RECORD:      return "Record an N-phase gesture";
		case COMMAND_HELP:        return "Print command usage information";
		case COMMAND_DEXTERITY:   return "Dexterity is a wearable gesture recognition device";
		default:                  return "INVALID";
	}
}

void print_usage(Command command, bool align)
{
	char const * program = command_string(COMMAND_DEXTERITY);
	char const * subcommand = subcommand_string(command);
	char const * help = (command == COMMAND_HELP ? "" : "[--help]");
	char const * arguments = arguments_string(command);

	if (align)
	{
		log(LOG_INFO, "%-9s %-9s %-8s %-s\n", program, subcommand, help, arguments);
	}
	else
	{
		log(LOG_INFO, "%s %s %s %s\n", program, subcommand, help, arguments);
	}

}

void print_description(Command command)
{
	log(LOG_INFO, "%s\n", description_string(command));
}

static bool argument_is_help(char * argument)
{
	if (argument == NULL)
	{
		return false;
	}

	char const * string = subcommand_string(COMMAND_HELP);

	if (strncmp(argument, string, strlen(string)) == 0)
	{
		return true;
	}

	return false;
}

void command_free_gestures(void)
{
	gesture_destroy(GESTURES, QUANTITY);
	GESTURES = NULL;
	QUANTITY = 0;
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

	int result = ERROR;
	GESTURES = NULL;
	QUANTITY = 0;

	if (gesture_import(gestures_file, &GESTURES, &QUANTITY) == ERROR)
	{
		log(LOG_ERROR, "Failed to import gestures from '%s'\n", gestures_file);
		goto EXIT;
	}

	if (QUANTITY != NUM_ACTIONS)
	{
		log(LOG_ERROR, "Incorrect number of gestures imported from '%s': Expected %d but imported %d\n",
		                gestures_file, NUM_ACTIONS, QUANTITY);
	}

	for (Action expected = 0; expected < NUM_ACTIONS; expected++)
	{
		Action actual = GESTURES[expected].action;

		if (actual != expected)
		{
			log(LOG_ERROR, "Incorrect action for gesture #%d: Expected '%s' but parsed '%s'\n",
			                expected, action_string(expected), action_string(actual));
			goto EXIT;
		}
	}

	log(LOG_SUCCESS, "Imported %d gestures from '%s'\n", QUANTITY, gestures_file);

	for (int i = 0; i < QUANTITY; i++)
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

	Hand hand;
	Control control = CONTROL_MOUSE;
	bool disabled = true;

	printf("\n-------- CONTROLS --------\n");
	printf("\r%-*s", ENABLE_DISABLE_PADDING, "DISABLED");
	fflush(stdout);

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
				printf("\r%-*s", ENABLE_DISABLE_PADDING, "ENABLED");
				fflush(stdout);
				disabled = false;   // bring the device back from a disabled state
			}
			else
			{
				printf("\r%-*s", ENABLE_DISABLE_PADDING, "DISABLED");
				fflush(stdout);
				disabled = true;   // ignore all input from device until re-enabled
			}

			continue;
		}

		if (disabled)
		{
			continue;   // don't proceed with gesture recognition if disabled
		}

		if (gesture_matches(ACTION_CYCLE, GESTURES, QUANTITY, &hand))
		{
			control = (control + 1) % NUM_CONTROLS;   // cycle to next control
			continue;
		}

		if (control_execute(control, GESTURES, QUANTITY, &hand) == ERROR)
		{
			log(LOG_ERROR, "Failed to control %s\n", control_string(control));
			goto EXIT;
		}
	}

	result = SUCCESS;

EXIT:
	gesture_destroy(GESTURES, QUANTITY);
	GESTURES = NULL;
	return result;
}

static int command_sample(void)
{
	Hand hand;
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
		printf(RED    " %s" WHITE ": % 4hd |", direction_string(X),   hand.accel[X]);
		printf(RED    " %s" WHITE ": % 4hd |", direction_string(Y),   hand.accel[Y]);
		printf(RED    " %s" WHITE ": % 4hd |", direction_string(Z),   hand.accel[Z]);
		printf(YELLOW " %s" WHITE ": % 4hd |", finger_string(THUMB),  hand.flex[THUMB]);
		printf(YELLOW " %s" WHITE ": % 4hd |", finger_string(INDEX),  hand.flex[INDEX]);
		printf(YELLOW " %s" WHITE ": % 4hd |", finger_string(MIDDLE), hand.flex[MIDDLE]);
		printf(YELLOW " %s" WHITE ": % 4hd |", finger_string(RING),   hand.flex[RING]);
		printf(YELLOW " %s" WHITE ": % 4hd |", finger_string(PINKY),  hand.flex[PINKY]);
		printf(GREEN  " %s" WHITE ": % 1hhd |", "BUTTON",        hand.button);
		printf(GREEN  " %s" WHITE ": % 1hhd |", "LED",           hand.led);

		fflush(stdout);
	}

	return SUCCESS;
}

static int command_calibrate(char * calibration_file)
{
	Calibration calibration;

	if (calibration_interactive(&calibration) == ERROR)
	{
		log(LOG_ERROR, "Interactive calibration failed\n");
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

static int command_upload(char * calibration_file)
{
	if (calibration_file == NULL)
	{
		log(LOG_ERROR, "No calibration file provided\n");
		return ERROR;
	}

	Calibration calibration;

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
	Calibration calibration;

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

static int command_raw(void)
{
	if (raw() == ERROR)
	{
		log(LOG_ERROR, "Failed to set device to raw mode\n");
		return ERROR;
	}

	log(LOG_SUCCESS, "Set device to raw mode\n");
	return SUCCESS;
}

static int command_scaled(void)
{
	if (scaled() == ERROR)
	{
		log(LOG_ERROR, "Failed to set device to scaled mode\n");
		return ERROR;
	}

	log(LOG_SUCCESS, "Set device to scaled mode\n");
	return SUCCESS;
}

static int command_record(char * gesture_file)
{
	Gesture gesture;

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

static int command_help(void)
{
	printf("==================================================================\n");
	log(LOG_INFO, "USAGE:\n");
	print_usage(COMMAND_DEXTERITY, false);
	printf("------------------------------------------------------------------\n");
	log(LOG_INFO, "COMMANDS:\n");

	for (Command command = 0; command < NUM_COMMANDS; command++)
	{
		print_usage(command, true);
	}

	printf("==================================================================\n");
	return SUCCESS;
}

static int command_dexterity(void)
{
	print_description(COMMAND_DEXTERITY);
	print_usage(COMMAND_DEXTERITY, false);
	log(LOG_INFO, "Please try 'dexterity --help' for a list of commands\n");
	return SUCCESS;
}

Command command_identify(char * subcommand)
{
	if (subcommand == NULL)
	{
		return COMMAND_DEXTERITY;   // default subcommand
	}

	for (Command command = 0; command < NUM_COMMANDS; command++)
	{
		char const * string = subcommand_string(command);

		if (strncmp(subcommand, string, strlen(string)) == 0)
		{
			return command;
		}
	}

	return COMMAND_UNKNOWN;
}

int command_execute(Command command, char ** arguments, int count)
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

	if (argument_is_help(arg0))
	{
		print_description(command);
		print_usage(command, false);
		return SUCCESS;
	}

	if (command != COMMAND_DEXTERITY && command != COMMAND_HELP)
	{
		if (init() == ERROR)
		{
			return ERROR;
		}
	}

	usleep(1000);   // give the serial port some time to initialize
	int result;

	switch (command)
	{
		case COMMAND_RUN:
			result = command_run(arg0, arg1);
			break;

		case COMMAND_SAMPLE:
			result = command_sample();
			break;

		case COMMAND_CALIBRATE:
			result = command_calibrate(arg0);
			break;

		case COMMAND_UPLOAD:
			result = command_upload(arg0);
			break;

		case COMMAND_DOWNLOAD:
			result = command_download(arg0);
			break;

		case COMMAND_RAW:
			result = command_raw();
			break;

		case COMMAND_SCALED:
			result = command_scaled();
			break;

		case COMMAND_RECORD:
			result = command_record(arg0);
			break;

		case COMMAND_HELP:
			result = command_help();
			break;

		case COMMAND_DEXTERITY:
			result = command_dexterity();
			break;

		default:
			log(LOG_ERROR, "Invalid command\n");
			result = ERROR;
			break;
	}

	cleanup(result);
	return result;
}
