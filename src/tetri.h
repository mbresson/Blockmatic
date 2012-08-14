
#ifndef H_TETRI
#define H_TETRI

#include "param.h"

typedef enum {
	/*
						O
		O				O
		X		OXOO		X		OOXO
		O				O
		O
	*/
	I_FORMAT = 0,

	/*
		OO
		OO
	*/
	O_FORMAT,
	
	/*
				O		 O		 O
		OXO		XO		OXO		OX
		 O		O				 O
	*/
	T_FORMAT,

	/*
		 O				OO		O
		 X		OXO		X		OXO
		OO		  O		O
	*/
	J_FORMAT,
	
	/*
		O		  O		OO
		X		OXO		 X		OXO
		OO				 O		O
	*/
	L_FORMAT,

	/*
				O		 OO		O
		 XO		XO		OX		OX
		OO		 O				 O

	*/
	S_FORMAT,

	/*
				 O		OO		 O
		OX		XO		 XO		OX
		 OO		O				O
	*/
	Z_FORMAT,

	__LAST_FORMAT
} Format;

typedef enum {
	TOP_ORIENTED,
	LEFT_ORIENTED,
	BOTTOM_ORIENTED,
	RIGHT_ORIENTED,

	__LAST_ORIENTED
} Orientation;

typedef struct {
	Format type;

	Orientation orientation;

	int px, py; // coordinates of the pivot block (X)

	int x[4], y[4]; // coordinates of each block relative to the pivot block,
	// x[0] and y[0] is the pivot block

	/*
	 * when speaking of coordinates,
	 * have in mind a grid of blocks_per_row * blocks_per_col blocks,
	 */
} Tetri;

typedef enum {
	CLOCKWISE_ROTATION,
	COUNTERCLOCKWISE_ROTATION
} Rotation;

typedef enum {
	LEFT_MOVE,
	RIGHT_MOVE,
	DOWN_MOVE
} Move;

/*
 * move tetri to the left or to the right
 * if movement is impossible and move == DOWN_MOVE, return false
 * else return true
 */
bool move_tetri(Tetri *tetri, Move mode);

/*
 * return a new Tetri object
 * type and orientation are random values
 */
Tetri new_random_tetri(const Settings *settings);

/*
 * rotate a shape clockwise/counter-clockwise
 * doesn't perform any check
 */
void rotate_tetri(Tetri *tetri, Rotation mode, const Settings *settings);

#endif
