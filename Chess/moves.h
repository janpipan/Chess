/*
 * moves.h
 *
 * Created: 15/07/2021 20:33:34
 *  Author: Jan
 */ 

#ifndef MOVES_H_
#define MOVES_H_

#include <stdlib.h>
#include <string.h>
#include "pieces.h"
#include "board.h"
#include "display.h"

// games states
enum {normal_move, check_move, check_mate, fifty_move_rule, stale_mate, insufficient_material};

int *sliding_moves(int *board, int square, int on_move);
int *knight_moves(int *board, int square, int on_move);
int *pawn_moves(int *board, int square, int on_move);
int *king_moves(int *board, int square);
int make_move(int square, int destination, int *possible_moves);
int *get_moves(int square, int on_move);
int try_move(int square, int destination, int on_move);
int moves_available(int side, int check, int king_position);
int is_square_attacked(int square, int side, int *board);
int *get_moves_in_check(int square, int side, int king_position);
int pawn_promotion(int promotion_square);
int is_draw(int *board);
int get_game_state(int king_position, int side, int *board);




#endif /* MOVES_H_ */