
/*
 * blockmatic.c
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

#include <stdio.h>
#include <stdlib.h>

#include "tetri.h"
#include "param.h"
#include "engine.h"
#include "grid.h"

int main(int argc, char **argv) {

	const Settings* settings = start_engine(argc, argv);
	if(settings->leave) {
		return EXIT_FAILURE;
	}
	atexit(stop_engine);

	// needed to control the framerate
	uint32_t last_time_refresh = get_ms(), current_time;
	uint32_t delay_until_refresh = 1000 / GAME_FRAMERATE;

	// wait settings->duration ms before moving down, this number will decrease
	uint32_t last_time_movedown = last_time_refresh;
	uint32_t delay_until_movedown = (uint32_t)settings->duration;

	// wait settings->delay ms before decreasing delay, if settings->usedelay
	uint32_t last_time_decrease = last_time_refresh;
	uint32_t delay_until_decrease = (uint32_t)settings->delay;

	// if keypause, the game was paused because P was pressed
	bool pause = false, keypause = false;

	// if recycle / movedown, require a new tetrimino / move down current tetrimino, etc.
	bool recycle = false, movedown = false, newgame = true;

	// the number of completed rows and the level
	int completed_rows = 0, level = 1, level_rows = 0;


	Tetri next = new_random_tetri(settings), tetri = new_random_tetri(settings);
	while(!settings->leave) {
		current_time = get_ms();

		if(current_time - last_time_movedown >= delay_until_movedown) {
			last_time_movedown = current_time;
			movedown = true;
		}

		if(current_time >= (last_time_refresh + delay_until_refresh)) {
			last_time_refresh = current_time;

			// care about events
			const bool* events = receive_events();
			if(events[EXIT_EVENT]) {
				trigger_exit();
				continue;
			}

			if(events[PAUSE_EVENT]) {
				pause = !pause;
				keypause = !keypause;
			}

			if(events[FOCUSLOST_EVENT]) {
				pause = true;
			}

			if(events[FOCUSGAINED_EVENT] && !keypause) {
				pause = false;
			}

			if(events[NEWGAME_EVENT]) {
				newgame = true;
			}

			if(!pause && !movedown && !newgame) {
				if(events[LEFT_EVENT]) {
					move_tetri(&tetri, LEFT_MOVE);
				}

				if(events[RIGHT_EVENT]) {
					move_tetri(&tetri, RIGHT_MOVE);
				}

				if(events[DELETE_EVENT]) {
					if(!empty_row(settings->blocks_per_col - 1)) {
						shift_grid(settings->blocks_per_col-1);
						completed_rows++;
					}
				}

				if(events[DROP_EVENT]) {
					while(move_tetri(&tetri, DOWN_MOVE));
					recycle = true;
				}

				if(events[ROTATE_CLOCKWS_EVENT]) {
					rotate_tetri(&tetri, CLOCKWISE_ROTATION, settings);
				}

				if(events[ROTATE_COUNTERCLOCKWS_EVENT]) {
					rotate_tetri(&tetri, COUNTERCLOCKWISE_ROTATION, settings);
				}

				if(events[SHIFT_EVENT]) {
					if(!move_tetri(&tetri, DOWN_MOVE)) {
						recycle = true;
					}
				}
			} else if(!pause && !newgame) {
				if(!move_tetri(&tetri, DOWN_MOVE)) {
					recycle = true;
				}

				movedown = false;
			}

			// detect complete rows
			if(!pause && !newgame) {
				int complete;
				while((complete = complete_line()) != -1) {
					shift_grid(complete);
					completed_rows++;
				}
			}

			// recycle?
			if(recycle) {
				freeze_tetri(&tetri);
				tetri = next;
				next = new_random_tetri(settings);

				if(!valid_position(&tetri)) {
					if(settings->restart) {
						newgame = true;
					} else {
						trigger_exit();
						continue;
					}
				}

				last_time_movedown = current_time;

				recycle = false;
			}

			// new game?
			if(newgame) {
				tetri = new_random_tetri(settings);
				next = new_random_tetri(settings);
				erase_grid();

				int start_row = settings->blocks_per_col - settings->rows;
				for(int row = start_row; row < settings->blocks_per_col; row++) {
					fill_row(row);
				}

				last_time_movedown = current_time;
				delay_until_movedown = (uint32_t)settings->duration;
				completed_rows = 0;
				level = 1;

				newgame = false;
				pause = false;
			}

			// next level?
			if(settings->usedelay && !pause) {
				if(current_time - last_time_decrease >= (delay_until_decrease * 1000)) {
					level++;

					uint32_t drawback = (delay_until_movedown / 100) * (uint32_t)settings->decrease;
					delay_until_movedown -= drawback;

					last_time_decrease = current_time;
					last_time_movedown = current_time;
				}
			} else {
				if(completed_rows >= settings->threshold + level_rows) {
					level_rows += settings->threshold;
					level++;

					uint32_t drawback = (delay_until_movedown / 100) * (uint32_t)settings->decrease;
					delay_until_movedown -= drawback;

					last_time_movedown = current_time;
				}
			}

			// care about drawing
			clear_screen();

			draw_tetri(&tetri, 255);
			draw_grid();

			// hints about the tetri position when fallen
			if(settings->foresee_fallen && !pause) {
				Tetri fallen = find_fallen_position(&tetri);
				draw_tetri(&fallen, settings->fallen_opacity);
			}

			if(pause) {
				draw_pause();
			} else {
				if(settings->preview) {
					draw_preview(&next);
				}

				draw_statistics(level, completed_rows);

				if(settings->hints) {
					// compute percentage
					int incomplete_delay = (int)(current_time - last_time_movedown);
					int percentage = (incomplete_delay * 100) / (int)delay_until_movedown;

					draw_percentage(percentage);
				}
			}

			update_screen();
		} else {
			pause_engine((last_time_refresh + delay_until_refresh) - current_time);
		}
	}

	return EXIT_SUCCESS;
}
