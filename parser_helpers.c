#include "parser.h"
#include <stdio.h>

/*
*Helper functions for use & organization in main parser ("parser.c"). 

-> booleans to check if input is a file, rank, piece.
-> more helpers to handle specific cases like promotion, capture, castling, disambiguation.
-> other helpers for skipping spaces and struct field resetting.

*/

// checks if character is a valid file. ('a' to 'h)
bool is_file(char c) {
    if (c >= 'a' and c <= 'h') {
        return true; // valid file
    }
   return false;
}

// checks if character is a valid rank. (1 to 8)
bool is_rank(char c) {
    if (c >= '1' and c <= '8') {
        return true; // valid rank
    }
    return false;
}

// checks if character is a chess piece; letters correspond to different moves. (N - knight, K - king, etc)
bool is_piece(char c) {
    switch (c) {
        case 'N':
        case 'B':
        case 'R':
        case 'Q':
        case 'K':
            return true;
        default:
            return false; 
    }
}

// skips whitespace characters in input. 
void skip_spaces() {
    int c;
    while ((c = getc(stdin)) == ' ') {
        ; // read chararacters as long as they are spaces.
    }
    if (c != EOF) {
        ungetc(c, stdin); // if parsing stopped because of a non-EOF character, put it back. 
    }
}

// read the next character and verify it matches 'expected'; trigger a parse error if there isn't a match.
void expect_char(char expected) {
    char c = getc(stdin);
    if (c != expected) {
        parse_error(c, "expected"); // report mismatch.
    }
}

// parse a chessboard square (file+rank, e.g., "e4")
int parse_square(char pre_file) {
    char file, rank;
    
    // use provided file, otherwise read file from input.
    if (pre_file) {
        file = pre_file;
        rank = getc(stdin);
    } else {
        file = getc(stdin);
        rank = getc(stdin);
    }

    // validate file & rank.
    if (!is_file(file)) parse_error(file, "square->file");
    if (!is_rank(rank)) parse_error(rank, "square->rank");
    
    return (rank - '1') * 8 + (file - 'a'); // return the index converted file and rank.
}

// handle flagging for parsing for capture scenarios, called when 'x' is encountered. 
// returns true if capture was found, otherwise returns false and pushes back the character. 
bool parse_capture() {
    char c = getc(stdin);
    if (c == 'x') {
        return true;
    }
    ungetc(c, stdin);
    return false;
}

// handle parsing for promotion scenarios, called when the promotion symbol '=' is encountered. 
// returns the ID of the promoted piece, or -1 if no promotion exists. 
int parse_promotion() {
    char c = getc(stdin);
    if (c != '=') { // check for '=' sign (promotion symbol).
        ungetc(c, stdin);
        return -1;
    }
    c = getc(stdin); // read the piece to be promoted.
    if (!is_piece(c) or c == 'K') parse_error(c, "pawn-promote"); // promotion must be to a non-king piece.

    // return the corresponding internal piece ID.
    switch(c) {
        case 'Q': return PIECE_QUEEN;
        case 'R': return PIECE_ROOK;
        case 'B': return PIECE_BISHOP;
        case 'N': return PIECE_KNIGHT;
    }
    return -1; 
}

// handle parsing for castling scenario, called when the castling symbols 'O-O' 'O-O-O' are encountered.
// updated move fields accordingly. 
void parse_castle(struct chess_move *move) {
    
    // castling symbol. (O, then '-', ....)
    char c = getc(stdin);
    if (c != 'O') {
        parse_error(c, "expected 'O'");
        return;
    }
    
    c = getc(stdin);
    if (c != '-') {
        parse_error(c, "expected '-'");
        return;
    }
    
    c = getc(stdin);
    if (c != 'O') {
        parse_error(c, "expected 'O'");
        return;
    }

    c = getc(stdin);
    if (c == '-') {
        c = getc(stdin);
        if (c != 'O') {
            parse_error(c, "expected 'O'");
            return;
        }
        move->is_long_castle = true;
    } else {
        ungetc(c, stdin); // not another '-', so it's short castle.
        move->is_long_castle = false;
    }
    
    move->is_castle = true;
    move->piece_id = PIECE_KING;
    move->to_square = -1;  
}

// resets all fields in chess_move object to default state. 
void reset_fields(struct chess_move *move) {
    move->is_capture = false;
    move->is_castle = false;
    move->is_long_castle = false;
    move->promotes_to_id = PIECE_UNKNOWN;
    move->from_square = -1;
    move->to_square = -1;
    move->piece_id = PIECE_UNKNOWN;
    move->from_file = '\0';
    move->from_rank = '\0';
}

// parse optional piece disambiguation. (file/rank)
// used in moves like "Nbd2" (ambiguation is present);
// writes result to from_file & from_rank (pointers) and returns the number of characters consumed from the input. 
int parse_disambiguation(char *from_file, char *from_rank) {
    int c_consumed = 0; // storage for number of consumed characters. 
    
    int c = getc(stdin);
    if (c == EOF) return 0;
    
    // if disambiguation starts with the capture symbol 'x', it is not disambiguation at all.
    if (c == 'x') {
        ungetc(c, stdin);
        return 0;
    }
    
    // first case: disambiguation by file. 
    if (is_file(c)) {
        // peek at next character without consuming.
        int next_c = getc(stdin);
        ungetc(next_c, stdin);
        ungetc(c, stdin);

        if (is_rank(next_c)) { // the parser will consume if "file + rank" so do nothing. 
            return 0;
        }
        
        // otherwise, consume the file disambiguation.
        c = getc(stdin); 
        if (from_file) *from_file = c;
        c_consumed++; // update storage/counter. 
        
        // look for optional rank disambiguation.
        c = getc(stdin);
        if (is_rank(c)) {
            if (from_rank) *from_rank = c;
            c_consumed++;
        } else {
            ungetc(c, stdin);
        }
    }
    // second case: disambiguation by rank only. 
    elif (is_rank(c)) {
        if (from_rank) *from_rank = c;
        c_consumed++;
    }
    else {
        ungetc(c, stdin);
    }
    
    return c_consumed;
}