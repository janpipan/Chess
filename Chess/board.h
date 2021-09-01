/*
 * board.h
 *
 * Created: 15/07/2021 20:34:35
 *  Author: Jan
 */ 


#ifndef BOARD_H_
#define BOARD_H_

#include "pieces.h"
#include "LCD_Ili9341.h"
#include "stdlib.h"
#include <string.h>

#define COLOR_SQUARE_1 C_DODGER_BLUE
#define COLOR_SQUARE_2 C_SKY_BLUE

/*
	EXTERN VARIABLES
*/

extern int board[128];
extern int on_move;
extern int castling_rights;
extern int enpassant;
extern int half_move;
extern int full_move;
extern int king_position[2];
extern int check[2];
extern char FEN_string[90];


void print_board(int *board);
void empty_board(void); 
void parse_FEN(char *string);
void make_FEN(int *board);
void draw_pawn(int square_number, UG_COLOR color);
void draw_king(int square_number, UG_COLOR color);
void draw_bishop(int square_number, UG_COLOR color);
void draw_knight(int square_number, UG_COLOR color);
void draw_queen(int square_number, UG_COLOR color);
void draw_rook(int square_number, UG_COLOR color);
void draw_empty(int square_number, UG_COLOR color);


#endif /* BOARD_H_ */