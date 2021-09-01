/*
 * Chess.c
 *
 * Created: 15/07/2021 20:17:42
 * Author : Jan
 */ 

#include <avr/io.h>
#include <string.h>
#include <util/atomic.h>
#include "board.h"
#include "pieces.h"
#include "moves.h"
#include "LCD_Ili9341.h"
#include "kbd.h"
#include "systime.h"
#include "display.h"
#include "joystick.h"
#include "buffer.h"
#include "UART.h"

typedef enum GAME_states {GAME_WELCOME_STATE, GAME_PLAY_STATE, GAME_OVER_STATE} GAME_states_t;
typedef enum WELCOME_states		{WELCOME_INIT, WELCOME_BUTTON, WELCOME_WAIT, WELCOME_CONNECT, 
								WELCOME_PIECE_SELECT, WELCOME_RECEIVE} WELCOME_states_t;
typedef enum GAMEPLAY_states	{GAMEPLAY_INIT, GAMEPLAY_BUTTON, GAMEPLAY_JOYSTICK, GAMEPLAY_MOVES,
								GAMEPLAY_UPDATE_BOARD, GAMEPLAY_CHECK_POSITION, GAMEPLAY_WAIT_FEN, 
								GAMEPLAY_SEND_FEN, GAMEPLAY_MAKE_MOVE, GAMEPLAY_PAWN_PROMOTION, GAMEPLAY_GAMESTATE} GAMEPLAY_states_t;
typedef enum GAMEOVER_states {GAMEOVER_INIT, GAMEOVER_RESULT, GAMEOVER_BUTTON, GAMEOVER_WAIT} GAMEOVER_states_t;


/*
	COMMUNCATION BUFFERS
*/
Circular_buffer buffer_RX;
Circular_buffer buffer_TX;

/*
	FUNCTION PROTOTYPES
*/
void game(void);
int welcome(void);
int game_play(void);
int game_over(void);

/*
	GAME VARIABLES
*/
// 0 = white, 1 = black
int player;
// 0 = one controller, 1 = two controllers
int two_controllers;
// game state 
// updates after every move
int game_state;

/*
	INERRUPT ROUTINES
*/
// receive complete interrupt routine
ISR (USART0_RX_vect)
{
	// store data to RX buffer when receiving data
	BUFF_store_data(&buffer_RX, UDR0);
}
// transmit buffer can only be written when the UDRE flag in the UCSRnA register is set
// UDRE flag is set if transmit buffer UDRn can receive new data
// when we write bit to UDRIE it enables UDRE interrupt flag
ISR (USART0_UDRE_vect)
{
	char data;
	// get character from transmitting buffer
	BUFF_get_data(&buffer_TX, &data);
	// set transmit buffer
	UDR0 = data;
	// after we send data we clear UDRIE bit in USCRB
	UCSR0B &= ~(1 << UDRIE0);
}

/*
	MAIN PROGRAM
*/

int main(void)
{
	KBD_Init();
	LCD_Init();
	Systime_Init();
	ADC_Init();
	UART_init(MYUBRR);
	sei();
	
    while (1) 
    {
		game();
    }
}

void game(void)
{
	static GAME_states_t state = GAME_WELCOME_STATE;
	int result = 0;
	
	switch(state)
	{
		case GAME_WELCOME_STATE:
			result = welcome();
			if (result != 0) state = GAME_PLAY_STATE;
			break;
			
			
		case GAME_PLAY_STATE:
			result = game_play();
			if (result != 0) state = GAME_OVER_STATE;
			break;
			
			
		case GAME_OVER_STATE:
			result = game_over();
			if (result != 0) state = GAME_WELCOME_STATE;
			break;
			
			
		default:
			state = GAME_WELCOME_STATE;
			break;
	}
	
}

int welcome(void)
{
	static WELCOME_states_t state = WELCOME_INIT;
	static uint32_t timer;
	static int start_receiving;
	int exit = 0;
	int no_error;
	char data;
	int key;
	
	switch (state)
	{
		// reset game variables
		case WELCOME_INIT:
			start_receiving = 0;
			two_controllers = 0;
			player = 0;
			game_state = 0;
			display_welcome_window();
			timer = GetSysTick();
			state = WELCOME_WAIT;
			break;
			
			case WELCOME_WAIT:
			if (Has_X_MillisecondsPassed(1000, &timer))
			{
				state = WELCOME_BUTTON;
				KBD_flush();
			}
			break;
		
		case WELCOME_RECEIVE:
			// allocates memory for receiving buffer
			if (!start_receiving)
			{
				buffer_RX.storage = malloc(8 * sizeof(char*));
				buffer_RX.rear = 0, buffer_RX.Q_size = 0, buffer_RX.front = 0;
				start_receiving = 1;
			}else // checks if second uC has sent data
			{
				// if receiving buffer has items add items to FEN string
				if (BUFF_get_number_of_items(&buffer_RX))
				{
					// atomic block is used so the block is not interrupted 
					// interrupts are off during execution of the block
					// atomic restorestate sets interrupt settings as they were before
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						no_error = BUFF_get_data(&buffer_RX, &data);
					}
					if (no_error)
					{
						// player that sent char is white, player that got char is black
						if (data == '0') player = 1;
						else if (data == '1') player = 0;
						// clears screen
						UG_FillFrame(0,0,320,240, C_BLACK);
						free((void*)buffer_RX.storage);
						buffer_RX.storage = NULL;
						two_controllers = 1;
						start_receiving = 0;
						state = WELCOME_INIT;
						exit = 1;
					}
				}else if (two_controllers) state = WELCOME_PIECE_SELECT;
				else state = WELCOME_BUTTON;
			}
			break;
		
		case WELCOME_BUTTON:
			KBD_Read();
			key = KBD_GetKey();
			// connect to second uC
			if (key == BTN_A)
			{
				two_controllers = 1;
				UG_FillFrame(0,100,320,240, C_BLACK);
				UG_PutString(120, 100, "A=white");
				UG_PutString(120, 120, "B=black");
				state = WELCOME_PIECE_SELECT;
			}
			// one uC
			else if (key == BTN_B)
			{
				two_controllers = 0;
				free((void*)buffer_RX.storage);
				buffer_RX.storage = NULL;
				UG_FillFrame(0,0,320,240, C_BLACK);
				exit = 1;
				state = WELCOME_INIT;
			} 
			else state = WELCOME_RECEIVE;
			break;
		
		
		case WELCOME_PIECE_SELECT:
			// if two uC are used player has to select side
			KBD_Read();
			key = KBD_GetKey();
			// player 1 selects white pieces
			if (key == BTN_A)
			{
				player = 0;
				free((void*)buffer_RX.storage);
				buffer_RX.storage = NULL;
				UG_FillFrame(0,0,320,240, C_BLACK);
				state = WELCOME_CONNECT;
			}
			// player 1 selects black pieces
			else if (key == BTN_B)
			{
				player = 1;
				free((void*)buffer_RX.storage);
				buffer_RX.storage = NULL;
				UG_FillFrame(0,0,320,240, C_BLACK);
				state = WELCOME_CONNECT;
			} else state = WELCOME_RECEIVE;
			break;
		
		
		case WELCOME_CONNECT:
			// sends data to second uC
			// allocates memory for transmitting buffer
			buffer_TX.storage = malloc(8 * sizeof(char));
			buffer_TX.rear = 0, buffer_TX.Q_size = 0, buffer_TX.front = 0;
			// if player is black send 1 otherwise send 0
			if (player) BUFF_store_data(&buffer_TX, '1');
			else BUFF_store_data(&buffer_TX, '0');
			// send char through UART
			// if buffer has items trigger write one to UDRIE in UCSRB
			while(BUFF_get_number_of_items(&buffer_TX))
			{
				UCSR0B |= (1 << UDRIE0);
			}
			free((void*)buffer_TX.storage);
			buffer_TX.storage = NULL;
			exit = 1;
			state = WELCOME_INIT;
			break;
		
		
		default:
			state = WELCOME_INIT;
			break;
	}
	return exit;
}

int game_play(void)
{
	static GAMEPLAY_states_t state = GAMEPLAY_INIT;
	static int cursor_square;
	static int pawn_promotion_square;
	static int possible_moves_visible;
	static int selected_square;
	static int *moves;
	static uint32_t timer1;
	static uint32_t timer2;
	int key;
	int no_error;
	static int start_receiving;
	static int received;
	char data;
	int exit = 0;
	
	switch (state)
	{
		case GAMEPLAY_INIT:
			// reset game variables
			cursor_square = 0;
			pawn_promotion_square = 73;
			possible_moves_visible = 0;
			selected_square = 0;
			start_receiving = 0;
			received = 0;
			check[0] = 0;
			check[1] = 0;
			// set starting FEN
			sprintf(FEN_string, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			parse_FEN(FEN_string);
			print_board(board);
			timer1 = GetSysTick();
			// if player is starting 
			if (player == 0) state = GAMEPLAY_JOYSTICK;
			else 
			{
				// clears string for receiving
				FEN_string[0] = '\0';
				state = GAMEPLAY_WAIT_FEN;
			}
			break;
			
			
		
		case GAMEPLAY_JOYSTICK:
			// waits 150 ms so the cursor doesn't move too fast
			if (Has_X_MillisecondsPassed(150, &timer1))
			{
				// moves cursor around the board
				display_clear_square(cursor_square);
				// update square if cursor was moved
				cursor_square = update_square_position(cursor_square, JOY_read_axis(X_AXIS), JOY_read_axis(Y_AXIS));
				display_selected_square(cursor_square);
			}
			state = GAMEPLAY_BUTTON;
			break;
			
			
		
		case GAMEPLAY_BUTTON:
			KBD_Read();
			key = KBD_GetKey();
			if (key == BTN_A)
			{
				// if player is on move and piece wasnt selected yet
				if((!possible_moves_visible) && (board[cursor_square] & (1 << on_move)))
				{
					// if player pressed A square is selected 
					selected_square = cursor_square;
					state = GAMEPLAY_MOVES;
				}
				if (possible_moves_visible && (board[selected_square] & (1 << on_move)))
				{
					// when moves are visible and if player is on the move go to state where we check if move is possbile
					state = GAMEPLAY_MAKE_MOVE;
				}
			}else if ((key == BTN_B) && possible_moves_visible)
			{
				// hide possible moves if player dont want to move selected piece
				possible_moves_visible = 0;
				display_clear_possible_moves(moves);		// clears possible moves
				free(moves);
				moves = NULL;
				state = GAMEPLAY_JOYSTICK;
			}else	// if button wasnt pressed back to joystick
			{
				state = GAMEPLAY_JOYSTICK;	
			}
			break;
			
			
		
		case GAMEPLAY_MAKE_MOVE:
			// if desired move is possible make move and clear moves from board, change who is on move
			if(make_move(selected_square, cursor_square, moves))
			{
				// full move counter is updated
				if (on_move) full_move++;
				// pawn promotion
				if (board[cursor_square] & (pawn))
				{
					// if pawn reaches last squares on the board display promotion window and go to pawn promotion state
					// on_move is updated because player move his piece
					if (cursor_square >= 112 || cursor_square <= 7)
					{
						display_pawn_promotion_select(on_move);
						on_move ^= 1;
						possible_moves_visible = 0;
						display_clear_possible_moves(moves);		// clears possible moves
						free(moves);
						moves = NULL;
						state = GAMEPLAY_PAWN_PROMOTION;
						break;
					}
				}
				on_move ^= 1;
				possible_moves_visible = 0;
				display_clear_possible_moves(moves);		// clears possible moves
				free(moves);
				moves = NULL;
				state = GAMEPLAY_UPDATE_BOARD;
			}else // if move is not possible back to joystick to select possible move
			{
				state = GAMEPLAY_JOYSTICK;
			}
			break;
			
			
			
		case GAMEPLAY_MOVES: 
			// get moves for current piece
			if (check[on_move]) moves = get_moves_in_check(selected_square, on_move, king_position[on_move]);
			else moves = get_moves(selected_square, on_move);
			// if selected square is not empty and piece has possible moves
			if (!(moves == NULL) && !(moves[0] == -1))
			{
				display_possible_moves(moves, C_GREEN);
				possible_moves_visible = 1;
			} else 
			{
				free(moves);
				moves = NULL;
			}
			state = GAMEPLAY_JOYSTICK;
			break;
			
			
			
		case GAMEPLAY_PAWN_PROMOTION:
			if (Has_X_MillisecondsPassed(150, &timer2))
			{
				// clears square in pawn promotion window
				display_clear_square(pawn_promotion_square);
				KBD_Read();
				key = KBD_GetKey();
				// if A is pressed piece is selected
				if(key == BTN_A)
				{
					// piece that is selected is placed on board
					board[cursor_square] = !on_move ? pawn_promotion(pawn_promotion_square) + black : pawn_promotion(pawn_promotion_square) + white;
					display_clear_pawn_promotion_select();
					display_clear_square(pawn_promotion_square);
					// reset pawn promotion square
					pawn_promotion_square = 73;
					state = GAMEPLAY_UPDATE_BOARD;
					break;
				}else if (key == BTN_B)	// move down on pawn select window
				{
					pawn_promotion_square += 16;
					// if square reaches bottom go to the top one
					if (pawn_promotion_square > 121) pawn_promotion_square = 73;
				}else if (key == BTN_OK) // move up on pawn select window
				{
					pawn_promotion_square -= 16;
					// if square reaches top go to the bottom one
					if (pawn_promotion_square < 73) pawn_promotion_square = 121;
				}
			}
			display_selected_square(pawn_promotion_square);
			break;
			
		
			
		case GAMEPLAY_UPDATE_BOARD:
			// update board
			// if we are making move dont parse fen 
			if (received)
			{
				parse_FEN(FEN_string);
			}
			print_board(board);
			state = GAMEPLAY_CHECK_POSITION;
			break;
			
			
			
		case GAMEPLAY_CHECK_POSITION:
			// get game state
			game_state = get_game_state(king_position[on_move], on_move, board);
			if (game_state == normal_move)
			{
				check[on_move] = 0;
				received = 0;
				if (player == on_move) state = GAMEPLAY_JOYSTICK;
				else if (two_controllers) state = GAMEPLAY_SEND_FEN;
				else state = GAMEPLAY_JOYSTICK;
			}
			else if (game_state == check_move)
			{
				received = 0;
				if (player == on_move) state = GAMEPLAY_JOYSTICK;
				else if (two_controllers) state = GAMEPLAY_SEND_FEN;
				else state = GAMEPLAY_JOYSTICK;
			}
			else // if game is draw or in check mate state game is over 
			{
				// if we received FEN and game is over we dont send the FEN because second 
				// controller is already on gameover screen
				if (two_controllers && !received) state = GAMEPLAY_SEND_FEN;
				else
				{
					state = GAMEPLAY_INIT;
					exit = 1;
				}
			}
			break;
			
			
		
		case GAMEPLAY_SEND_FEN:
			// allocates memory for transmitting buffer
			buffer_TX.storage = malloc(BUFFER_LENGTH * sizeof(char*));
			buffer_TX.rear = 0, buffer_TX.Q_size = 0, buffer_TX.front = 0;
			// updates FEN string
			make_FEN(board);
			// store FEN string in TX buffer
			for (int i = 0; FEN_string[i] != '\0'; i++)
			{
				BUFF_store_data(&buffer_TX, FEN_string[i]);
				
			}
			// add string termination character for detection in receiving
			BUFF_store_data(&buffer_TX, '\0');
			// send char through UART
			// if buffer has items trigger write one to UDRIE in UCSRB
			while(BUFF_get_number_of_items(&buffer_TX))
			{
				UCSR0B |= (1 << UDRIE0);
			}
			free((void*)buffer_TX.storage);
			buffer_TX.storage = NULL;
			// clear FEN string for receiving
			FEN_string[0] = '\0';
			state = GAMEPLAY_GAMESTATE;
			break;
			
			
		case GAMEPLAY_GAMESTATE:
			// if game is over dont wait for FEN
			if (game_state == normal_move || game_state == check_move) state = GAMEPLAY_WAIT_FEN;
			else
			{
				state = GAMEPLAY_INIT;
				exit = 1;
			}
			break;
			
			
			
		case GAMEPLAY_WAIT_FEN:
			// allocates memory for receiving buffer
			if (!start_receiving)
			{
				buffer_RX.storage = malloc(BUFFER_LENGTH * sizeof(char*));
				buffer_RX.rear = 0, buffer_RX.Q_size = 0, buffer_RX.front = 0;
				start_receiving = 1;	 
			}
			else 
			{
				// if receiving buffer has items add items to FEN string
				if (BUFF_get_number_of_items(&buffer_RX))
				{
					// atomic block is used so the block is not interrupted
					// interrupts are off during execution of the block
					// atomic restorestate sets interrupt settings as they were before
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						no_error = BUFF_get_data(&buffer_RX, &data);
					}
					if (no_error)
					{
						// add received character to FEN string
						sprintf(FEN_string, "%s%c", FEN_string, data);
						// when we get string termination character we stop receiving
						if (data == '\0') 
						{
							free((void*)buffer_RX.storage);
							buffer_RX.storage = NULL;
							start_receiving = 0, received = 1;
							state = GAMEPLAY_UPDATE_BOARD;
						}
						
					}
				}
					
			}
			break;
			
		
			
		default:
			state = GAMEPLAY_INIT;
			break;
	}
	return exit;
}

int game_over(void)
{
	static GAMEOVER_states_t state = GAMEOVER_INIT;
	static uint32_t timer;
	int exit = 0;
	int key;
	
	switch (state)
	{
		case GAMEOVER_INIT:
			display_gameover_screen();
			timer = GetSysTick();
			state = GAMEOVER_RESULT;
			break;
			
		case GAMEOVER_RESULT:
			// after 1s display game result
			if (Has_X_MillisecondsPassed(1000, &timer))
			{
				timer = GetSysTick();
				display_result(game_state);
				state = GAMEOVER_WAIT;
			}
			break;
		
		case GAMEOVER_WAIT:
			// wait for 2s 
			if (Has_X_MillisecondsPassed(2000, &timer))
			{
				KBD_flush();
				state = GAMEOVER_BUTTON;
			}
			break;
			
		case GAMEOVER_BUTTON:
			// if key is pressed exit and go to welcome state
			KBD_Read();
			key = KBD_GetKey();
			if (key != 0)
			{
				state = GAMEOVER_INIT;
				exit = 1;
			}
			break;
			
		default:
			state = GAMEOVER_INIT;
			break;
	}
	return exit;
}