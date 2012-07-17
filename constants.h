
#ifndef H_CONSTANTS
#define H_CONSTANTS

#define GAME_TITLE "Blockmatic"
#define GAME_VERSION "0.4"
#define GAME_CREATOR "Bresson Matthieu"
#define GAME_FRAMERATE 30

#define BACKGROUND_RED 205
#define BACKGROUND_GREEN 205
#define BACKGROUND_BLUE 205

#define CHEATMODE_STRING "42"

typedef enum {
	undef = -1,
	false = 0,
	true = 1
} bool;

#define DEFAULT_BACKGROUND_FILE NULL
#define DEFAULT_BACKGROUND_CENTER false
#define DEFAULT_BACKGROUND_CROP false

#define DEFAULT_BACKGROUND_RED 205
#define DEFAULT_BACKGROUND_GREEN 205
#define DEFAULT_BACKGROUND_BLUE 205

#define DEFAULT_BLOCK_FILE "/usr/share/blockmatic/basic.png"
#define DEFAULT_BLOCK_SIZE 24
#define DEFAULT_BLOCKS_PER_COL 20
#define DEFAULT_BLOCKS_PER_ROW 14

#define DEFAULT_DECREASE 10 // %
#define DEFAULT_DELAY 60 // seconds
#define DEFAULT_DURATION 2000 // ms

#define DEFAULT_FONT_FILE "/usr/share/blockmatic/Cantarell-Bold.otf"
#define DEFAULT_FONT_SIZE 18

#define DEFAULT_FONT_RED 255
#define DEFAULT_FONT_GREEN 255
#define DEFAULT_FONT_BLUE 255

#define DEFAULT_HINTS true

#define DEFAULT_KEYREPEAT true

#define DEFAULT_PAUSE_MESSAGE "PAUSE"

#define DEFAULT_PAUSE_RED 100
#define DEFAULT_PAUSE_GREEN 100
#define DEFAULT_PAUSE_BLUE 100

#define DEFAULT_PREVIEW true

#define DEFAULT_RESTART false

#define DEFAULT_ROWS 0

#define DEFAULT_FORESEE_FALLEN false
#define DEFAULT_FALLEN_OPACITY 100

#define DEFAULT_THRESHOLD 10
#define DEFAULT_USEDELAY false

#define DEFAULT_WINDOW_ICON "/usr/share/blockmatic/icon.png"
#define DEFAULT_WINDOW_TITLE GAME_TITLE
#define DEFAULT_WINDOW_NOBORDER false

#endif
