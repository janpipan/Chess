/*
 * board.c
 *
 * Created: 15/07/2021 20:32:18
 *  Author: Jan
 */ 

#include "board.h"


int board[128] = {
	b_rook, b_knight, b_bishop, b_queen, b_king, b_bishop, b_knight, b_rook,	  0, 0, 0, 0, 0, 0, 0, 0,
	b_pawn, b_pawn, b_pawn, b_pawn, b_pawn, b_pawn, b_pawn, b_pawn,				  0, 0, 0, 0, 0, 0, 0, 0,
	empty, empty, empty, empty, empty, empty, empty, empty,		                  0, 0, 0, 0, 0, 0, 0, 0,
	empty, empty, empty, empty, empty, empty, empty, empty,		                  0, 0, 0, 0, 0, 0, 0, 0,
	empty, empty, empty, empty, empty, empty, empty, empty,		                  0, 0, 0, 0, 0, 0, 0, 0,
	empty, empty, empty, empty, empty, empty, empty, empty,		                  0, 0, 0, 0, 0, 0, 0, 0,
	w_pawn, w_pawn, w_pawn, w_pawn, w_pawn, w_pawn, w_pawn, w_pawn, 			  0, 0, 0, 0, 0, 0, 0, 0,
	w_rook, w_knight, w_bishop, w_queen, w_king, w_bishop, w_knight, w_rook,	  0, 0, 0, 0, 0, 0, 0, 0,
};
// 0 = white, 1 = black
int on_move;
int castling_rights;
/*
white king side K	= 1000
white queen side Q	= 0100
black king side k	= 0010
black queen side q	= 0001
*/
// enpassant square
int enpassant;
// keeps track of half moves (if pawn has been moved or piece has been captured the counter resets)
int half_move;
// keeps track of moves increases on black move
int full_move;
// keeps track of king square
int king_position[2];
// keeps track if player is in check 0 = white, 1 = black
int check[2];
char FEN_string[90];
char char_pieces[] = ".PNBRQKpnbrqk";

void (*draw_pieces[13])() = {draw_empty, draw_pawn, draw_knight, draw_bishop, draw_rook, draw_queen, draw_king, 
	draw_pawn, draw_knight, draw_bishop, draw_rook, draw_queen, draw_king};


// prints board and pieces
void print_board(int *board)
{
	UG_COLOR square_color, color = C_WHITE;
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			// calculates square 
			int square = rank * 16 + file;
			// true if selected square is on board
			if (!(square & 0x88))
			{
				// calculates position of selected square for drawing
				int x_pos = (square - ((square/16) * 16)) * 30;
				int y_pos = (square / 16) *  30;
				
				// logic for coloring squares differently
				if (square % 2 == 0 && (square / 16) % 2 == 0) square_color = COLOR_SQUARE_2;
				else if (square % 2 == 1 && (square / 16) % 2 == 1) square_color = COLOR_SQUARE_2;
				else square_color = COLOR_SQUARE_1;
				
				// logic for coloring pieces differently
				if((board[square] & (white))) color = C_WHITE;
				if((board[square] & (black))) color = C_BLACK;
				
				// draw squares and pieces
				UG_FillFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,square_color);
				UG_DrawFrame(x_pos,y_pos, x_pos + 30 , y_pos + 30,C_BLACK);
				draw_pieces[decode_piece(board[square])](square, color);
			}
		}
	}
}

// loops through the board and fills the squares with empty spaces
void empty_board(void)
{
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			int square = rank * 16 + file;
			// set every square on the board to empty
			if (!(square & 0x88)) board[square] = empty;
		}
	}
}


void parse_FEN(char *string)
{
	empty_board();
	int square = 0;
	int index = 0;
	// loops through string piece placement
	for (; string[index] != ' '; index++)
	{
		if ((string[index] >= 'A' && string[index] <= 'Z') || (string[index] >= 'a' && string[index] <= 'z'))
		{
			if (string[index] == 'K') king_position[0] = square;
			if (string[index] == 'k') king_position[1] = square;
			board[square++] = enum_char_pieces[(int)string[index]];
		}
		if (string[index] > '0' && string[index] < '9') square += string[index] - '0';
		if (string[index] == '/') square += 8;

	}
	// space skip
	index++;
	// detects who is on move
	on_move = string[index] == 'w' ? 0 : 1;
	// space skip
	index += 2;
	// castling rights
	castling_rights = 0;
	if (string[index] != '-')
	{
		while (string[index] != ' ')
		{
			if (string[index] == 'K') castling_rights |= 8;
			if (string[index] == 'Q') castling_rights |= 4;
			if (string[index] == 'k') castling_rights |= 2;
			if (string[index] == 'q') castling_rights |= 1;
			index++;
		}
	}
	// space skip
	index++;
	// enpassant square
	enpassant = -1;
	if (string[index] != '-')
	{
		int file = string[index] - 'a';
		index++;
		int rank = 8 - (string[index] - '0');
		enpassant = rank * 16 + file;
	}
	// space skip
	index += 2;
	// halfmove clock
	char str1[2];
	for (int i = 0; string[index] != ' '; i++)
	{
		str1[i] = string[index];
		index++;
	}
	half_move = atoi(str1);
	// space skip
	index++;
	// fullmove clock
	char str2[2];
	for (int i = 0; string[index] != '\0'; i++)
	{
		str2[i] = string[index];
		index++;
	}
	full_move = atoi(str2);
}


void make_FEN(int *board)
{
	// rest FEN string
	FEN_string[0] = '\0';
	int empty_counter = 0;
	// loops through board
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			int square = rank * 16 + file;
			if (!(square & 0x88))
			{
				// count empty squares
				if (board[square] == empty)
				{
					empty_counter++;
				}
				// add piece to string
				else
				{
					// if counter is greater than 0 add number before the piece
					if (empty_counter) sprintf(FEN_string, "%s%d", FEN_string, empty_counter);
					sprintf(FEN_string, "%s%c", FEN_string, char_pieces[decode_piece(board[square])]);
					empty_counter = 0;
				}
			}
		}// at the end of rank add / and reset empty counter
		if (rank < 7)
		{
			if (empty_counter) sprintf(FEN_string, "%s%d", FEN_string, empty_counter);
			sprintf(FEN_string, "%s/", FEN_string);
			empty_counter = 0;
		}

	}
	//side
	sprintf(FEN_string, "%s %s", FEN_string, on_move ? "b" : "w");
	// castling
	if (castling_rights) sprintf(FEN_string, "%s %s%s%s%s", FEN_string,	(castling_rights & 8) ? "K" : "",
																		(castling_rights & 4) ? "Q" : "",
																		(castling_rights & 2) ? "k" : "",
																		(castling_rights & 1) ? "q" : "");
	else sprintf(FEN_string, "%s -", FEN_string);
	// enpassant
	if (enpassant != -1) sprintf(FEN_string, "%s %c%c", FEN_string, enpassant % 16 + 'a', 8 - (enpassant / 16) + '0');
	else sprintf(FEN_string, "%s %s", FEN_string, "-");
	// halfmove
	sprintf(FEN_string, "%s %d", FEN_string, half_move);
	// fullmove
	sprintf(FEN_string, "%s %d", FEN_string, full_move);
}

void draw_pawn(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30;
	int y_pos = (square_number / 16) *  30 ;
	
	UG_FillFrame(x_pos + 5, y_pos + 23, x_pos + 23, y_pos + 25, color);
	UG_FillFrame(x_pos + 8, y_pos + 19, x_pos + 20, y_pos + 22, color);
	UG_FillFrame(x_pos + 9, y_pos + 15, x_pos + 19, y_pos + 18, color);
	UG_FillFrame(x_pos + 10, y_pos + 11, x_pos + 18, y_pos + 14, color);
	UG_FillFrame(x_pos + 13, y_pos + 9, x_pos + 15, y_pos + 10, color);
	UG_FillCircle(x_pos + 13, y_pos + 6,4, color);
}

void draw_king(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30;
	int y_pos = (square_number / 16) *  30 ;
	
	UG_FillFrame(x_pos + 5, y_pos + 23, x_pos + 23, y_pos + 25, color);
	UG_FillFrame(x_pos + 10, y_pos + 19, x_pos + 18, y_pos + 22, color);
	UG_FillFrame(x_pos + 8, y_pos + 17, x_pos + 20, y_pos + 18, color);
	UG_FillFrame(x_pos + 7, y_pos + 12, x_pos + 21, y_pos + 16, color);
	UG_FillFrame(x_pos + 13, y_pos + 3, x_pos + 15, y_pos + 10, color);
	UG_FillFrame(x_pos + 10, y_pos + 5, x_pos + 18, y_pos + 7, color);
	UG_DrawLine(x_pos + 9, y_pos + 11, x_pos + 19, y_pos + 11, color);
}

void draw_bishop(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30;
	int y_pos = (square_number / 16) *  30 ;
	
	UG_FillFrame(x_pos + 5, y_pos + 23, x_pos + 23, y_pos + 25, color);
	UG_FillFrame(x_pos + 8, y_pos + 20, x_pos + 20, y_pos + 22, color);
	UG_FillFrame(x_pos + 10, y_pos + 18, x_pos + 18, y_pos + 19, color);
	UG_FillFrame(x_pos + 12, y_pos + 13, x_pos + 16, y_pos + 17, color);
	UG_FillFrame(x_pos + 13, y_pos + 3, x_pos + 15, y_pos + 5, color);
	UG_FillCircle(x_pos + 13, y_pos + 9,3, color);
}

void draw_knight(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30;
	int y_pos = (square_number / 16) *  30 ;
	
	UG_FillFrame(x_pos + 5, y_pos + 23, x_pos + 23, y_pos + 25, color);
	UG_FillFrame(x_pos + 8, y_pos + 20, x_pos + 20, y_pos + 22, color);
	UG_FillFrame(x_pos + 9, y_pos + 18, x_pos + 20, y_pos + 19, color);
	UG_FillFrame(x_pos + 11, y_pos + 15, x_pos + 20, y_pos + 17, color);
	UG_FillFrame(x_pos + 13, y_pos + 13, x_pos + 20, y_pos + 14, color);
	UG_FillFrame(x_pos + 14, y_pos + 11, x_pos + 20, y_pos + 12, color);
	UG_FillFrame(x_pos + 11, y_pos + 6, x_pos + 20, y_pos + 10, color);
	UG_FillFrame(x_pos + 8, y_pos + 7, x_pos + 10, y_pos + 9, color);
	UG_FillFrame(x_pos + 13, y_pos + 3, x_pos + 14, y_pos + 5, color);
	UG_FillFrame(x_pos + 17, y_pos + 3, x_pos + 18, y_pos + 5, color);
}

void draw_queen(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30;
	int y_pos = (square_number / 16) *  30 ;
	
	UG_FillFrame(x_pos + 5, y_pos + 23, x_pos + 23, y_pos + 25, color);
	UG_FillFrame(x_pos + 9, y_pos + 20, x_pos + 19, y_pos + 22, color);
	UG_FillFrame(x_pos + 7, y_pos + 18, x_pos + 21, y_pos + 19, color);
	UG_FillFrame(x_pos + 5, y_pos + 16, x_pos + 23, y_pos + 17, color);
	UG_FillFrame(x_pos + 4, y_pos + 13, x_pos + 24, y_pos + 15, color);
	UG_FillFrame(x_pos + 8, y_pos + 11, x_pos + 20, y_pos + 12, color);
	UG_FillFrame(x_pos + 3, y_pos + 9, x_pos + 5, y_pos + 12, color);
	UG_FillFrame(x_pos + 8, y_pos + 7, x_pos + 10, y_pos + 10, color);
	UG_FillFrame(x_pos + 13, y_pos + 4, x_pos + 15, y_pos + 10, color);
	UG_FillFrame(x_pos + 18, y_pos + 7, x_pos + 20, y_pos + 10, color);
	UG_FillFrame(x_pos + 23, y_pos + 9, x_pos + 25, y_pos + 12, color);
}

void draw_rook(int square_number, UG_COLOR color)
{
	int x_pos = (square_number - ((square_number/16) * 16)) * 30 ;
	int y_pos = (square_number / 16) *  30;
	
	UG_FillFrame(x_pos + 7, y_pos + 23, x_pos + 25, y_pos + 25, color);
	UG_FillFrame(x_pos + 11, y_pos + 11, x_pos + 21, y_pos + 22, color);
	UG_FillFrame(x_pos + 9, y_pos + 5, x_pos + 11, y_pos + 11, color);
	UG_FillFrame(x_pos + 15, y_pos + 5, x_pos + 17, y_pos + 11, color);
	UG_FillFrame(x_pos + 21, y_pos + 5, x_pos + 23, y_pos + 11, color);
}

void draw_empty(int square_number, UG_COLOR color)
{
	
}