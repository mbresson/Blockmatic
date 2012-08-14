
/*
 * tetri.c
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

#include "tetri.h"

#include "engine.h"
#include "param.h"
#include "debug.h"

#include <stdlib.h>


// min <= result < max
static inline int get_random_in_range(int min, int max) {
	assert(min < max);

	return (rand() % max + min);
}


// long, painful function!
static void place_tetri(Tetri *tetri, const Settings *settings) {
	assert(tetri != NULL);
	assert(settings != NULL);

	tetri->px = settings->blocks_per_row / 2;

	/*
	 * here, we hard-code the coordinates of each block in a tetri
	 * theses coordinates are relative to the pivot block of each tetri
	 * this pivot block is marked X, the other blocks are marked O
	 */
	
	switch(tetri->type) {
	case I_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				O
				X
				O
				O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 1;
			tetri->x[3] = 0; tetri->y[3] = 2;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				OXOO
			*/

			tetri->x[0] = -1; tetri->y[0] = 0;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 2; tetri->y[3] = 0;

			tetri->py = 0;
			break;

		case BOTTOM_ORIENTED:
			/*
				O
				O
				X
				O
			*/

			tetri->x[0] = 0; tetri->y[0] = -2;
			tetri->x[1] = 0; tetri->y[1] = -1;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 2;
			break;

		case RIGHT_ORIENTED:
			/*
				OOXO
			*/

			tetri->x[0] = -2; tetri->y[0] = 0;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 0;

			tetri->py = 0;

			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	case O_FORMAT:
		/*
			OX
			OO
		*/

		// no rotation
		tetri->x[0] = -1; tetri->y[0] = 0;
		tetri->x[1] = 0; tetri->y[1] = 0;
		tetri->x[2] = -1; tetri->y[2] = 1;
		tetri->x[3] = 0; tetri->y[3] = 1;

		tetri->py = 0;
		break;
	
	case T_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				 O
				OXO
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 0;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				 O
				OX
				 O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case BOTTOM_ORIENTED:
			/*
				OXO
				 O
			*/

			tetri->x[0] = -1; tetri->y[0] = 0;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 0;
			break;

		case RIGHT_ORIENTED:
			/*
				O
				XO
				O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	case J_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				OO
				X
				O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 1; tetri->y[1] = -1;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				O
				OXO
			*/

			tetri->x[0] = -1; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 0;

			tetri->py = 1;
			break;

		case BOTTOM_ORIENTED:
			/*
				 O
				 X
				OO
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = -1; tetri->y[2] = 1;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case RIGHT_ORIENTED:
			/*
				OXO
				  O
			*/

			tetri->x[0] = -1; tetri->y[0] = 0;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 1;

			tetri->py = 0;
			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	case L_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				OO
				 X
				 O
			*/

			tetri->x[0] = -1; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = -1;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				OXO
				O
			*/

			tetri->x[0] = -1; tetri->y[0] = 0;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = -1; tetri->y[3] = 1;

			tetri->py = 0;
			break;

		case BOTTOM_ORIENTED:
			/*
				O
				X
				OO
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 1;
			tetri->x[3] = 1; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case RIGHT_ORIENTED:
			/*
				  O
				OXO
			*/

			tetri->x[0] = 1; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 0;

			tetri->py = 1;
			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	case S_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				O
				XO
				 O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				 OO
				OX
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = 1; tetri->y[1] = -1;
			tetri->x[2] = -1; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 0;

			tetri->py = 1;
			break;

		case BOTTOM_ORIENTED:
			/*
				O
				OX
				 O
			*/

			tetri->x[0] = -1; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case RIGHT_ORIENTED:
			/*
				 XO
				OO
			*/

			tetri->x[0] = 0; tetri->y[0] = 0;
			tetri->x[1] = 1; tetri->y[1] = 0;
			tetri->x[2] = -1; tetri->y[2] = 1;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 0;
			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	case Z_FORMAT:
		switch(tetri->orientation) {
		case TOP_ORIENTED:
			/*
				 O
				OX
				O
			*/

			tetri->x[0] = 0; tetri->y[0] = -1;
			tetri->x[1] = -1; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = -1; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case LEFT_ORIENTED:
			/*
				OX
				 OO
			*/

			tetri->x[0] = -1; tetri->y[0] = 0;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 0; tetri->y[2] = 1;
			tetri->x[3] = 1; tetri->y[3] = 1;

			tetri->py = 0;
			break;

		case BOTTOM_ORIENTED:
			/*
				 O
				XO
				O
			*/

			tetri->x[0] = 1; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = 0;
			tetri->x[2] = 1; tetri->y[2] = 0;
			tetri->x[3] = 0; tetri->y[3] = 1;

			tetri->py = 1;
			break;

		case RIGHT_ORIENTED:
			/*
				OO
				 XO
			*/

			tetri->x[0] = -1; tetri->y[0] = -1;
			tetri->x[1] = 0; tetri->y[1] = -1;
			tetri->x[2] = 0; tetri->y[2] = 0;
			tetri->x[3] = 1; tetri->y[3] = 0;

			tetri->py = 1;
			break;

		default:
			assert(tetri->orientation != __LAST_ORIENTED); // WILL FAIL!
		}
		break;
	
	default:
		assert(tetri->type != __LAST_FORMAT); // WILL FAIL
	}
}


bool move_tetri(Tetri *tetri, Move mode) {
	assert(tetri != NULL);

	Tetri copy = *tetri;

	switch(mode) {
	case LEFT_MOVE:
		copy.px--;
		break;
	
	case RIGHT_MOVE:
		copy.px++;
		break;
	
	case DOWN_MOVE:
		copy.py++;
		break;
	}

	if(valid_position(&copy)) {
		*tetri = copy;
	} else if(mode == DOWN_MOVE) {
		return false;
	}

	return true;
}


Tetri new_random_tetri(const Settings *settings) {
	assert(settings != NULL);
	assert(settings->blocks_per_row > 0);

	Tetri tmp;

	tmp.type = get_random_in_range(I_FORMAT, __LAST_FORMAT);
	tmp.orientation = get_random_in_range(TOP_ORIENTED, __LAST_ORIENTED);
	tmp.px = settings->blocks_per_row / 2;

	place_tetri(&tmp, settings);

	return tmp;
}


void rotate_tetri(Tetri *tetri, Rotation mode, const Settings *settings) {
	assert(tetri != NULL);
	assert(settings != NULL);

	Tetri copy = *tetri;
	if(mode == CLOCKWISE_ROTATION) {
		if(copy.orientation == TOP_ORIENTED) {
			copy.orientation = __LAST_ORIENTED - 1;
		} else {
			copy.orientation--;
		}
	} else { // COUNTERCLOCKWISE_ROTATION
		if(copy.orientation == __LAST_ORIENTED - 1) {
			copy.orientation = TOP_ORIENTED;
		} else {
			copy.orientation++;
		}
	}

	place_tetri(&copy, settings);

	copy.px = tetri->px;
	copy.py = tetri->py;

	if(valid_position(&copy)) {
		*tetri = copy;
	}
}
