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
        10, 10, 10, 05, 05, 15, 10, 10,     // F-pawn protects king diagonal
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
         00, 00,10, 00, 00, 00,10, 00,    // Home rank (castling on c1,g1)
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000     // Enemy rank
};

int Black_kingScore[64] = {
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,     // Enemy rank
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
      -1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
         00, 00,10, 00, 00, 00,10, 00      // Home rank (castling on c8,g8)
};

int eval_material(bool color) {
        int score = 0;

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
                        case 4:
                                numQueens++;
                        case 5:
                                break;
                }
        }

        score = (numPawns * materialValue[0]) + 
                (numKnights * materialValue[1]) + 
                (numBishops * materialValue[2]) + 
                (numRooks + materialValue[3]) + 
                (numQueens * materialValue[4]);

    return score;
}

// 0 = white, 1 = black.
float eval_squares() {
        float white_score = 0.0f;
        float black_score = 0.0f;
        float score = 0.0f;

        struct chess_board board;

        // White Pawn
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_PAWN) {
                                white_score = white_score + (materialValue[PIECE_PAWN] * White_pawnScore[i]);
                        }
                }
        }

        // White Knights
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_KNIGHT) {
                                white_score = white_score + (materialValue[PIECE_KNIGHT] * White_knightScore[i]);
                        }
                }
        }

        // White Bishops
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_BISHOP) {
                                white_score = white_score + (materialValue[PIECE_BISHOP] * White_bishopScore[i]);
                        }
                }
        }

        // White Rooks
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_ROOK) {
                                white_score = white_score + (materialValue[PIECE_ROOK] * White_rookScore[i]);
                        }
                }
        }

        // White Queens
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_QUEEN) {
                                white_score = white_score + (materialValue[PIECE_QUEEN] * White_queenScore[i]);
                        }
                }
        }

        // White King
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && !board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_KING) {
                                white_score = white_score + (materialValue[PIECE_KING] * White_kingScore[i]);
                        }
                }
        }

        // Black Pawn
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_PAWN) {
                                black_score = black_score + (materialValue[PIECE_PAWN] * Black_pawnScore[i]);
                        }
                }
        }

        // Black Knight
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_KNIGHT) {
                                black_score = black_score + (materialValue[PIECE_KNIGHT] * Black_knightScore[i]);
                        }
                }
        }

        // Black Rook
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_ROOK) {
                                black_score = black_score + (materialValue[PIECE_ROOK] * Black_rookScore[i]);
                        }
                }
        }

        // Black Queen
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_QUEEN) {
                                black_score = black_score + (materialValue[PIECE_QUEEN] * Black_queenScore[i]);
                        }
                }
        }

        // Black King
        for(int i = 0; i < 64; i++) {
                if (board.piece_present[i] && board.piece_color[i]) {
                        if (board.piece_id[i] == PIECE_KING) {
                                black_score = black_score + (materialValue[PIECE_KING] * Black_kingScore[i]);
                        }
                }
        }

        score = white_score - black_score;
        return score;
}