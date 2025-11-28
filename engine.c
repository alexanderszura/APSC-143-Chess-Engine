// Chess engine evaluation - Edward S

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
        1      // King
};

int White_pawnScore[64] = {
        00, 00, 00, 00, 00, 00, 00, 00,     // Starting rank
<<<<<<< Updated upstream
        10, 10, 10, 05, 05, 15, 10, 10,     // F-pawn protects king diagonal
=======
        10, 10, 10, 05, 05, 15, 10, 10,     // F-pawn protects king diagonal (against Qg5)
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
// Edit these for 10 instead of -1000
int White_kingScore[64] = {
         00, 00,10, 00, 00, 00,10, 00,    // Home rank (castling on c1,g1)
      -10,-10,-10,-10,-10,-10,-10,-10,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000, 
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000     // Enemy rank
};

// Edit these for 10 instead of -1000
int Black_kingScore[64] = {
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,     // Enemy rank
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -10,-10,-10,-10,-10,-10,-10,-10,
         00, 00,10, 00, 00, 00,10, 00      // Home rank (castling on c8,g8)
};

int white_score_arrays[6];
int black_score_arrays[6];

float color_material_eval(bool color) {
        float score = 0.0f;

        int numPawns = 0;
        int numKnights = 0;
        int numBishops = 0;
        int numRooks = 0;
        int numQueens = 0;

        struct chess_board board;

        for(int i = 0; i<64; i++) {
                int current_piece;

                if (board.piece_color[i] == color) {
                   current_piece =  board.piece_id[i];
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
                (numRooks + materialValue[PIECE_ROOK]) + 
                (numQueens * materialValue[PIECE_QUEEN]);

    return score;
=======
int White_kingScore[64] = {
        0, 0, 10, 0, 0, 0, 10, 0,   // White castling positions have high scores -> c1, g1.
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0,     
        0, 0, 0, 0, 0, 0, 0, 0      
};

int Black_kingScore[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 10, 0, 0, 0, 10, 0    // Black castling positions -> c8, g8.
};

const int *white_score_arrays[6] = {
        White_pawnScore,
        White_knightScore,
        White_bishopScore,
        White_rookScore,
        White_queenScore,
        White_kingScore
};

const int *black_score_arrays[6] = {
        Black_pawnScore,
        Black_knightScore,
        Black_bishopScore,
        Black_rookScore,
        Black_queenScore,
        Black_kingScore
};

float color_material_eval(const struct chess_board *board, enum chess_player color) {

        float score = 0.0f;

        int numPawns   = 0;
        int numKnights = 0;
        int numBishops = 0;
        int numRooks   = 0;
        int numQueens  = 0;

        for (int i = 0; i < BOARD_SIZE; i++) {
                if (board->piece_present[i] && board->piece_color[i] == color) {

                        enum chess_piece current_piece = board->piece_id[i];

                        switch (current_piece) {
                        case PIECE_PAWN:
                                numPawns++;
                                break;
                        case PIECE_KNIGHT:
                                numKnights++;
                                break;
                        case PIECE_BISHOP:
                                numBishops++;
                                break;
                        case PIECE_ROOK:
                                numRooks++;
                                break;
                        case PIECE_QUEEN:
                                numQueens++;
                                break;
                        case PIECE_KING:
                                break;
                        }
                }
        }

        score = (numPawns   * materialValue[PIECE_PAWN]) +
                (numKnights * materialValue[PIECE_KNIGHT]) +
                (numBishops * materialValue[PIECE_BISHOP]) +
                (numRooks   * materialValue[PIECE_ROOK]) +
                (numQueens  * materialValue[PIECE_QUEEN]);

        return score;
>>>>>>> Stashed changes
}

float eval_material() {
        float material_score = 0.0f;
        material_score = color_material_eval(PLAYER_WHITE) - color_material_eval(PLAYER_BLACK);

        return material_score;
}

float color_squares_eval(bool color) {
        float score = 0.0f;
        int current_piece_id = 0;

        struct chess_board board;

<<<<<<< Updated upstream
        for (int i; i < 64; i++) {
                if (board.piece_present[i] = true && board.piece_color[i] == color) {
                        current_piece_id = board.piece_id[i];

=======
        for (int i = 0; i < BOARD_SIZE; i++) {
                if (board.piece_present[i] = true && board.piece_color[i] == color) {
                        current_piece_id = board.piece_id[i];

                        
>>>>>>> Stashed changes
                }
        }
}