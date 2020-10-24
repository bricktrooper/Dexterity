#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "dexterity.h"
#include "log.h"
#include "utils.h"
#include "serial.h"
#include "calibration.h"

#include "mouse.h"
#include "keyboard.h"

#include "command.h"

#define MAX_COMMAND_SIZE   15

static char * PROGRAM = "";

char * COMMANDS [NUM_COMMANDS] = {
	"run",
	"sample",
	"calibrate",
	"upload",
	"download",
	"raw",
	"scaled"
};

static int command_run(void)
{
	log_suppress(LOG_DEBUG, false);
	struct Hand hand;
	// mouse_glide(234, 345);
	// return SUCCESS;

	while (1)
	{
		if (sample(&hand) != SUCCESS)
		{
			printf(":(\n");
			return ERROR;
		}

		mouse_move(-hand.accel[Z], -hand.accel[X]);

		// if (hand.flex[F1] > 0 && hand.flex[F2] > 0 && hand.flex[F3] <= 0 && hand.flex[F4] <= 0 && hand.flex[F5] <= 0)
		// {
		// 	mouse_single_click(MOUSE_BUTTON_LEFT);
		// }

		// if (hand.accel[X] == 0)
		// {
		// 	printf("STOP\n");
		// 	// mouse_move(0, 0);
		// 	// mouse_scroll(MOUSE_SCROLL_UP, 0);
		// }
		// else if (hand.accel[X] > 0)
		// {
		// 	printf("UP\n");
		// 	mouse_move(0, -2);
		// 	// mouse_scroll(MOUSE_SCROLL_UP, 1);
		// }
		// if (hand.accel[X] < 0)
		// {
		// 	printf("DOWN\n");
		// 	mouse_move(0, 2);
		// 	// mouse_scroll(MOUSE_SCROLL_DOWN, 1);
		// }
	}

	return SUCCESS;
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

		if (result != SUCCESS)
		{
			log_print(LOG_ERROR, "%s: Failed to sample sensors\n", PROGRAM);
			return result;
		}

		latency = ((double)(end - begin) / CLOCKS_PER_SEC) * 1000;   // calculate sampling latency in milliseconds

		printf("[%0.3fms] X: %hd Y: %hd Z: %hd F1: %hd F2: %hd F3: %hd F4: %hd F5: %hd BUTTON: %hhd LED: %hhd\n",
				latency,
				hand.accel[X], hand.accel[Y], hand.accel[Z],
				hand.flex[F1], hand.flex[F2], hand.flex[F3], hand.flex[F4], hand.flex[F5],
				hand.button, hand.led);
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
	int result = calibration_interactive(&calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Interactive calibration failed\n", PROGRAM);
		return result;
	}

	calibration_print(&calibration);
	result = calibration_export(file_name, &calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, file_name);
		return result;
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
	int result = calibration_import(file_name, &calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to import calibration from '%s'\n", PROGRAM, file_name);
		return result;
	}

	calibration_print(&calibration);
	result = calibration_upload(&calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to upload calibration to device\n", PROGRAM);
		return result;
	}

	return SUCCESS;
}

static int command_download(char * file_name)
{
	struct Calibration calibration;
	int result = calibration_download(&calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to download calibration from device\n", PROGRAM);
		return result;
	}

	calibration_print(&calibration);

	if (file_name == NULL)
	{
		return SUCCESS;
	}

	result = calibration_export(file_name, &calibration);

	if (result != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to export calibration to '%s'\n", PROGRAM, file_name);
		return result;
	}

	return SUCCESS;
}

static int command_raw(void)
{
	if (serial_purge() != SUCCESS ||
		serial_write_message(MESSAGE_RAW) != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to disable scaling\n", PROGRAM);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s: Disabled scaling\n", PROGRAM);
	return SUCCESS;
}

static int command_scaled(void)
{
	if (serial_purge() != SUCCESS ||
		serial_write_message(MESSAGE_SCALED) != SUCCESS)
	{
		log_print(LOG_ERROR, "%s: Failed to enable scaling\n", PROGRAM);
		return ERROR;
	}

	log_print(LOG_SUCCESS, "%s: Enabled scaling\n", PROGRAM);
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
			return command_run();
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

		case COMMAND_RAW:
			return command_raw();
			break;

		case COMMAND_SCALED:
			return command_scaled();
			break;

		default:
			log_print(LOG_ERROR, "%s(): Invalid command\n", __func__);
			return ERROR;
	}
}
