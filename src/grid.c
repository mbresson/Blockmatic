
/*
 * grid.c
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

#include "grid.h"

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * at runtime, this is a malloc-ed matrix
 * used to store the state of every case of the board
 */
static Case **s_grid;

static int s_blocks_per_col, s_blocks_per_row;


int complete_line(void) {
	assert(s_grid != NULL);

	for(int y = s_blocks_per_col - 1; y >= 0; y--) {
		int counter = 0;
		for(int x = s_blocks_per_row - 1; x >= 0; x--) {
			if(s_grid[y][x] == FILLED_CASE) {
				counter++;
			}
		}

		if(counter == s_blocks_per_row) {
			return y;
		}
	}

	return -1;
}


bool empty_row(int row) {
	assert(s_grid != NULL);
	assert(row >= 0 && row < s_blocks_per_col);

	bool empty = true;
	
	for(int x = 0; x < s_blocks_per_row; x++) {
		if(s_grid[row][x] == FILLED_CASE) {
			empty = false;
			break;
		}
	}

	return empty;
}


void erase_grid(void) {
	assert(s_grid != NULL);

	for(int y = 0; y < s_blocks_per_col; y++) {
		for(int x = 0; x < s_blocks_per_row; x++) {
			s_grid[y][x] = EMPTY_CASE;
		}
	}
}


void fill_row(int row) {
	assert(s_grid != NULL);
	assert(row > 3 && row < s_blocks_per_col);

	int filled_blocks = 0;

	for(int block = 0; block < s_blocks_per_row; block++) {
		if(rand() % 2 == 0) {
			if(filled_blocks < s_blocks_per_row-1) {
				filled_blocks++;
				s_grid[row][block] = FILLED_CASE;
			}
		}
	}
}


Tetri find_fallen_position(Tetri *const tetri) {
	assert(s_grid != NULL);
	assert(tetri != NULL);

	Tetri tmp = *tetri;

	while(valid_position(&tmp)) {
		tmp.py++;
	}

	// we've been one step too far
	tmp.py--;

	return tmp;
}


void free_grid(void) {
	assert(s_grid != NULL);

	for(int row = 0; row < s_blocks_per_col; row++) {
		assert(s_grid[row] != NULL);
		
		free(s_grid[row]);
	}

	free(s_grid);
}

void freeze_tetri(Tetri *tetri) {
	assert(tetri != NULL);
	assert(s_grid != NULL);

	for(int block = 0; block < 4; block++) {
		int tmp_x = tetri->x[block] + tetri->px;
		int tmp_y = tetri->y[block] + tetri->py;

		assert(tmp_x < s_blocks_per_row);
		assert(tmp_y < s_blocks_per_col);
		assert(s_grid[tmp_y] != NULL);

		s_grid[tmp_y][tmp_x] = FILLED_CASE;
	}
}


const Case** get_grid(void) {
	assert(s_grid != NULL);

	return (const Case**) s_grid;
}


/*
 * this function must be called after every other initialization has been made
 * first it mallocs each column as an array of pointers to arrays of Case
 * then it mallocs each of these arrays of Case
 */
bool init_grid(int blocks_per_col, int blocks_per_row) {
	assert(!s_grid);

	assert(blocks_per_col > 0);
	assert(blocks_per_row > 0);

	size_t size_of_col = sizeof(Case*);
	size_t size_of_row = sizeof(Case) * (size_t) blocks_per_row;

	s_grid = (Case**) malloc(size_of_col * (size_t) blocks_per_col);
	if(!s_grid) {
		fprintf(stderr, "Couldn't allocate %zu bytes!\n", size_of_col);
		return false;
	}

	for(int row = 0; row < blocks_per_col; row++) {
		s_grid[row] = (Case*) malloc(size_of_row);
		if(!s_grid[row]) {
			fprintf(stderr, "Couldn't allocate %zu bytes!\n", size_of_row);
			return false;
		}

		for(int row_case = 0; row_case < blocks_per_row; row_case++) {
			s_grid[row][row_case] = EMPTY_CASE;
		}
	}

	s_blocks_per_row = blocks_per_row;
	s_blocks_per_col = blocks_per_col;

	return true;
}


void shift_grid(int line) {
	assert(s_grid != NULL);
	assert(line > 0 && line < s_blocks_per_col);

	Case *last = s_grid[line];

	// from s_grid[line] to the upper lines
	for(int y = line; y > 0; y--) {
		s_grid[y] = s_grid[y - 1];
	}

	s_grid[0] = last;
	for(int x = 0; x < s_blocks_per_row; x++) {
		s_grid[0][x] = EMPTY_CASE;
	}
}


bool valid_position(Tetri *tetri) {
	assert(tetri != NULL);
	assert(s_grid != NULL);

	bool result = true;

	for(int block = 0; block < 4; block++) {
		int tmp_x = tetri->x[block] + tetri->px;
		int tmp_y = tetri->y[block] + tetri->py;

		// first, we check if tetri is out of the grid
		if(tmp_x < 0 || tmp_x >= s_blocks_per_row || tmp_y < 0 || tmp_y >= s_blocks_per_col) {
			result = false;
			break;
		}

		// then we check if there is a non-empty case at grid[tmp_y][tmp_x]
		assert(s_grid[tmp_y] != NULL);
		if(s_grid[tmp_y][tmp_x] == FILLED_CASE) {
			result = false;
			break;
		}
	}

	return result;
}
