#include "stdio.h"
#include "board.h"
#include "board.c"
#include "tools.h"
#include "tools.c"

int materialValue[] = {
        1,     // Pawn
        2,     // Knight
        3,     // Bishop
        5,     // Rook
        9,     // Queen
        1      // King (Arbitrary value, used so that the computer does not do king moves over and over).
};


// Each piece has a positional score based on their impact on the game.
// The positional score are from black and white's perspectives.
// The positional score is multiplied by the piece's material value to get the final positional score.
// Strong knights can be better than bad rooks, for example.
int White_pawnScore[64] = {
        00, 00, 00, 00, 00, 00, 00, 00,     // Starting rank
        10, 10, 10, 05, 05, 15, 10, 10,     // F-pawn protects king diagonal (against Qg5)
        05, 05, 05, 05, 05, 00, 05, 05,
        00, 05, 05, 20, 20, 05, 05, 05,     // Centerpawns
        10, 10, 10, 10, 10, 10, 10, 10,
        20, 20, 20, 30, 30, 20, 20, 20,     // Attacking pawns
        90, 90, 90, 90, 90, 90, 90, 90      // Promotion rank
};

int Black_pawnScore[64] = {
        90, 90, 90, 90, 90, 90, 90, 90,     // Promotion rank
        20, 20, 20, 30, 30, 20, 20, 20,
        10, 10, 10, 10, 10, 10, 10, 10,
        00, 05, 05, 20, 20, 05, 05, 05,
        05, 05, 05, 05, 05, 00, 05, 05,
        10, 10, 10, 05, 05, 15, 10, 10,
        00, 00, 00, 00, 00, 00, 00, 00      // Starting rank
};

int White_knightScore[64] = {
        -20,-10,-15,-20,-20,-15,-10,-20,    // Starting rank
         00, 00, 05, 05, 00, 00, 00, 00,
         00, 10, 10, 20, 20, 10, 10, 00,
         05, 10, 15, 20, 20, 15, 10, 05,
         00, 00, 15, 20, 20, 15, 00, 00,
         00, 10, 10, 10, 10, 10, 10, 00,
         00, 00, 05, 10, 10, 05, 00, 00,
         00, 00, 00, 00, 00, 00, 00, 00     // Enemy rank
};

int Black_knightScore[64] = {
         00, 00, 00, 00, 00, 00, 00, 00,     // Enemy rank
         00, 00, 05, 10, 10, 05, 00, 00,
         00, 10, 10, 10, 10, 10, 10, 00,
         00, 00, 15, 20, 20, 15, 00, 00,
         05, 10, 15, 20, 20, 15, 10, 05,
         00, 10, 10, 20, 20, 10, 10, 00,
         00, 00, 05, 05, 00, 00, 00, 00,
        -20,-10,-15,-20,-20,-15,-10,-20      // Starting rank
};

int White_bishopScore[64] = {
         -5, -5, 05, -5, -5, 05, -5, -5,    // Starting rank
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 15, 15, 20, 20, 15, 15, 05,
         10, 15, 20, 25, 25, 20, 15, 10,
         10, 15, 20, 25, 25, 20, 15, 10,
         05, 15, 15, 20, 20, 15, 15, 05,
         15, 20, 20, 25, 25, 20, 20, 15,    
         20, 25, 25, 30, 30, 25, 25, 20     // Enemy rank
};

int Black_bishopScore[64] = {
         20, 25, 25, 30, 30, 25, 25, 20,     // Enenmy rank
         15, 20, 20, 25, 25, 20, 20, 15,
         05, 15, 15, 20, 20, 15, 15, 05,
         10, 15, 20, 25, 25, 20, 15, 10,
         10, 15, 20, 25, 25, 20, 15, 10,
         05, 15, 15, 20, 20, 15, 15, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         -5, -5, 05, -5, -5, 05, -5, -5      // Staring rank
};

int White_rookScore[64] = {
         00, 00, 05, 10, 10, 05, 00, 00,    // Starting rank
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         20, 20, 20, 20, 20, 20, 20, 20,    // Enemy pawn rank
         30, 20, 20, 35, 35, 20, 20, 30     // Enemy rank
};

int Black_rookScore[64] = {
         30, 20, 20, 35, 35, 20, 20, 30,     // Enemy rank
         20, 20, 20, 20, 20, 20, 20, 20,     // Enemy pawn rank
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         05, 10, 10, 15, 15, 10, 10, 05,
         00, 00, 05, 10, 10, 05, 00, 00      // Starting Rank
};

int White_queenScore[64] = {
         00, 00, 05, 10, 10, 05, 00, 00,    // Starting rank
         05, 10, 15, 20, 20, 15, 10, 05,
         10, 15, 20, 25, 25, 20, 15, 10,
         15, 20, 25, 30, 30, 25, 20, 15, 
         15, 20, 25, 30, 30, 25, 20, 15,
         10, 15, 20, 25, 25, 20, 15, 10,
         20, 25, 30, 35, 35, 30, 25, 20,    
         25, 30, 35, 40, 40, 35, 30, 25     // Enemy rank
};

int Black_queenScore[64] = {
         25, 30, 35, 40, 40, 35, 30, 25,     // Enemy rank
         20, 25, 30, 35, 35, 30, 25, 20,
         10, 15, 20, 25, 25, 20, 15, 10,
         15, 20, 25, 30, 30, 25, 20, 15,
         15, 20, 25, 30, 30, 25, 20, 15,
         10, 15, 20, 25, 25, 20, 15, 10,
         05, 10, 15, 20, 20, 15, 10, 05,
         00, 00, 05, 10, 10, 05, 00, 00      // Starting rank
};

int White_kingScore[64] = {
        00, 00, 10, 00, 00, 00, 10, 00,    // Home rank (castling on c1,g1)
      -10,-10,-10,-10,-10,-10,-10,-10,
      00, 00, 00, 00, 00, 00, 00, 00, 
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00       // Enemy rank
};

int Black_kingScore[64] = {
      00, 00, 00, 00, 00, 00, 00, 00,     // Enemy rank
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00,
      -10,-10,-10,-10,-10,-10,-10,-10,
        00, 00, 10, 00, 00, 00, 10, 00    // Home rank (castling on c8,g8)
};


float color_material_eval(struct chess_board *board, bool color) {
        float score = 0.0f;

        int numPawns = 0;
        int numKnights = 0;
        int numBishops = 0;
        int numRooks = 0;
        int numQueens = 0;

        for(int i = 0; i<64; i++) {
                int current_piece = -1;

                if (board->piece_present[i] && board->piece_color[i] == color) {
                   current_piece =  board->piece_id[i];
                }

                switch(current_piece) {
                        case 0:
                                numPawns++;
                                break;
                        case 1:
                                numKnights++;
                                break;
                        case 2:
                                numBishops++;
                                break;
                        case 3:
                                numRooks++;
                                break;
                        case 4:
                                numQueens++;
                                break;
                        case 5:
                                break;
                }
        }

        score = (numPawns * materialValue[PIECE_PAWN]) + 
                (numKnights * materialValue[PIECE_KNIGHT]) + 
                (numBishops * materialValue[PIECE_BISHOP]) + 
                (numRooks * materialValue[PIECE_ROOK]) + 
                (numQueens * materialValue[PIECE_QUEEN]);

    return score;
}

float eval_material(struct chess_board *board) {
        float material_score = 0.0f;
        material_score = color_material_eval(board, PLAYER_WHITE) - color_material_eval(board, PLAYER_BLACK);

        return material_score;
}

float color_squares_eval(struct chess_board *board, bool color) {
        float score = 0.0f;
        int current_piece_id = 0;

        for (int i = 0; i < 64; i++) {
                if (board->piece_present[i] == true && board->piece_color[i] == color) {
                        current_piece_id = board->piece_id[i];

                        int *score_array = NULL;
                        if (color == PLAYER_WHITE) {
                                switch(current_piece_id) {
                                        case PIECE_PAWN:
                                                score_array = White_pawnScore;
                                                break;
                                        case PIECE_KNIGHT:
                                                score_array = White_knightScore;
                                                break;
                                        case PIECE_BISHOP:
                                                score_array = White_bishopScore;
                                                break;
                                        case PIECE_ROOK:
                                                score_array = White_rookScore;
                                                break;
                                        case PIECE_QUEEN:
                                                score_array = White_queenScore;
                                                break;
                                        case PIECE_KING:
                                                score_array = White_kingScore;
                                                break;
                                }
                        } else {
                                switch(current_piece_id) {
                                        case PIECE_PAWN:
                                                score_array = Black_pawnScore;
                                                break;
                                        case PIECE_KNIGHT:
                                                score_array = Black_knightScore;
                                                break;
                                        case PIECE_BISHOP:
                                                score_array = Black_bishopScore;
                                                break;
                                        case PIECE_ROOK:
                                                score_array = Black_rookScore;
                                                break;
                                        case PIECE_QUEEN:
                                                score_array = Black_queenScore;
                                                break;
                                        case PIECE_KING:
                                                score_array = Black_kingScore;
                                                break;
                                }
                        }

                        // Add the positional evaluation for all pieces together to return the final score
                        if (score_array != NULL) {
                                score += score_array[i];
                        }
                }
        }

        return score;
}

float positional_eval(struct chess_board *board) {
        float positional_score = 0.0f;

        positional_score = color_squares_eval(board, PLAYER_WHITE) - color_squares_eval(board, PLAYER_BLACK);

        return positional_score;
}


//  Negative number means black is winning, positive means white is winning (same as other chess engines like stockfish).
float evaluate_board(struct chess_board *board) {
        float total_evaluation = 0.0f;

        total_evaluation += eval_material(board);
        total_evaluation += positional_eval(board);
        return total_evaluation;
}

void print_evaluation(struct chess_board *board) {
        printf("Board evaluation: %.2f\n", evaluate_board(board));
}