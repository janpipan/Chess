/*
 * display.c
 *
 * Created: 22/07/2021 17:02:27
 *  Author: Jan
 */ 

#include "display.h"


void display_welcome_window(void)
{
	UG_FillFrame(0,0,320, 240, C_BLACK);
	display_chess_logo();
	UG_PutString(135, 60, "CHESS");
	UG_PutString(120, 100, "A=two uC");
	UG_PutString(120, 120, "B=one uC");
}

void display_selected_square(int square)
{
	// calculates frame coords
	int x_pos = (square - ((square/16) * 16)) * 30;
	int y_pos = (square / 16) *  30 ;
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_RED);
}

void display_clear_square(int square)
{
	// calculates frame coords
	int x_pos = (square - ((square/16) * 16)) * 30 ;
	int y_pos = (square / 16) *  30;
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30, C_BLACK);
}

void display_possible_moves(int *moves, UG_COLOR color)
{
	for (int i = 0; moves[i] != -1; i++)
	{
		// calculates circle coords
		int x_pos = (moves[i] - ((moves[i]/16) * 16)) * 30 + 5;
		int y_pos = (moves[i] / 16) *  30 + 5;
		UG_FillCircle(x_pos,y_pos,2,color);
	}
}

void display_clear_possible_moves(int *moves)
{
	UG_COLOR color;
	for (int i = 0; moves[i] != -1; i++)
	{
		int x_pos = (moves[i] - ((moves[i]/16) * 16)) * 30 + 5;
		int y_pos = (moves[i] / 16) *  30 + 5; 
		// fills possible move circle with color of the square
		if (moves[i] % 2 == 0 && (moves[i] / 16) % 2 == 0) color = COLOR_SQUARE_2;
		else if (moves[i] % 2 == 1 && (moves[i] / 16) % 2 == 1) color = COLOR_SQUARE_2;
		else color = COLOR_SQUARE_1;
		UG_FillCircle(x_pos,y_pos,2,color);
	}
}

// displays pawn promotion tab in bottom right corner
void display_pawn_promotion_select(int on_move)
{
	UG_COLOR piece_color = C_WHITE;
	if (on_move) piece_color = C_BLACK;
	int x_pos = (73 - ((73 / 16) * 16)) * 30;
	int y_pos = (73 / 16) *  30;
	UG_FillFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,COLOR_SQUARE_1);
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_BLACK);
	draw_queen(73, piece_color);
	x_pos = (89 - ((89 / 16) * 16)) * 30;
	y_pos = (89 / 16) *  30;
	UG_FillFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,COLOR_SQUARE_2);
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_BLACK);
	draw_rook(89, piece_color);
	x_pos = (105 - ((105 / 16) * 16)) * 30;
	y_pos = (105 / 16) *  30;
	UG_FillFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,COLOR_SQUARE_1);
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_BLACK);
	draw_bishop(105, piece_color);
	x_pos = (121 - ((121 / 16) * 16)) * 30;
	y_pos = (121 / 16) *  30;
	UG_FillFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,COLOR_SQUARE_2);
	UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_BLACK);
	draw_knight(121, piece_color);
}

void display_clear_pawn_promotion_select(void)
{
	UG_FillFrame(270, 120, 300, 240, C_BLACK);
}

void display_chess_logo(void)
{
	UG_FillFrame(140, 48, 179, 53, C_WHITE);
	UG_FillFrame(144, 44, 175, 48, C_WHITE);
	UG_FillFrame(147, 40, 172, 44, C_WHITE);
	UG_FillFrame(149, 36, 175, 40, C_WHITE);
	UG_FillFrame(151, 32, 177, 36, C_WHITE);
	UG_FillFrame(153, 28, 177, 32, C_WHITE);
	UG_FillFrame(155, 23, 177, 28, C_WHITE);
	UG_FillFrame(149, 13, 175, 23, C_WHITE);
	UG_FillFrame(144, 15, 149, 20, C_WHITE);
	UG_FillFrame(157, 6, 161, 13, C_WHITE);
	UG_FillFrame(167, 6, 171, 13, C_WHITE);
}

void display_gameover_screen(void)
{
	UG_FillFrame(0,0,320,240, C_BLACK);
	UG_PutString(120, 70, "Game over!");
}

void display_result(int game_state)
{
	char txt[15];
	if (game_state == check_mate)
	{
		sprintf(txt, "%s wins", on_move ? "White" : "Black");
		UG_PutString(120,100, txt);
	}
	else  UG_PutString(145, 100, "Draw");
	UG_PutString(70, 220, "Press key to continue");
}