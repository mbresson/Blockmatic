
/*
 * param.c
 *
 * Copyright 2012 Bresson Matthieu <mbresson@etudiant.univ-mlv.fr>
 *
 * This file is part of Blockmatic.
 *
 * Blockmatic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blockmatic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULIAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blockmatic. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "param.h"

#include "debug.h"
#include "constants.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * this two pointers are there because
 * I'm too lazy to send a copy of argc and argv to every static function
 * needed to parse command-line arguments
 *
 * before and after parse_params is called, they are set to NULL for more safety
 */
static const int *s_argc = NULL;
static char * const *s_argv = NULL;


static inline bool equals(const char *str1, const char *str2) {
	assert(str1 != NULL);
	assert(str2 != NULL);

	return (strcmp(str1, str2) == 0);
}


static inline bool file_exists(const char *filename) {
	assert(filename != NULL);

	FILE *tmp = fopen(filename, "r");

	if(tmp != NULL) {
		fclose(tmp);
		return true;
	} else {
		return false;
	}
}


static inline void set_default_settings(Settings *obj) {
	assert(obj != NULL);

	/*
	 * set every field in Settings struct object
	 * to its default value if no other value was provided
	 */

	if(obj->background_file == NULL) {
		obj->background_file = DEFAULT_BACKGROUND_FILE;
	}

	if(obj->background_center == undef) {
		obj->background_center = DEFAULT_BACKGROUND_CENTER;
	}

	if(obj->background_crop == undef) {
		obj->background_crop = DEFAULT_BACKGROUND_CROP;
	}

	if(obj->background_color.red == -1) {
		obj->background_color.red = DEFAULT_BACKGROUND_RED;
	}

	if(obj->background_color.green == -1) {
		obj->background_color.green = DEFAULT_BACKGROUND_GREEN;
	}

	if(obj->background_color.blue == -1) {
		obj->background_color.blue = DEFAULT_BACKGROUND_BLUE;
	}

	if(obj->block_file == NULL) {
		obj->block_file = DEFAULT_BLOCK_FILE;
	}

	if(obj->block_size == -1) {
		obj->block_size = DEFAULT_BLOCK_SIZE;
	}

	// a preview block size is half a real block size
	obj->preview_size = obj->block_size / 2;

	if(obj->blocks_per_col == -1) {
		obj->blocks_per_col = DEFAULT_BLOCKS_PER_COL;
	}

	if(obj->blocks_per_row == -1) {
		obj->blocks_per_row = DEFAULT_BLOCKS_PER_ROW;
	}

	if(obj->cheatmode == undef) {
		obj->cheatmode = false;
	}

	if(obj->decrease == -1) {
		obj->decrease = DEFAULT_DECREASE;
	}

	if(obj->delay == -1) {
		obj->delay = DEFAULT_DELAY;
	}

	if(obj->duration == -1) {
		obj->duration = DEFAULT_DURATION;
	}

	if(obj->font_file == NULL) {
		obj->font_file = DEFAULT_FONT_FILE;
	}

	if(obj->font_size == -1) {
		obj->font_size = DEFAULT_FONT_SIZE;
	}

	if(obj->font_color.red == -1) {
		obj->font_color.red = DEFAULT_FONT_RED;
	}

	if(obj->font_color.green == -1) {
		obj->font_color.green = DEFAULT_FONT_GREEN;
	}

	if(obj->font_color.blue == -1) {
		obj->font_color.blue = DEFAULT_FONT_BLUE;
	}

	if(obj->hints == undef) {
		obj->hints = DEFAULT_HINTS;
	}

	if(obj->keyrepeat == undef) {
		obj->keyrepeat = DEFAULT_KEYREPEAT;
	}

	if(obj->preview == undef) {
		obj->preview = DEFAULT_PREVIEW;
	}

	if(obj->pausemsg == NULL) {
		obj->pausemsg = DEFAULT_PAUSEMSG;
	}

	if(obj->pause_color.red == -1) {
		obj->pause_color.red = DEFAULT_PAUSE_RED;
	}

	if(obj->pause_color.green == -1) {
		obj->pause_color.green = DEFAULT_PAUSE_GREEN;
	}

	if(obj->pause_color.blue == -1) {
		obj->pause_color.blue = DEFAULT_PAUSE_BLUE;
	}

	if(obj->restart == undef) {
		obj->restart = DEFAULT_RESTART;
	}

	if(obj->rows == -1) {
		obj->rows = DEFAULT_ROWS;
	}

	if(obj->threshold == -1) {
		obj->threshold = DEFAULT_THRESHOLD;
	}

	if(obj->usedelay == undef) {
		obj->usedelay = DEFAULT_USEDELAY;
	}

	if(obj->window_icon == NULL) {
		obj->window_icon = DEFAULT_WINDOW_ICON;
	}

	if(obj->window_title == NULL) {
		obj->window_title = DEFAULT_WINDOW_TITLE;
	}

	if(obj->window_noborder == undef) {
		obj->window_noborder = DEFAULT_WINDOW_NOBORDER;
	}
}


static inline void set_undef_settings(Settings *obj) {
	assert(obj != NULL);

	/*
	 * set every field in Settings struct object
	 * to its undefined value:
	 * pointer => NULL
	 * numeric => -1
	 * bool => undef
	 */

	obj->background_file = NULL;
	obj->background_center = undef;
	obj->background_crop = undef;

	obj->background_color.red = -1;
	obj->background_color.green = -1;
	obj->background_color.blue = -1;

	obj->block_file = NULL;
	obj->block_size = -1;
	obj->blocks_per_col = -1;
	obj->blocks_per_row = -1;

	obj->cheatmode = undef;

	obj->decrease = -1;
	obj->delay = -1;
	obj->duration = -1;

	obj->font_file = NULL;
	obj->font_size = -1;

	obj->font_color.red = -1;
	obj->font_color.green = -1;
	obj->font_color.blue = -1;

	obj->hints = undef;

	obj->keyrepeat = undef;
	obj->preview = undef;

	obj->pausemsg = NULL;

	obj->pause_color.red = -1;
	obj->pause_color.green = -1;
	obj->pause_color.blue = -1;

	obj->restart = undef;

	obj->rows = -1;

	obj->threshold = -1;
	obj->usedelay = undef;

	obj->window_icon = NULL;
	obj->window_title = NULL;
	obj->window_noborder = undef;
}


static void print_version(void);
static void print_help(void);


/*
 * this function looks for conflits or dependencies
 * and prints warnings in stderr when needed
 */
static void check_to_warn(Settings *obj) {
	assert(obj != NULL);

	// if background_crop is set but there is no background picture
	if(obj->background_file == NULL && obj->background_crop != undef) {
		fprintf(stderr, "'%s': statement with no effect (a background file must be specified)!\n",
				PARAM_BACKGROUND_CROP);

		obj->background_crop = undef;
	}

	// if background_center is set but the background picture is not cropped
	if(obj->background_center == true && (obj->background_crop == false ||
		(obj->background_crop == undef && !DEFAULT_BACKGROUND_CROP))) {

		fprintf(stderr, "'%s': statement with no effect (background picture must be cropped ('%s'))!\n",
			PARAM_BACKGROUND_CENTER, PARAM_BACKGROUND_CROP);
		
		obj->background_center = undef;
	}

	// if a delay has been set but won't be used
	if(obj->delay != -1 && (obj->usedelay == false ||
		(obj->usedelay == undef && !DEFAULT_USEDELAY))) {

		fprintf(stderr, "'%s': statement with no effect (delay must be switched on ('%s'))!\n",
			PARAM_DELAY, PARAM_USEDELAY);

		obj->delay = -1;
	}

	// if a threshold is set but won't be used
	if(obj->threshold != -1 && (obj->usedelay == true ||
		(obj->usedelay == undef && DEFAULT_USEDELAY))) {

		fprintf(stderr, "'%s': statement with no effect (delay must be switched off (no '%s'))!\n",
			PARAM_THRESHOLD, PARAM_USEDELAY);

		obj->threshold = -1;
	}
}


static bool check_file_parameter(int index, char **target) {
	assert(target != NULL);
	assert(s_argc != NULL && s_argv != NULL);
	assert(index < *s_argc);

	const char *param = s_argv[index];

	/*
	 * check if a filename is provided
	 * check if given filename exists
	 */

	if(index == (*s_argc) - 1) {
		fprintf(stderr, "'%s': you must provide a file name!\n", param);
		return false;
	} else if(!file_exists(s_argv[index+1])) {
		fprintf(stderr, "'%s': file '%s' does not exist!\n", param, s_argv[index+1]);
		return false;
	} else if(*target != NULL) {
		fprintf(stderr, "'%s': you cannot define it twice!\n", param);
		return false;
	} else {
		*target = s_argv[index+1];
		return true;
	}
}


/*
 * this function reads three numeric values from the current parameter
 * the values must be formatted as following: "red,green,blue"
 * they cannot be lower than 0 or higher than 255
 */
static bool check_color_parameter(int index, int *red, int *green, int *blue) {
	assert(red != NULL);
	assert(green != NULL);
	assert(blue != NULL);

	const char *param = s_argv[index];

	/*
	 * check if an argument is provided
	 * check if it suits the right format
	 */

	if(index == (*s_argc) - 1) {
		fprintf(stderr, "'%s': you must provide an argument!\n", param);
		return false;
	} else {
		const char *format = "%u,%u,%u";
		int tmp_red, tmp_green, tmp_blue;

		int result = sscanf(s_argv[index+1], format, &tmp_red, &tmp_green, &tmp_blue);

		if(result == 3) {
			if(tmp_red < 0 || tmp_red > 255 || tmp_green < 0 || tmp_green > 255 || tmp_blue < 0 || tmp_blue > 255) {
				fprintf(stderr, "'%s': values in '%s' must not be lower than 0 or higher than 255!\n",
					param, s_argv[index+1]);
				return false;
			}

			if(*red != -1 || *green != -1 || *blue != -1) {
				fprintf(stderr, "'%s': you cannot define it twice!\n", param);
				return false;
			}

			*red = tmp_red;
			*green = tmp_green;
			*blue = tmp_blue;
			return true;
		} else {
			fprintf(stderr, "'%s': parameter '%s' isn't correctly formatted!\n", param, s_argv[index+1]);
			return false;
		}
	}
}


static bool check_numeric_parameter(int index, int *target, int min, int max) {
	assert(target != NULL);
	assert(s_argc != NULL && s_argv != NULL);
	assert(index < *s_argc);

	const char *param = s_argv[index];

	/*
	 * check if a number is provided
	 * check if this number is not invalid or out of range
	 */

	if(index == (*s_argc) - 1) {
		fprintf(stderr, "'%s': you must provide a numeric value!\n", param);
		return false;
	} else {
		errno = 0;
		char *char_ptr = NULL;

		long int result = strtol(s_argv[index+1], &char_ptr, 10);

		if(errno == ERANGE || result > max || result < min) {
			fprintf(stderr, "'%s': value '%s' out of range (min: %d, max: %d)!\n", param, s_argv[index+1], min, max);
			return false;
		} else if(*char_ptr != '\0') {
			fprintf(stderr, "'%s': value '%s' is not a number!\n", param, s_argv[index+1]);
			return false;
		} else if(*target != -1) {
			fprintf(stderr, "'%s': you cannot define it twice!\n", param);
			return false;
		} else {
			*target = (int) result;
			return true;
		}
	}
}


Settings* parse_params(int argc, char **argv) {
	Settings *tmp = malloc(sizeof(Settings));
	if(!tmp) {
		fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(Settings));
		exit(EXIT_FAILURE);
	}

	tmp->leave = false;

	s_argc = &argc;
	s_argv = argv;

	// before parsing, every value is initialized to prevent strange errors
	set_undef_settings(tmp);

	int index = 1;
	while(index < *s_argc && !tmp->leave) {
		const char *param = s_argv[index];

		/*
		 * for each parameter,
		 * check if the required argument is provided
		 * if it's all right, increment index once more to go to the next parameter,
		 * if it's all wrong, stop, set tmp->leave to true and return
		 */

		if(equals(param, PARAM_HELP)) {
			print_help();

			tmp->leave = true;

		} else if(equals(param, PARAM_VERSION)) {
			print_version();

			tmp->leave = true;

		} else if(equals(param, PARAM_BACKGROUND_FILE)) {
			if(!check_file_parameter(index, &(tmp->background_file))) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BACKGROUND_CENTER)) {
			tmp->background_center = true;

		} else if(equals(param, PARAM_BACKGROUND_CROP)) {
			tmp->background_crop = true;

		} else if(equals(param, PARAM_BACKGROUND_COLOR)) {
			if(!check_color_parameter(index,
				&(tmp->background_color.red), &(tmp->background_color.green), &(tmp->background_color.blue))) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCK_FILE)) {
			if(!check_file_parameter(index, &(tmp->block_file))) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCK_SIZE)) {
			if(!check_numeric_parameter(index, &(tmp->block_size), 1, INT_MAX)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCKS_PER_COL)) {
			int min_blocks_per_col = tmp->rows == -1 ? 8 : tmp->rows + 4;
			if(!check_numeric_parameter(index, &(tmp->blocks_per_col), min_blocks_per_col, INT_MAX)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCKS_PER_ROW)) {
			if(!check_numeric_parameter(index, &(tmp->blocks_per_row), 8, INT_MAX)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_DECREASE)) {
			if(!check_numeric_parameter(index, &(tmp->decrease), 0, 99)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_DELAY)) {
			if(!check_numeric_parameter(index, &(tmp->delay), 1, 86400)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_DURATION)) {
			if(!check_numeric_parameter(index, &(tmp->duration), 100, 10000)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_FONT_FILE)) {
			if(!check_file_parameter(index, &(tmp->font_file))) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_FONT_SIZE)) {
			if(!check_numeric_parameter(index, &(tmp->font_size), 1, INT_MAX)) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_FONT_COLOR)) {
			if(!check_color_parameter(index,
				&(tmp->font_color.red), &(tmp->font_color.green), &(tmp->font_color.blue))) {
			} else {
				index++;
			}

		} else if(equals(param, PARAM_NOHINTS)) {
			tmp->hints = false;

		} else if(equals(param, PARAM_NOKEYREPEAT)) {
			tmp->keyrepeat = false;

		} else if(equals(param, PARAM_NOPREVIEW)) {
			tmp->preview = false;

		} else if(equals(param, PARAM_PAUSEMSG)) {
			if(index == (*s_argc) - 1) {
				fprintf(stderr, "'%s': you must provide a string!\n", param);
				tmp->leave = true;
			} else if(tmp->pausemsg != NULL) {
				fprintf(stderr, "'%s': you cannot define it twice!\n", param);
				tmp->leave = true;
			} else {
				if(strlen(s_argv[index+1]) == 0) {
					tmp->pausemsg = " ";
				} else {
					tmp->pausemsg = s_argv[index+1];
				}

				if(equals(s_argv[index+1], CHEATMODE_STRING)) {
					printf("Cheat mode enabled, press 'd' to delete last row of blocks.\n");
					tmp->cheatmode = true;
				}
				index++;
			}

		} else if(equals(param, PARAM_PAUSE_COLOR)) {
			if(!check_color_parameter(index,
				&(tmp->pause_color.red), &(tmp->pause_color.green), &(tmp->pause_color.blue))) {
				tmp->leave = true;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_RESTART)) {
			tmp->restart = true;

		} else if(equals(param, PARAM_ROWS)) {
			int max_rows = (tmp->blocks_per_col == -1 ? DEFAULT_BLOCKS_PER_COL : tmp->blocks_per_col) - 4;
			if(!check_numeric_parameter(index, &(tmp->rows), 0, max_rows)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_THRESHOLD)) {
			if(!check_numeric_parameter(index, &(tmp->threshold), 1, 1000000)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_USEDELAY)) {
			tmp->usedelay = true;

		} else if(equals(param, PARAM_WINDOW_ICON)) {
			if(!check_file_parameter(index, &(tmp->window_icon))) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_WINDOW_TITLE)) {
			if(index == (*s_argc) - 1) {
				fprintf(stderr, "'%s': you must provide a string!\n", param);
				tmp->leave = true;
			} else {
				tmp->window_title = s_argv[index+1];
				index++;
			}

		} else if(equals(param, PARAM_WINDOW_NOBORDER)) {
			tmp->window_noborder = true;

		} else {
			fprintf(stderr, "Invalid option '%s'!\n", param);
			tmp->leave = true;
		}

		index++;
	}

	if(!tmp->leave) {
		check_to_warn(tmp);

		// after parsing, every member left undefined is set to its default value
		set_default_settings(tmp);
	}

	s_argc = NULL;
	s_argv = NULL;

	return tmp;
}


static void print_help(void) {
	puts(GAME_TITLE " " GAME_VERSION " by " GAME_CREATOR "\n");

	printf("\t" PARAM_HELP "\n \
		list available options\n\n");

	printf("\t" PARAM_VERSION "\n \
		display version information\n\n");

	printf("\t" PARAM_BACKGROUND_FILE " file.{png,jpg,bmp}\n \
		the path to a background image\n \
		default: %s\n\n", DEFAULT_BACKGROUND_FILE == NULL ? "no background image" : DEFAULT_BACKGROUND_FILE);

	printf("\t" PARAM_BACKGROUND_CENTER "\n \
		if set, the background image (if cropped) will be centered when displayed\n \
		default: %s\n\n", DEFAULT_BACKGROUND_CENTER ? "background centered" : "background not centered");

	printf("\t" PARAM_BACKGROUND_COLOR "\n \
		set a background color (if no background image) in the format 'red,green,blue'\n \
		default: %d,%d,%d, min: 0, max: 255\n\n", DEFAULT_BACKGROUND_RED, DEFAULT_BACKGROUND_GREEN, DEFAULT_BACKGROUND_BLUE);

	printf("\t" PARAM_BACKGROUND_CROP "\n \
		if set, the background image (if provided) will be cropped instead of being stretched when displayed\n \
		default: %s\n\n", DEFAULT_BACKGROUND_CROP ? "background cropped" : "background stretched");

	printf("\t" PARAM_BLOCK_FILE " file.{png,jpg,bmp}\n \
		the path to the image to use to represent blocks\n \
		default: '" DEFAULT_BLOCK_FILE "'\n\n");

	printf("\t" PARAM_BLOCK_SIZE " number\n \
		the standard size for blocks (width == height, used when computing the window's size)\n \
		default: %d px, min: 1\n\n", DEFAULT_BLOCK_SIZE);

	printf("\t" PARAM_BLOCKS_PER_COL " number\n \
		the number of blocks per column (used when computing the window's height)\n \
		default: %d, min: 8\n\n", DEFAULT_BLOCKS_PER_COL);

	printf("\t" PARAM_BLOCKS_PER_ROW " number\n \
		the number of blocks per row (used when computing the window's width)\n \
		default: %d, min: 8\n\n", DEFAULT_BLOCKS_PER_ROW);

	printf("\t" PARAM_DECREASE " number\n \
		the percentage of duration (ms) decrease\n \
		default: %d%%, min: 0, max: 99\n\n", DEFAULT_DECREASE);

	printf("\t" PARAM_DELAY " number\n \
		the number of seconds before duration (ms) is decreased by decrease (%%)\n \
		default: %d%%, min: 1, max: 86400\n\n", DEFAULT_DELAY);

	printf("\t" PARAM_DURATION " number\n \
		the number of milliseconds to wait before moving down a tetri\n \
		default: %d, min: 100, max: 10000\n\n", DEFAULT_DURATION);

	printf("\t" PARAM_FONT_FILE " file.{otf,ttf}\n \
		the path to the font file to be used to display various informations\n \
		default: '" DEFAULT_FONT_FILE "'\n\n");

	printf("\t" PARAM_FONT_SIZE " number\n \
		the size of the font, pause text will be twice this size\n \
		default: %d\n\n", DEFAULT_FONT_SIZE);

	printf("\t" PARAM_FONT_COLOR "\n \
		set a color for the text in the format 'red,green,blue'\n \
		default: %d,%d,%d, min: 0, max: 255\n\n", DEFAULT_FONT_RED, DEFAULT_FONT_GREEN, DEFAULT_FONT_BLUE);

	printf("\t" PARAM_NOHINTS "\n \
		if set, no hints will be displayed (hints == time remaining until moving down)\n \
		default: %s\n\n", DEFAULT_HINTS ? "hints allowed" : "no hints");

	printf("\t" PARAM_NOKEYREPEAT "\n \
		if set, key repeat will be disabled\n \
		default: %s\n\n", DEFAULT_KEYREPEAT ? "key repeat enabled" : "no key repeat");

	printf("\t" PARAM_NOPREVIEW "\n \
		if set, the next tetrimino can't be previewed\n \
		default: %s\n\n", DEFAULT_PREVIEW ? "preview enabled" : "no preview");

	printf("\t" PARAM_PAUSEMSG " string\n \
		the message to be displayed when the game is paused\n \
		default: '" DEFAULT_PAUSEMSG "'\n\n");

	printf("\t" PARAM_PAUSE_COLOR "\n \
		set a color for the screen when the game is paused in the format 'red,green,blue'\n \
		default: %d,%d,%d, min: 0, max: 255\n\n", DEFAULT_PAUSE_RED, DEFAULT_PAUSE_GREEN, DEFAULT_PAUSE_BLUE);

	printf("\t" PARAM_RESTART "\n \
		if set, when the game is over, a new game is started\n \
		default: %s\n\n", DEFAULT_RESTART ? "restart enabled" : "no restart");

	printf("\t" PARAM_ROWS " number\n \
		the number of rows to be randomly filled with blocks at the beginning\n \
		default: %d, min: 0, max: %d or the user-defined number of blocks per column\n\n",
		DEFAULT_ROWS, DEFAULT_BLOCKS_PER_COL);

	printf("\t" PARAM_THRESHOLD " number\n \
		the number of rows to be completed before duration (ms) is decreased by decrease (%%)\n \
		default: %d%%, min: 1, max: 1000000\n\n", DEFAULT_THRESHOLD);

	printf("\t" PARAM_USEDELAY "\n \
		to decide if duration (ms) must be decreased after delay (s)\n \
		default: %s\n\n", DEFAULT_USEDELAY ? "use delay" : "use threshold");

	printf("\t" PARAM_WINDOW_ICON " file.{png,jpg,bmp}\n \
		the path to the window's icon file\n \
		default: '" DEFAULT_WINDOW_ICON "'\n\n");

	printf("\t" PARAM_WINDOW_TITLE " string\n \
		the window's title\n \
		default: '" DEFAULT_WINDOW_TITLE "'\n\n");

	printf("\t" PARAM_WINDOW_NOBORDER "\n \
		to create a window without borders\n \
		default: %s\n\n", DEFAULT_WINDOW_NOBORDER ? "no border" : "window with borders");

	printf("Runtime control keys:\n\n");

	printf("\tn\n\t\tnew game\n\n");

	printf("\tescape\n\t\tleave game\n\n");

	printf("\tp\n\t\tpause game\n\n");

	printf("\tleft\n\t\tmove left\n\n");

	printf("\tright\n\t\tmove right\n\n");

	printf("\tup\n\t\tcounter-clockwise rotation\n\n");

	printf("\tdown\n\t\tclockwise rotation\n\n");

	printf("\tshift + down\n\t\tmove down\n\n");

	printf("\tspace\n\t\tdrop tetri\n\n");

	printf("\t?\n\t\tdelete last row (cheat mode)\n\n");
}


static void print_version(void) {
	puts(GAME_TITLE " " GAME_VERSION " by " GAME_CREATOR);
}
