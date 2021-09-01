/*
 * pieces.c
 *
 * Created: 15/07/2021 20:32:49
 *  Author: Jan
 */ 

#include "pieces.h"


int decode_piece(int piece)
{
	int number = 0;
	if (piece & black) number = 6;
	if (piece & pawn) return number += 1;
	if (piece & knight) return number += 2;
	if (piece & bishop) return number + 3;
	if (piece & rook) return number += 4;
	if (piece & queen) return number += 5;
	if (piece & king) return number += 6;
	return 0;
}


int enum_char_pieces[] = {
	['.'] = empty,
	['P'] = w_pawn,
	['N'] = w_knight,
	['B'] = w_bishop,
	['R'] = w_rook,
	['Q'] = w_queen,
	['K'] = w_king,
	['p'] = b_pawn,
	['n'] = b_knight,
	['b'] = b_bishop,
	['r'] = b_rook,
	['q'] = b_queen,
	['k'] = b_king
};