#include <stdbool.h>
#include "tools.h"

#ifndef APSC143__BOARD_H
#define APSC143__BOARD_H

#define MAX_DEPTH 3 
#define KING_X_LOCATION 4

char player_char(enum chess_player player);

// Gets a lowercase string denoting the piece type.
const char *piece_string(enum chess_piece piece);

char piece_char(enum chess_piece piece);

const char *color_string(enum chess_player color);

// Initializes the state of the board for a new chess game.
void board_initialize(struct chess_board *board);

bool find_forced_mate(struct chess_board *board, int depth, struct chess_move *recommended_move);

// Classify the state of the board, printing one of the following:
// - game incomplete
// - white wins by checkmate
// - black wins by checkmate
// - draw by stalemate
void board_summarize(struct chess_board *board);

struct dynamic_array *generate_legal_moves(enum chess_piece piece, struct chess_board board, int id, bool include_castle, bool remove_moves_when_checked);

bool king_in_check(struct chess_board *board, enum chess_player player);
bool player_in_check(struct chess_board *board, int id_to_check);
bool check_for_castle(struct chess_board board, bool *castle_left, bool *castle_right);

#endif