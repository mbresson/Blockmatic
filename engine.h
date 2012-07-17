
#ifndef H_ENGINE
#define H_ENGINE

#include "tetri.h"
#include "grid.h"
#include "param.h"

#include <limits.h>
#include <stdint.h>

typedef enum {
	DELETE_EVENT,
	DROP_EVENT,
	EXIT_EVENT,
	FOCUSGAINED_EVENT,
	FOCUSLOST_EVENT,
	LEFT_EVENT,
	NEWGAME_EVENT,
	PAUSE_EVENT,
	RIGHT_EVENT,
	ROTATE_CLOCKWS_EVENT,
	ROTATE_COUNTERCLOCKWS_EVENT,
	SHIFT_EVENT,
	__LAST_EVENT
} Event;

typedef enum {
	YELLOW = 0,
	RED,
	GREEN,
	BLUE,
	PURPLE,
	BROWN,
	ORANGE,
	GRAY,
	__LAST_COLOR
} Colors;


/*
 * fill screen with a color or a background image if it is loaded
 */
void clear_screen(void);

/*
 * draw a preview of next tetri
 */
void draw_preview(Tetri *tetri);

/*
 * draw the entire grid
 */
void draw_grid(void);

/*
 * darken the screen and display the pause text
 */
void draw_pause(void);

/*
 * display value at the top left corner of the screen
 */
void draw_percentage(int value);

/*
 * display "rows / level"
 * below the percentage
 */
void draw_statistics(int level, int rows);

/*
 * draw a tetri object
 * opacity must be comprised between 0 and 255
 */
void draw_tetri(Tetri *tetri, int opacity);

/*
 * get the number of milliseconds since the engine is started
 */
uint32_t get_ms(void);

/*
 * pause the game during ms microseconds
 * this is not a user-triggered pause!
 */
void pause_engine(uint32_t ms);

/*
 * check for user input (key pressed, button clicked)
 * then update the array of events
 * return the address of a static array of events
 * if no event was received, return NULL
 */
const bool* receive_events(void);

/*
 * create a window, open needed files
 */
const Settings* start_engine(int argc, char **argv);

/*
 * destroy main window, leave game, free memory
 * atexit(stop_engine)
 */
void stop_engine(void);

/*
 * set settings->leave to true
 */
void trigger_exit(void);

/*
 * update screen
 */
void update_screen(void);

#endif
