/*
 * pieces.h
 *
 * Created: 15/07/2021 20:34:22
 *  Author: Jan
 */ 


#ifndef PIECES_H_
#define PIECES_H_

extern int enum_char_pieces[];

// empty = 0, pawn = 1, kight = 2, bishop = 4, rook = 8, queen = 16, king = 32, black = 64, white = 128;
/*
empty =     0000 0000
white =     0000 0001
black =     0000 0010
pawn =      0000 0100
knight =    0000 1000
bishop =    0001 0000
rook =      0010 0000
queen =     0100 0000
king =      1000 0000
*/
enum {
	empty = 0,
	white = 1,
	black = 2,
	pawn = 4,
	knight = 8,
	bishop = 16,
	rook = 32,
	queen = 64,
	king = 128
};

enum {
	w_pawn = white + pawn,
	w_knight = white + knight,
	w_bishop = white + bishop,
	w_rook = white + rook,
	w_queen = white + queen,
	w_king = white + king,
	b_pawn = black + pawn,
	b_knight = black + knight,
	b_bishop = black + bishop,
	b_rook = black + rook,
	b_queen = black + queen,
	b_king = black + king
};


int decode_piece(int piece);


#endif /* PIECES_H_ */