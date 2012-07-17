
#ifndef H_GRID
#define H_GRID

#include "tetri.h"

typedef enum {
	EMPTY_CASE = 0,
	FILLED_CASE
} Case;


/*
 * return the number of the first complete line found
 * if no line is complete, return -1
 */
int complete_line(void);

/*
 * if there is no FILLED_CASE in row, return true
 * else return false
 */
bool empty_row(int row);

/*
 * set all grid case to EMPTY_CASE
 * used when restarting a game
 */
void erase_grid(void);

/*
 * fill row incompletely and randomly
 */
void fill_row(int row);

/*
 * find the position of the current tetri when it will have fallen
 */
Tetri find_fallen_position(Tetri *const tetri);

/*
 * free malloc-ed grid
 * called by start_engine
 */
void free_grid(void);

/*
 * append tetri to the grid
 */
void freeze_tetri(Tetri *tetri);

/*
 * return a pointer to s_grid
 */
const Case** get_grid(void);

/*
 * malloc enough memory to hold a grid of blocks_per_col * blocks_per_row int objects
 * return false if memory allocation failed
 */
bool init_grid(int blocks_per_col, int blocks_per_row);

/*
 * called when a line is complete
 * delete the line and push the other ones
 */
void shift_grid(int line);

/*
 * if tetri is (partly) out of the grid
 * or on a non-empty case,
 * return false
 */
bool valid_position(Tetri *tetri);

#endif
