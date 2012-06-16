
#ifndef H_PARAM
#define H_PARAM

#include "constants.h"

/*
 * this set of constants defines a string for every legal parameter
 */

/*
 * the path to the background image
 * default: DEFAULT_BACKGROUND_FILE
 * Settings member: background_file
 */
#define PARAM_BACKGROUND_FILE "--background-file"

/*
 * the path to the blocks image
 * default: DEFAULT_BLOCK_FILE
 * Settings member: block_file
 */
#define PARAM_BLOCK_FILE "--block-file"

/*
 * the standard size for blocks (width == height)
 * default: DEFAULT_BLOCK_SIZE, min: 1
 * Settings member: block_size
 */
#define PARAM_BLOCK_SIZE "--block-size"

/*
 * the number of blocks per column (used when computing the window's height)
 * default: DEFAULT_BLOCKS_PER_COL, min: 8
 * Settings member: blocks_per_col
 */
#define PARAM_BLOCKS_PER_COL "--blocks-per-col"


/*
 * the number of blocks per row (used when computing the window's width)
 * default: DEFAULT_BLOCKS_PER_ROW, min: 8
 * Settings member: blocks_per_row
 */
#define PARAM_BLOCKS_PER_ROW "--blocks-per-row"

/*
 * the percentage of duration (ms) decrease
 * default: DEFAULT_DECREASE, min: 0, max: 99
 * Settings member: decrease
 */
#define PARAM_DECREASE "--decrease"

/*
 * the number of seconds before duration (ms) is decreased by decrease (%)
 * default: DEFAULT_DELAY, min: 1, max: 86400 (60*60*24)
 * Settings member: delay
 */
#define PARAM_DELAY "--delay"

/*
 * the number of milliseconds to wait before moving down a tetri
 * this number will decrease of decrease (%) after delay (s) or after threshold (number of rows)
 * default: DEFAULT_DURATION, min: 100, max: 10000
 * Settings member: duration
 */
#define PARAM_DURATION "--duration"

/*
 * the path to the font file to be used to display various informations
 * default: DEFAULT_FONT_FILE
 * Settings member: font_file
 */
#define PARAM_FONT_FILE "--font-file"

/*
 * the size of the font, pause text will be twice this size
 * default: DEFAULT_FONT_SIZE
 * Settings member: font_size
 */
#define PARAM_FONT_SIZE "--font-size"

/*
 * to decide if hints must be displayed or not (hints == time remaining until moving down)
 * default: DEFAULT_HINTS
 * Settings member: hints
 */
#define PARAM_NOHINTS "--nohints"

/*
 * to decide if key repeat must be enabled or not
 * default: DEFAULT_KEYREPEAT
 * Settings member: keyrepeat
 */
#define PARAM_NOKEYREPEAT "--nokeyrepeat"

/*
 * to decide if the next tetrimino must be displayed or not
 * default: DEFAULT_PREVIEW
 * Settings member: preview
 */
#define PARAM_NOPREVIEW "--nopreview"

/*
 * the message to display when the game is paused
 * default: DEFAULT_PAUSEMSG
 * Settings member: pausemsg
 */
#define PARAM_PAUSEMSG "--pausemsg"

/*
 * to decide if a new game must be started in case of a game over
 * default: DEFAULT_RESTART
 * Settings member: restart
 */
#define PARAM_RESTART "--restart"

/*
 * the number of rows to be randomly filled with blocks at the beginning
 * default: DEFAULT_ROWS, min: 0, max: DEFAULT_BLOCKS_PER_COL or blocks_per_col if already set
 * Settings member: rows
 */
#define PARAM_ROWS "--rows"

/*
 * the number of rows to be completed before duration (ms) is decreased by decrease (%)
 * default: DEFAULT_THRESHOLD, min: 1, max: 1000000
 * Settings member: threshold
 */
#define PARAM_THRESHOLD "--threshold"

/*
 * to decide if duration (ms) must be decreased after delay (s)
 * default: DEFAULT_USEDELAY
 * Settings member: usedelay
 */
#define PARAM_USEDELAY "--usedelay"

/*
 * the path to the window's icon file
 * default: DEFAULT_WINDOW_ICON
 * Settings member: window_icon
 */
#define PARAM_WINDOW_ICON "--window-icon"

/*
 * the window's title
 * default: DEFAULT_WINDOW_TITLE
 * Settings member: window_title
 */
#define PARAM_WINDOW_TITLE "--window-title"

/*
 * the Settings struct is used to store the user's game settings
 * they can me changed with command-line parameters
 * otherwise, they get default values
 *
 * before parsing command line arguments,
 * each item in the Settings struct is set to either NULL or -1 or undef (see constants.h)
 */

typedef struct {
	char *background_file;

	char *block_file;
	int block_size, preview_size; // preview_size cannot be set be the user!
	int blocks_per_col;
	int blocks_per_row;

	bool cheatmode; // if set to true, the player will be able to delete incomplete lines

	int decrease;
	int delay;
	int duration;

	char *font_file;
	int font_size;

	bool hints;

	bool keyrepeat;
	bool preview;

	char *pausemsg;

	bool restart;

	int rows;

	int threshold;
	bool usedelay;

	char *window_icon;
	char *window_title;

	bool leave; // if set to true, exit the program
} Settings;

/*
 * return a pointer to a Settings object
 * filled with default values or user-defined ones
 * the object need to be freed at the end
 * called by start_engine
 */
Settings* parse_params(int argc, char **argv);

#endif
