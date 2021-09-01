/*
 * moves.c
 *
 * Created: 15/07/2021 20:33:02
 *  Author: Jan
 */ 

#include "moves.h"


int slide_offsets[8] = {1,-1,16,-16,17,-17,15,-15};
int knight_offsets[8] = {33, 31, 18, 14, -33, -31, -18, -14};

// returns 1 if square is attacked
int is_attacked(int square, int *threat_map)
{
	if (threat_map[square] == 1) return 1;
	return 0;
}

// returns pointer to array of moves for sliding pieces
int *sliding_moves(int *board, int square, int on_move)
{
	int *moves = malloc(28 * sizeof(int));
	int moves_index = 0;
	int offset_start = 0;
	int offset_end = 8;
	//detects bishop
	if (board[square] & (bishop)) offset_start = 4;
	//detects rook
	if (board[square] & (rook)) offset_end = 4;
	for (;offset_start < offset_end; offset_start++)
	{
		int new_position = square + slide_offsets[offset_start];
		while (!(new_position & 0x88))
		{
			// if piece on new position is the same color break
			if ((board[new_position] & (white) && (board[square] & (white))) || (board[new_position] & (black) && (board[square] & (black)))) break;
			// if there is no piece on new position add position to array
			else if (board[new_position] == empty && try_move(square,new_position,on_move)) moves[moves_index++] = new_position;
			// if there is enemy piece add position and break = capture move
			else if (try_move(square,new_position,on_move))
			{
				moves[moves_index++] = new_position;
				break;
			}
			new_position += slide_offsets[offset_start];
		}
	}
	moves[moves_index] = -1;
	return moves;
}

//returns pointer to array of moves for knight
int *knight_moves(int *board, int square, int on_move)
{
	int *moves = malloc(9 * sizeof(int));
	int moves_index = 0;
	for (int offset_start = 0; offset_start < 8; offset_start++)
	{
		int new_position = square + knight_offsets[offset_start];
		if (!(new_position & 0x88))
		{
			// if piece on new position is the same color continue
			if ((board[new_position] & (white) && (board[square] & (white))) || (board[new_position] & (black) && (board[square] & (black)))) continue;
			// if there is no piece on new position add position to array
			else if (board[new_position] == empty && try_move(square,new_position,on_move)) moves[moves_index++] = new_position;
			// if there is enemy piece add position and continue = capture move
			else if (try_move(square,new_position,on_move)) moves[moves_index++] = new_position;
		}
	}
	moves[moves_index] = -1;
	return moves;
}

//returns pointer to array of moves for pawn 
int *pawn_moves(int *board, int square, int on_move)
{
	int *moves = malloc(5 * sizeof(int));
	int moves_index = 0;
	// checks if piece is black or white, true if black
	if (board[square] & (black))
	{
		//if square infront of pawn is empty move is possible
		if (board[square + 16] == empty && try_move(square,square + 16,on_move)) moves[moves_index++] = square + 16;
		//if there is enemy piece in front of pawn diagonally pawn capture move is possible
		if (((!(board[square + 15] & (black)) && board[square + 15] != empty) || (enpassant >= 80 && enpassant <= 87 && enpassant == square + 15)) && try_move(square,square + 15,on_move)) moves[moves_index++] = square + 15;
		if (((!(board[square + 17] & (black)) && board[square + 17] != empty) || (enpassant >= 80 && enpassant <= 87 && enpassant == square + 17)) && try_move(square,square + 17,on_move)) moves[moves_index++] = square + 17;
		// if pawn is in starting position it can move for two squares if they are empty
		if ((square >= 16 && square <= 23) && board[square + 32] == empty && board[square + 16] == empty && try_move(square,square + 32,on_move)) moves[moves_index++] = square + 32;
	}else
	{
		//if square infront of pawn is empty move is possible
		if (board[square - 16] == empty && try_move(square,square - 16,on_move)) moves[moves_index++] = square - 16;
		//if there is enemy piece in front of pawn diagonally pawn capture move is possible or if square diagonally is enpassant square
		if (((!(board[square - 15] & (white)) && board[square - 15] != empty) || (enpassant >= 32 && enpassant <= 39 && enpassant == square - 15)) && try_move(square,square - 15,on_move)) moves[moves_index++] = square - 15;
		if (((!(board[square - 17] & (white)) && board[square - 17] != empty) || (enpassant >= 32 && enpassant <= 39 && enpassant == square - 17)) && try_move(square,square - 17,on_move)) moves[moves_index++] = square - 17;
		// if pawn is in starting position it can move for two squares if they are empty
		if ((square >= 96 && square <= 103) && board[square - 32] == empty && board[square - 16] == empty && try_move(square,square - 32,on_move)) moves[moves_index++] = square - 32;
	}
	moves[moves_index] = -1;
	return moves;
}

//returns pointer to array of moves for king
int *king_moves(int *board, int square)
{
	// get threat map of opponent
	int side = 1;
	if (board[square] & (black)) side = 0;
	int *threat_map = get_threat_map(side, board);
	// allocates memory for possible moves
	int *moves = malloc(9 * sizeof(int));
	int moves_index = 0;
	for (int offset_start = 0; offset_start < 8; offset_start++)
	{
		int new_position = square + slide_offsets[offset_start];
		if (!(new_position & 0x88))
		{
			// if piece on new position is the same color continue
			if ((board[new_position] & (white) && (board[square] & (white))) || (board[new_position] & (black) && (board[square] & (black)))) continue;
			{
				if (!is_attacked(new_position, threat_map))
				{
					// if there is no piece on new position add position to array
					if (board[new_position] == empty) moves[moves_index++] = new_position;
					// if there is enemy piece add position = capture move
					else moves[moves_index++] = new_position;
				}
			}
			
		}
	}
	// castling moves
	if ((square == 116 || square == 4) && !is_attacked(square, threat_map))
	{
		if (board[square] & white)
		{
			if (board[117] == empty && board[118] == empty &&  (castling_rights & (1 << 3))) moves[moves_index++] = 118;
			else if (board[115] == empty && board[114] == empty && board[113] == empty && (castling_rights & (1 << 2))) moves[moves_index++] = 114;
		}else 
		{
			if (board[5] == empty && board[6] == empty && (castling_rights & (1 << 1))) moves[moves_index++] = 6;
			else if (board[3] == empty && board[2] == empty && board[1] == empty && (castling_rights & (1 << 0))) moves[moves_index++] = 2;
		}
	}
	free(threat_map);
	threat_map = NULL;
	moves[moves_index] = -1;
	return moves;
}

// returns 1 if desired move is possible
int make_move(int square, int destination, int *possible_moves)
{
	int on_move = ((board[square] & black) == 2);
	// checks if move is in move list
	for (int i = 0; possible_moves[i] != -1; i++)
	{
		if (destination == possible_moves[i])
		{
			// king moves
			if (board[square] & king)
			{
				// castling moves
				if (castling_rights)
				{
					if (destination == 118)
					{
						board[117] = w_rook;
						board[119] = empty;
					}
					else if (destination == 114)
					{
						board[115] = w_rook;
						board[112] = empty;
					}
					else if (destination == 6)
					{
						board[5] = b_rook;
						board[7] = empty;
					}
					else if (destination == 2)
					{
						board[3] = b_rook;
						board[0] = empty;
					}
				}
				// update king position
				if (on_move) king_position[1] = destination;
				else king_position[0] = destination;
				castling_rights &= ~(1 << (3 - (on_move * 2))) & ~(1 << (2 - (on_move * 2)));
			}
			// pawn moves
			if (board[square] & pawn)
			{
				//resets half move clock
				half_move = 0;
				// enpassant captures
				if (destination == enpassant)
				{
					if (board[square] & black) board[destination - 16] = empty;
					else board[destination + 16] = empty;
				}
				// reset enpassant
				enpassant = -1;
				//enpassant update
				if (square >= 16 && square <= 23 && (destination == square + 32)) enpassant = destination - 16;
				else if (square >= 96 && square <= 103 && (destination == square - 32)) enpassant = destination + 16;
			}
			// if rook moves castling on its side is not possible anymore
			if ((board[square] & rook) && castling_rights)
			{
				if(square == 119) castling_rights &= ~(1 << 3);
				else if (square == 112) castling_rights &= ~(1 << 2);
				else if (square == 7) castling_rights &= ~(1 << 1);
				else if (square == 0) castling_rights &= ~(1 << 0);
			}
			// resets half move clock if move is capture otherwise increase half move
			if (board[destination] != empty) half_move = 0;
			else half_move++;
			// make move and clear square 
			board[destination] = board[square];
			board[square] = empty; 
			return 1;
		}
	}
	return 0;
}
// returns pointer of moves for piece on the square, if square is empty return NULL
int *get_moves(int square, int on_move)
{
	if (board[square] & (pawn)) return pawn_moves(board, square, on_move);
	if (board[square] & (knight)) return knight_moves(board, square, on_move);
	if (board[square] & (bishop)) return sliding_moves(board, square, on_move);
	if (board[square] & (rook)) return sliding_moves(board, square, on_move);
	if (board[square] & (queen)) return sliding_moves(board, square, on_move);
	if (board[square] & (king)) return king_moves(board, square);
	return NULL;
}

// returns threat map for selected side
// side 0 = white 
// side 1 = black
int *get_threat_map(int side, int *board)
{
	int *threat_map = calloc(128, sizeof(int));
	// variable for bit shifting
	int side_shift = white;
	if (side) side_shift = black;
	
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			int square = rank * 16 + file;
			
			if (!(square & 0x88))
			{
				if (!(board[square] & (side_shift))) continue;
				// king attacks
				if (board[square] & (king))
				{
					// loop over sliding offsets
					for (int offset_start = 0; offset_start < 8; offset_start++)
					{
						int new_position = square + slide_offsets[offset_start];
						if (!(new_position & 0x88))
						{
							// if piece on new position is the same color continue
							if ((board[new_position] & (side_shift) && (board[square] & (side_shift)))) threat_map[new_position] = 1;
							// if there is no piece on new position add position to array
							else if (board[new_position] == empty) threat_map[new_position] = 1;
							// if there is enemy piece add position and continue = capture move
							else threat_map[new_position] = 1;
						}
					}
				} // knight attacks
				else if (board[square] & (knight))
				{
					// loop over knights offsets
					for (int offset_start = 0; offset_start < 8; offset_start++)
					{
						int new_position = square + knight_offsets[offset_start];
						if (!(new_position & 0x88))
						{
							// if piece on new position is the same color continue
							if ((board[new_position] & (side_shift) && (board[square] & (side_shift)))) threat_map[new_position] = 1;
							// if there is no piece on new position add position to array
							else if (board[new_position] == empty) threat_map[new_position] = 1;
							// if there is enemy piece add position = capture move
							else threat_map[new_position] = 1;
						}
					}
				} // pawn attacks
				else if (board[square] & (pawn))
				{
					if (board[square] & (black))
					{
						threat_map[square + 15] = 1;
						threat_map[square + 17] = 1;
					}else
					{
						threat_map[square - 15] = 1;
						threat_map[square - 17] = 1;
					}
				} // slide attacks
				else if ( (board[square] & (bishop)) || (board[square] & (rook)) || (board[square] & (queen)))
				{
					int offset_start = 0;
					int offset_end = 8;
					//detects bishop
					if (board[square] & (bishop)) offset_start = 4;
					//detects rook
					if (board[square] & (rook)) offset_end = 4;
					// loop over sliding offsets
					for (;offset_start < offset_end; offset_start++)
					{
						int new_position = square + slide_offsets[offset_start];
						// add offset until square is on board 
						while (!(new_position & 0x88))
						{
							// if piece on new position is the same color break
							if ((board[new_position] & (side_shift) && (board[square] & (side_shift))))
							{
								threat_map[new_position] = 1;
								break;
							}
							// if there is no piece on new position add position to array
							else if (board[new_position] == empty) threat_map[new_position] = 1;
							// if there is enemy piece add position and break = capture move
							else
							{
								threat_map[new_position] = 1;
								if (board[new_position] & king) threat_map[new_position + slide_offsets[offset_start]] = 1;
								break;
							}
							new_position += slide_offsets[offset_start];
						}
					}
				}
			}
			
		}
	}
	return threat_map;
}

// returns 1 if move is possible
int try_move(int square, int  destination, int side)
{
	// make temporary board
	int *temp_board = malloc(128 * sizeof(int));
	memcpy(temp_board, board, 128 * sizeof(int));
	// make move on temp board
	temp_board[destination] = board[square];
	temp_board[square] = empty;
	// if king is in check after the move, move is not possible
	if (in_check(king_position[side], side, temp_board))
	{
		free(temp_board);
		temp_board = NULL;
		return 0;	
	}
	free(temp_board);
	temp_board = NULL;
	return 1;	
}

// returns 1 if there is still possible move else return 1
int moves_available(int side, int check, int king_position)
{
	int *moves;
	int side_shift = white;	
	if (side) side_shift = black;
	// loops over the board
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			int square = rank * 16 + file;
			// if square is on board and piece is colored correctly
			if (!(square & 0x88) && (board[square] & (side_shift)))
			{
				// get moves for piece
				if (check) moves = get_moves_in_check(square, side, king_position);
				else moves = get_moves(square, side);
				// if move array is not empty player has possible moves
				if (moves != NULL && moves[0] != -1)
				{
					free(moves);
					moves = NULL;
					return 1;
				}
				free(moves);
				moves = NULL;
			}
		}
	}
	return 0;
}

// returns 1 if king is in check 
int in_check(int king_position, int side, int *board)
{
	// get threat_map of opponent
	int *threat_map = get_threat_map(!side, board);
	// if king is on attacked square player is in check
	if (threat_map[king_position] == 1)
	{
		free(threat_map);
		threat_map = NULL;
		return 1;
	}
	free(threat_map);
	threat_map = NULL;
	return 0;
}

// return pointer array of moves when playre is in check
int *get_moves_in_check(int square, int side, int king_position)
{
	// if square is empty there is no moves
	if (board[square] == empty) return NULL;
	int *all_moves, *possible_moves;
	int move_index = 0;
	// moves without check
	all_moves = get_moves(square, side);
	// allocate space for possible moves
	possible_moves = malloc(9 * sizeof(int));
	for (int move = 0; all_moves[move] != -1; move++)
	{
		// make temporary board 
		int *temp_board = malloc(128 * sizeof(int));
		memcpy(temp_board, board, 128 * sizeof(int));
		// make move on temporary board
		temp_board[all_moves[move]] = board[square];
		temp_board[square] = empty;
		if (board[square] & (king))
		{
			// king can move only to not attacked squares 
			possible_moves[move_index++] = all_moves[move];
		}else
		{
			// if king is not in check after the move move can be made
			if (!in_check(king_position, side, temp_board)) possible_moves[move_index++] = all_moves[move];
		}
		free(temp_board);
		temp_board = NULL;
	}
	free(all_moves);
	all_moves = NULL;
	// moves ending sign
	possible_moves[move_index] = -1;
	return possible_moves;
}

// returns piece based on promotion square 
int pawn_promotion(int promotion_square)
{
	if (promotion_square == 73) return queen;
	if (promotion_square == 89) return rook;
	if (promotion_square == 105) return bishop;
	if (promotion_square == 121) return knight;
	return 1;
}

int is_draw(int *board)
{
	// 50-move rule
	if (half_move >= 100) return fifty_move_rule;
	int moves = moves_available(on_move, check[on_move], king_position[on_move]);
	
	// stale mate
	if (!moves) return stale_mate;
	
	// insufficient material
	// 0-1 pawn, 2-3 knight, 4 white light squared bishop, 5 white dark squared bishop, 
	// 6 black light squared bishop, 7 black dark squared bishop, 8-9 rook, 10-11 queen
	// first element for white, second for black 
	// possible that not all cases are implemented
	int material[12] = {0};
	// loop over the board
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)
		{
			int square = rank * 16 + file;
			if (!(square & 0x88))
			{
				if (board[square] & white)
				{
					// count material
					if (board[square] & (pawn)) material[0]++;
					else if (board[square] & (knight)) material[2]++;
					else if (board[square] & (bishop))
					{
						// count different colored bishops
						if (square % 2 == 0 && (square / 16) % 2 == 0) material[4]++;
						else if (square % 2 == 1 && (square / 16) % 2 == 1) material[4]++;
						else material[5]++;
					}
					else if (board[square] & (rook)) material[8]++;
					else if (board[square] & (queen)) material[10]++;
				}
				if (board[square] & black)
				{
					// count material
					if (board[square] & (pawn)) material[1]++;
					else if (board[square] & (knight)) material[3]++;
					else if (board[square] & (bishop))
					{
						// count different colored bishops
						if (square % 2 == 0 && (square / 16) % 2 == 0) material[6]++;
						else if (square % 2 == 1 && (square / 16) % 2 == 1) material[6]++;
						else material[7]++;
					}
					else if (board[square] & (rook)) material[9]++;
					else if (board[square] & (queen)) material[11]++;
				}
				
			}
		}
	}
	// get number of pieces on board
	int num_pawns = material[0] + material[1];
	int num_knights = material[2] + material[3];
	int num_bishops = material[4] + material[5] + material[6] + material[7];
	int num_rooks = material[8] + material[9];
	int num_queens = material[10] + material[11]; 
	
	// if both players have only one rook mate cannot be forced
	if ((num_pawns + num_bishops + num_knights + num_queens == 0) && material[8] == 1 && material[9] == 1) return insufficient_material;
	if (num_rooks + num_queens > 0) return normal_move;
	if (num_pawns + num_rooks + num_queens == 0)
	{
		// check mate can not be forced if there is only one bishop or only one knight and kings on the board
		if ((num_knights == 1 && num_bishops == 0) || (num_bishops == 1 && num_knights == 0)) return insufficient_material;
		// if both players have only two bishops on opposite colored square mate can not be forced
		if ((material[4] == 1 && material[7] == 1 && num_knights == 0) || (material[5] == 1 && material[6] == 1 && num_knights == 0)) return insufficient_material;
		// if player only has two knights mate can not be forced
		if ((num_bishops == 0 && material[2] == 2) || (num_bishops == 0 && material[3] == 3)) return insufficient_material;
 
	}
	// threefold repetition not implememnted
	
	return normal_move;
}

int get_game_state(int king_position, int side, int *board)
{
	if (in_check(king_position, side, board))
	{
		// if player is in check set his side to 1
		check[side] = 1;
		// if moves are still available game is not over
		if (moves_available(side, check[side], king_position))
		{
			return check_move;
		}else
		{
			return check_mate;
		}
	}
	return is_draw(board);
}
