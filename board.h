#include <stdbool.h>
#include "tools.h"

#ifndef APSC143__BOARD_H
#define APSC143__BOARD_H

char player_char(enum chess_player player);

// Gets a lowercase string denoting the piece type.
const char *piece_string(enum chess_piece piece);

char piece_char(enum chess_piece piece);

const char *color_string(enum chess_player color);

// Initializes the state of the board for a new chess game.
void board_initialize(struct chess_board *board);

// Determine which piece is moving, and complete the move data accordingly.
// Panics if there is no piece which can make the specified move, or if there
// are multiple possible pieces.
void board_complete_move(const struct chess_board *board, struct chess_move *move);

// Apply move to the board. The move must already be complete, i.e., the initial
// square must be known. Panics if the move is not legal in the current board
// position.
void board_apply_move(struct chess_board *board, const struct chess_move *move);

bool find_forced_mate(struct chess_board *board, int depth, struct chess_move *recommended_move);

void print_recommended_move(struct chess_move *move, enum chess_player player);

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
void create_move(struct chess_move *move, struct chess_board *board, int from_id, int to_id);

#endif
