
/*
 * engine.c
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

#include "engine.h"

#include "grid.h"
#include "debug.h"

#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/*
 * an array of events
 * each one is either true or false depending on the user input
 */
static bool s_events[__LAST_EVENT];

/*
 * an array containing the key code for each event
 */
static bool s_keys[__LAST_EVENT];

/*
 * this struct keeps in memory the main components of the game*
 * its window & renderer,
 * the graphics for the background and the blocks
 * the font to display informations on the screen
 */
static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *blocks, *background, *pause;
	SDL_Rect pausedst; // used to store the coordinates of the pause text

	TTF_Font *font;

	int width, height; // the window's width and height
} s_engine;

static Settings *s_settings;


void clear_screen(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);
	assert(s_engine.renderer != NULL);

	SDL_RenderClear(s_engine.renderer);

	if(s_engine.background != NULL) {
		if(s_settings->background_crop) {
			int background_width, background_height;
			SDL_QueryTexture(s_engine.background, NULL, NULL, &background_width, &background_height);

			SDL_Rect dest_rect;
			dest_rect.x = 0; dest_rect.y = 0;
			dest_rect.w = background_width; dest_rect.h = background_height;

			if(s_settings->background_center) {
				dest_rect.x = s_engine.width / 2 - dest_rect.w / 2;
				dest_rect.y = s_engine.height / 2 - dest_rect.h / 2;
			}

			SDL_RenderCopy(s_engine.renderer, s_engine.background, NULL, &dest_rect);
		} else { // stretch background texture
			SDL_RenderCopy(s_engine.renderer, s_engine.background, NULL, NULL);
		}
	}
}


void draw_grid(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);
	assert(s_engine.renderer != NULL);
	assert(s_engine.blocks != NULL);

	Colors color = GRAY;

	SDL_Rect src_rect;

	// we select the color
	src_rect.y = 0;
	src_rect.x = s_settings->block_size * (int)color;
	src_rect.w = s_settings->block_size; src_rect.h = s_settings->block_size;

	const Case** grid = get_grid();

	for(int y = 0; y < s_settings->blocks_per_col; y++) {
		for(int x = 0; x < s_settings->blocks_per_row; x++) {
			if(grid[y][x] == FILLED_CASE) {
				/*
				 * we must compute the real coordinates of each filled case
				 */

				int real_x = x * s_settings->block_size;
				int real_y = y * s_settings->block_size;

				SDL_Rect dest_rect;

				dest_rect.y = real_y; dest_rect.x = real_x;
				dest_rect.w = s_settings->block_size; dest_rect.h = s_settings->block_size;

				SDL_RenderCopy(s_engine.renderer, s_engine.blocks, &src_rect, &dest_rect);
			}
		}
	}
}


void draw_pause(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);
	assert(s_engine.renderer != NULL);

	SDL_SetRenderDrawColor(s_engine.renderer,
		(Uint8) s_settings->pause_color.red,
		(Uint8) s_settings->pause_color.green,
		(Uint8) s_settings->pause_color.blue, 255);

	SDL_RenderFillRect(s_engine.renderer, NULL);

	SDL_SetRenderDrawColor(s_engine.renderer,
		(Uint8) s_settings->background_color.red,
		(Uint8) s_settings->background_color.green,
		(Uint8) s_settings->background_color.blue, 255);

	// if pause_message is empty, no need to draw it
	if(s_engine.pause != NULL) {
		SDL_RenderCopy(s_engine.renderer, s_engine.pause, NULL, &(s_engine.pausedst));
	}

}


void draw_percentage(int value) {
	assert(s_settings != NULL);
	assert(s_engine.renderer != NULL);

	assert(value >= 0 && value <= 100);

	/* before displaying value, we need to write it in a complete string
	 * length: 5 because maximum value is 100 and 3 + 2 (% and \0) = 5
	 */
	char string[5];
	snprintf(string, 5, "%d%%", value);


	SDL_Color color = {
		(unsigned char) s_settings->font_color.red,
		(unsigned char) s_settings->font_color.green,
		(unsigned char) s_settings->font_color.blue, 0};

	SDL_Surface *stext = TTF_RenderText_Blended(s_engine.font, string, color);
	if(!stext) {
		fprintf(stderr, "Couldn't render '%s' (Blended)!\n=>\t%s\n", string, TTF_GetError());
	} else {
		SDL_Texture *text = SDL_CreateTextureFromSurface(s_engine.renderer, stext);

		SDL_Rect textdst;
		textdst.x = 10; textdst.y = 10;
		TTF_SizeText(s_engine.font, string, &textdst.w, &textdst.h);

		SDL_RenderCopy(s_engine.renderer, text, NULL, &textdst);

		SDL_DestroyTexture(text);
		SDL_FreeSurface(stext);
	}
}


/*
 * this function is used to compute how many characters are needed
 * to represent number in a char[]
 */
static int count_numberstr(int number) {
	assert(number >= 0);

	int count = 1;
	while((number /= 10) >= 1) {
		count++;
	}

	return count;
}


void draw_statistics(int level, int rows) {
	assert(s_settings != NULL);
	assert(s_engine.renderer != NULL);

	assert(level >= 0);
	assert(rows >= 0);

	/* before displaying value, we need to write it in a complete string
	 * we need to estimate how many characters we need to represent each number
	 * we will use VLA (Variable Length Array), which are optional in C11
	 */
	int chars_for_rows = count_numberstr(rows);
	int chars_for_level = count_numberstr(level);
	int total = chars_for_rows + chars_for_level + 3 + 1; // 3 for " ()", 1 for \0

	char string[total];
	snprintf(string, (size_t)total, "%d (%d)", level, rows);

	SDL_Color color = {
		(unsigned char) s_settings->font_color.red,
		(unsigned char) s_settings->font_color.green,
		(unsigned char) s_settings->font_color.blue, 0};

	SDL_Surface *stext = TTF_RenderText_Blended(s_engine.font, string, color);
	if(!stext) {
		fprintf(stderr, "Couldn't render '%s' (Blended)!\n=>\t%s\n", string, TTF_GetError());
	} else {
		SDL_Texture *text = SDL_CreateTextureFromSurface(s_engine.renderer, stext);

		SDL_Rect textdst;
		textdst.x = 10; textdst.y = s_engine.height - 10;

		TTF_SizeText(s_engine.font, string, &textdst.w, &textdst.h);
		textdst.y -= textdst.h; // to display the informations below the percentage

		SDL_RenderCopy(s_engine.renderer, text, NULL, &textdst);

		SDL_DestroyTexture(text);
		SDL_FreeSurface(stext);
	}

}


void draw_preview(Tetri *tetri) {
	assert(s_settings != NULL);
	assert(s_engine.renderer != NULL);
	assert(s_engine.blocks != NULL);

	Colors color = (Colors)tetri->type;

	SDL_Rect src_rect;

	// we select the color
	src_rect.y = 0;
	src_rect.x = s_settings->block_size * (int)color;
	src_rect.w = s_settings->block_size; src_rect.h = s_settings->block_size;

	int init_x = s_engine.width - (4 * s_settings->preview_size);
	int init_y = 2 * s_settings->preview_size;

	SDL_SetTextureAlphaMod(s_engine.blocks, 200);

	for(int index = 0; index < 4; index++) {
		/*
		 * we must compute the real coordinates of each block in the tetri
		 */

		int real_x = init_x + (tetri->x[index] * s_settings->preview_size);
		int real_y = init_y + (tetri->y[index] * s_settings->preview_size);

		SDL_Rect dest_rect;

		dest_rect.y = real_y; dest_rect.x = real_x;
		dest_rect.w = s_settings->preview_size; dest_rect.h = s_settings->preview_size;

		SDL_RenderCopy(s_engine.renderer, s_engine.blocks, &src_rect, &dest_rect);
	}

	SDL_SetTextureAlphaMod(s_engine.blocks, 255);
}


void draw_tetri(Tetri *tetri, int opacity) {
	assert(s_settings != NULL);
	assert(s_engine.renderer != NULL);
	assert(s_engine.blocks != NULL);
	assert(opacity >= 0); assert(opacity < 256);

	SDL_SetTextureAlphaMod(s_engine.blocks, (Uint8) opacity);

	Colors color = (Colors)tetri->type;

	SDL_Rect src_rect;

	// we select the color
	src_rect.y = 0;
	src_rect.x = s_settings->block_size * (int)color;
	src_rect.w = s_settings->block_size; src_rect.h = s_settings->block_size;

	for(int index = 0; index < 4; index++) {
		/*
		 * we must compute the real coordinates of each block in the tetri
		 */

		int real_x = (tetri->px + tetri->x[index]) * s_settings->block_size;
		int real_y = (tetri->py + tetri->y[index]) * s_settings->block_size;

		SDL_Rect dest_rect;

		dest_rect.y = real_y; dest_rect.x = real_x;
		dest_rect.w = s_settings->block_size; dest_rect.h = s_settings->block_size;

		SDL_RenderCopy(s_engine.renderer, s_engine.blocks, &src_rect, &dest_rect);
	}

	// we reset the texture's opacity to its max value
	SDL_SetTextureAlphaMod(s_engine.blocks, 255);
}


uint32_t get_ms(void) {
	assert(s_settings != NULL);

	return SDL_GetTicks();
}


void pause_engine(uint32_t ms) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);
	assert(s_engine.renderer != NULL);

	SDL_Delay(ms);
}


static inline void reset_events(void) {
	for(int index = 0; index < __LAST_EVENT; index++) {
		s_events[index] = false;
	}
}


const bool* receive_events(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);
	assert(s_settings->keyrepeat != undef);

	reset_events();

	SDL_Event event;
	bool kshift = false; // if a shift key is pressed

	SDL_Keymod mods = SDL_GetModState();
	if(mods & KMOD_SHIFT) {
		kshift = true;
	}

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_KEYDOWN:
				;
				SDL_Keycode code = event.key.keysym.sym;

				if(code == s_keys[EXIT_EVENT]) {
					s_events[EXIT_EVENT] = true;
					return s_events; // no need to continue polling
				}

				if(code == s_keys[LEFT_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							s_events[LEFT_EVENT] = true;
						}
					} else {
						s_events[LEFT_EVENT] = true;
					}
				}

				if(code == s_keys[RIGHT_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							s_events[RIGHT_EVENT] = true;
						}
					} else {
						s_events[RIGHT_EVENT] = true;
					}
				}

				if(code == s_keys[ROTATE_CLOCKWS_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							if(kshift) {
								s_events[SHIFT_EVENT] = true;
							} else {
								s_events[ROTATE_CLOCKWS_EVENT] = true;
							}
						}
					} else {
						if(kshift && !s_settings->vi_mode) {
							s_events[SHIFT_EVENT] = true;
						} else {
							s_events[ROTATE_CLOCKWS_EVENT] = true;
						}
					}
				}

				if(s_settings->vi_mode && code == s_keys[SHIFT_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							s_events[SHIFT_EVENT] = true;
						}
					} else {
						s_events[SHIFT_EVENT] = true;
					}
				}

				if(code == s_keys[ROTATE_COUNTERCLOCKWS_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							s_events[ROTATE_COUNTERCLOCKWS_EVENT] = true;
						}
					} else {
						s_events[ROTATE_COUNTERCLOCKWS_EVENT] = true;
					}
				}

				if(code == s_keys[PAUSE_EVENT]) {
					s_events[PAUSE_EVENT] = true;
				}

				if(code == s_keys[NEWGAME_EVENT]) {
					s_events[NEWGAME_EVENT] = true;
				}

				if(code == s_keys[DROP_EVENT]) {
					if(event.key.repeat) {
						if(s_settings->keyrepeat) {
							s_events[DROP_EVENT] = true;
						}
					} else {
						s_events[DROP_EVENT] = true;
					}
				}

				if(code == s_keys[DELETE_EVENT]) {
					if(s_settings->cheatmode) {
						if(event.key.repeat) {
							if(s_settings->keyrepeat) {
								s_events[DELETE_EVENT] = true;
							}
						} else {
							s_events[DELETE_EVENT] = true;
						}
					}
				}
				break;

		case SDL_QUIT:
			s_events[EXIT_EVENT] = true;
			return s_events; // no need to continue polling

		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
				s_events[FOCUSLOST_EVENT] = true;
			} else if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
				s_events[FOCUSGAINED_EVENT] = true;
			}
			break;
		}
	}

	return s_events;
}


const Settings* start_engine(int argc, char **argv) {
	s_settings = parse_params(argc, argv);
	assert(s_settings != NULL);

	if(s_settings->leave) {
		return s_settings;
	}

	/*
	 * init random-ness, required for random tetriminos
	 */

	srand((unsigned int) time(NULL));

	/*
	 * allocate the whole grid
	 * check if it's all right
	 */
	
	if(!init_grid(s_settings->blocks_per_col, s_settings->blocks_per_row)) {
		s_settings->leave = true;
		return s_settings;
	}

	/*
	 * initialize SDL, SDL_image, SDL_TTF
	 */

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr," Couldn't initialize SDL2!\n=>\t%s\n", SDL_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
		fprintf(stderr, "Couldn't initialize SDL2 image!\n=>\t%s\n", IMG_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	if(TTF_Init() != 0) {
		fprintf(stderr, "Couldn't initialize SDL2 ttf!\n=>\t%s\n", TTF_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	/*
	 * create main window, then create main window's renderer
	 */

	assert(s_settings->window_title != NULL);

	int width = s_settings->block_size * s_settings->blocks_per_row;
	int height = s_settings->block_size * s_settings->blocks_per_col;

	SDL_WindowFlags flags = 0;
	if(s_settings->window_noborder) {
		flags = SDL_WINDOW_BORDERLESS;
	}

	s_engine.window = SDL_CreateWindow(s_settings->window_title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, flags);
	if(!s_engine.window) {
		fprintf(stderr, "Couldn't create a %dx%d window!\n=>\t%s\n", width, height, SDL_GetError()); 
		s_settings->leave = true;
		return s_settings;
	}

	s_engine.renderer = SDL_CreateRenderer(s_engine.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!s_engine.renderer) {
		fprintf(stderr, "Couldn't create a renderer for %dx%d window!\n=>\t%s\n", width, height, SDL_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	// we store width and height in s_engine
	s_engine.width = width;
	s_engine.height = height;

	// set a drawing color to the renderer
	SDL_SetRenderDrawColor(s_engine.renderer,
		(Uint8) s_settings->background_color.red,
		(Uint8) s_settings->background_color.green,
		(Uint8) s_settings->background_color.blue, 255);

	// set a blendmode to the renderer
	SDL_SetRenderDrawBlendMode(s_engine.renderer, SDL_BLENDMODE_MOD);

	/*
	 * load the icon file
	 */

	assert(s_settings->window_icon != NULL);

	SDL_Surface *icon = IMG_Load(s_settings->window_icon);
	if(!icon) {
		fprintf(stderr, "Couldn't load icon file '%s'!\n=>\t%s\n", s_settings->window_icon, IMG_GetError());
		SDL_ClearError();
	} else {
		SDL_SetWindowIcon(s_engine.window, icon);
		SDL_FreeSurface(icon);
	}

	/*
	 * load background image file
	 */

	if(s_settings->background_file != NULL) {
		s_engine.background = IMG_LoadTexture(s_engine.renderer, s_settings->background_file);
		if(!s_engine.background) {
			fprintf(stderr, "Couldn't load image file '%s'!\n=>\t%s\n", s_settings->background_file, IMG_GetError());
			// we can do without a background image, so leaving the game isn't mandatory

			SDL_ClearError();
		}
	} // we don't need to set s_engine.background to NULL, it is already NULL

	/*
	 * load block image file and retrieve each colored element
	 * check this image file has the right width and height
	 */

	assert(s_settings->block_file != NULL);

	s_engine.blocks = IMG_LoadTexture(s_engine.renderer, s_settings->block_file);
	if(!s_engine.blocks) {
		fprintf(stderr, "Couldn't load image file '%s'!\n=>\t%s\n", s_settings->block_file, IMG_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	int legal_width = s_settings->block_size * __LAST_COLOR;
	int legal_height = s_settings->block_size;

	int texture_width = 0, texture_height;
	SDL_QueryTexture(s_engine.blocks, NULL, NULL, &texture_width, &texture_height);
	if(texture_width != legal_width || texture_height != legal_height) {
		fprintf(stderr, "Texture for '%s' is not the right size (%dx%d, should be %dx%d)!\n",
			s_settings->block_file, texture_width, texture_height, legal_width, legal_height);
	}

	/*
	 * load the font file
	 */

	assert(s_settings->font_file != NULL);
	assert(s_settings->font_size > 0);

	s_engine.font = TTF_OpenFont(s_settings->font_file, s_settings->font_size);
	if(!s_engine.font) {
		fprintf(stderr, "Couldn't load font file '%s' (%d px)!\n=>\t%s\n",
			s_settings->font_file, s_settings->font_size, TTF_GetError());
		s_settings->leave = true;
		return s_settings;
	}

	/*
	 * store the rendered pause text in an SDL_Texture for later use
	 */

	TTF_Font *pause_font = TTF_OpenFont(s_settings->font_file, s_settings->font_size * 2);
	if(!pause_font) {
		fprintf(stderr, "Couldn't load font file '%s' (%d px)!\n=>\t%s\n",
			s_settings->font_file, s_settings->font_size, TTF_GetError());
		s_settings->leave = true;
		return s_settings;
	} else {
		SDL_Color color = {
			(unsigned char) s_settings->font_color.red,
			(unsigned char) s_settings->font_color.green,
			(unsigned char) s_settings->font_color.blue, 0};

		SDL_Surface *spause = TTF_RenderUTF8_Blended(pause_font, s_settings->pause_message, color);
		if(!spause) {
			fprintf(stderr, "Couldn't render '%s' (Blended)!\n=>\t%s\n", s_settings->pause_message, TTF_GetError());
			// no need to exit, the pause_message will simply not be displayed
		} else {
			s_engine.pause = SDL_CreateTextureFromSurface(s_engine.renderer, spause);

			// compute the coordinates of the pause text (the center of the window)
			int center_x = s_engine.width / 2;
			int center_y = s_engine.height / 2;

			int text_width, text_height;
			TTF_SizeText(pause_font, s_settings->pause_message, &text_width, &text_height);

			s_engine.pausedst.w = text_width;
			s_engine.pausedst.h = text_height;

			s_engine.pausedst.x = center_x - text_width/2;
			s_engine.pausedst.y = center_y - text_height/2;

			SDL_FreeSurface(spause);
		}

		TTF_CloseFont(pause_font); pause_font = NULL;
	}

	/*
	 * set the keycodes
	 */

	// universal key controls
	s_keys[DELETE_EVENT] = SDLK_d;
	s_keys[DROP_EVENT] = SDLK_SPACE;
	s_keys[EXIT_EVENT] = SDLK_ESCAPE;
	s_keys[PAUSE_EVENT] = SDLK_p;

	if(s_settings->vi_mode) {
		s_keys[LEFT_EVENT] = SDLK_s;
		s_keys[RIGHT_EVENT] = SDLK_l;
		s_keys[NEWGAME_EVENT] = SDLK_RETURN;
		s_keys[ROTATE_CLOCKWS_EVENT] = SDLK_f;
		s_keys[ROTATE_COUNTERCLOCKWS_EVENT] = SDLK_j;
		s_keys[SHIFT_EVENT] = SDLK_k;
	} else {
		s_keys[LEFT_EVENT] = SDLK_LEFT;
		s_keys[RIGHT_EVENT] = SDLK_RIGHT;
		s_keys[NEWGAME_EVENT] = SDLK_n;
		s_keys[ROTATE_CLOCKWS_EVENT] = SDLK_DOWN;
		s_keys[ROTATE_COUNTERCLOCKWS_EVENT] = SDLK_UP;
		s_keys[SHIFT_EVENT] = SDLK_DOWN;
	}

	return s_settings;
}


void stop_engine(void) {

	/*
	 * free grid memory
	 */
	
	free_grid();

	/*
	 * free loaded fonts
	 */

	assert(s_engine.font != NULL);

	TTF_CloseFont(s_engine.font); s_engine.font = NULL;

	/*
	 * free loaded blocks image file and background image file
	 */

	assert(s_engine.blocks != NULL);
	SDL_DestroyTexture(s_engine.blocks); s_engine.blocks = NULL;

	if(s_engine.background != NULL) {
		SDL_DestroyTexture(s_engine.background); s_engine.background = NULL;
	}

	/*
	 * destroy the main window and its renderer
	 */

	assert(s_engine.window != NULL);
	assert(s_engine.renderer != NULL);

	SDL_DestroyRenderer(s_engine.renderer); s_engine.renderer = NULL;
	SDL_DestroyWindow(s_engine.window); s_engine.window = NULL;

	/*
	 * quit SDL_ttf, SDL_image and SDL
	 */

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	/*
	 * unset s_settings
	 */
	free(s_settings);
	s_settings = NULL;
}

void trigger_exit(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);

	s_settings->leave = true;
}

void update_screen(void) {
	assert(s_settings != NULL);
	assert(!s_settings->leave);

	assert(s_engine.window != NULL);
	assert(s_engine.renderer != NULL);

	SDL_RenderPresent(s_engine.renderer);
}

