/*
 * display.h
 *
 * Created: 22/07/2021 17:02:37
 *  Author: Jan
 */ 



#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "LCD_Ili9341.h"
#include "board.h"
#include "moves.h"

void display_welcome_window(void);
void display_selected_square(int square);
void display_clear_square(int square);
void display_possible_moves(int *moves, UG_COLOR color);
void display_clear_possible_moves(int *moves);
void display_pawn_promotion_select(int on_move);
void display_clear_pawn_promotion_select(void);
void display_chess_logo(void);
void display_gameover_screen(void);
void display_result(int game_state);



#endif /* DISPLAY_H_ */