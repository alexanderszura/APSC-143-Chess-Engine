#include "stdio.h"
#include "tools.h"
#include "board.h"
#include "moves.h"

// Evaluates the material difference between black and white.
float eval_material(struct chess_board *board);

// Evaluates the positional scores of all pieces on the board. Arrays are defined in engine.c
float positional_eval(struct chess_board *board);

// Evaluates who is winning. This function is key for the reccomended move function later.
// Outputs a number determining if white is winning either or both positionally and in terms 
// of material (positive number) or if black is winning (negative number).
float evaluate_board(struct chess_board *board);

// Prints the engine's evaluation of the current board position to the console.
void print_evaluation(struct chess_board *board);

// Uses the evaluate_board function and the generate_legal_moves function from moves.c
// to determine the best possbile move positonally and in terms of material.
// Key note: does not consider many tactics (forks, pins, skewers, etc.) so the move 
// may not always be the best and can lose  against a better human player.
void print_recommended_move(struct chess_board *board);