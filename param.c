
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

	obj->hints = undef;

	obj->keyrepeat = undef;
	obj->preview = undef;

	obj->pausemsg = NULL;

	obj->restart = undef;

	obj->rows = -1;

	obj->threshold = -1;
	obj->usedelay = undef;

	obj->window_icon = NULL;
	obj->window_title = NULL;
}


static void print_version(void);
static void print_help(void);


static inline bool check_file_parameter(int index, char **target) {
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
	} else {
		*target = s_argv[index+1];
		return true;
	}
}


static inline bool check_numeric_parameter(int index, int *target, int min, int max) {
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

	// if first parameter is --help, print help
	if(*s_argc == 2 && equals(s_argv[1], "--help")) {
		print_help();

		tmp->leave = true;
		return tmp;
	} else if(*s_argc == 2 && equals(s_argv[1], "--version")) {
		print_version();

		tmp->leave = true;
		return tmp;
	}

	int index = 1;
	while(index < *s_argc) {
		const char *param = s_argv[index];

		/*
		 * for each parameter,
		 * check if the required argument is provided
		 * if it's all right, increment index once more to go to the next parameter,
		 * if it's all wrong, stop, set tmp->leave to true and return
		 */

		if(equals(param, PARAM_BACKGROUND_FILE)) {
			if(!check_file_parameter(index, &(tmp->background_file))) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCK_FILE)) {
			if(!check_file_parameter(index, &(tmp->block_file))) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCK_SIZE)) {
			if(!check_numeric_parameter(index, &(tmp->block_size), 1, INT_MAX)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCKS_PER_COL)) {
			int min_blocks_per_col = tmp->rows == -1 ? 8 : tmp->rows + 4;
			if(!check_numeric_parameter(index, &(tmp->blocks_per_col), min_blocks_per_col, INT_MAX)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_BLOCKS_PER_ROW)) {
			if(!check_numeric_parameter(index, &(tmp->blocks_per_row), 8, INT_MAX)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_DECREASE)) {
			if(!check_numeric_parameter(index, &(tmp->decrease), 0, 99)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_DELAY)) {
			if(!check_numeric_parameter(index, &(tmp->delay), 1, 86400)) {
				tmp->leave = true;
				break;
			} else {
				if(tmp->usedelay == false || (tmp->usedelay == undef && !DEFAULT_USEDELAY)) {
					printf("'%s': statement with no effect (require --usedelay).\n", param);
				}
				index++;
			}

		} else if(equals(param, PARAM_DURATION)) {
			if(!check_numeric_parameter(index, &(tmp->duration), 100, 10000)) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_FONT_FILE)) {
			if(!check_file_parameter(index, &(tmp->font_file))) {
				tmp->leave = true;
				break;
			} else {
				index++;
			}

		} else if(equals(param, PARAM_FONT_SIZE)) {
			if(!check_numeric_parameter(index, &(tmp->font_size), 1, INT_MAX)) {
				tmp->leave = true;
				break;
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
				break;
			} else {
				tmp->pausemsg = s_argv[index+1];
				if(equals(s_argv[index+1], "42")) {
					printf("Cheat mode enabled, press 'd' to delete last row of blocks.\n");
					tmp->cheatmode = true;
				}
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
				break;
			} else {
				tmp->window_title = s_argv[index+1];
				index++;
			}

		} else {
			fprintf(stderr, "Invalid option '%s'!\n", param);
			tmp->leave = true;
			break;
		}

		index++;
	}

	// after parsing, every member left undefined is set to its default value
	set_default_settings(tmp);

	s_argc = NULL;
	s_argv = NULL;

	return tmp;
}


static void print_help(void) {
	puts(GAME_TITLE " " GAME_VERSION " by " GAME_CREATOR "\n");

	printf("\t--help\n \
		list available options\n\n");

	printf("\t" PARAM_BACKGROUND_FILE " file.{png,jpg,bmp}\n \
		the path to a background image\n \
		default: rgb(%d,%d,%d)\n\n", BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE);

	printf("\t" PARAM_BLOCK_FILE " file.{png,jpg,bmp}\n \
		the path to the blocks image\n \
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
