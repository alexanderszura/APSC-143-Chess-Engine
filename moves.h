#include <stdbool.h>
#include "tools.h"

#ifndef APSC143__MOVES_H
#define APSC143__MOVES_H

// Determine which piece is moving, and complete the move data accordingly.
// Panics if there is no piece which can make the specified move, or if there
// are multiple possible pieces.
void board_complete_move(const struct chess_board *board, struct chess_move *move);
bool handle_castle_move(const struct chess_board *board, struct chess_move *move);
int select_piece_for_move(const struct chess_board *board, struct chess_move *move, enum chess_player color);

// Apply move to the board. The move must already be complete, i.e., the initial
// square must be known. Panics if the move is not legal in the current board
// position.
void board_apply_move(struct chess_board *board, const struct chess_move *move);
void create_move(struct chess_move *move, struct chess_board *board, int from_id, int to_id);
void print_recommended_move(struct chess_move *move, enum chess_player player);

bool add_move(struct dynamic_array *moves, struct chess_board board, int x, int y, enum chess_player color);
struct dynamic_array *generate_legal_moves(enum chess_piece piece, struct chess_board board, int id, bool include_castle, bool remove_moves_when_checked);
void move_piece(struct chess_board *board, int from, int to);



#endif