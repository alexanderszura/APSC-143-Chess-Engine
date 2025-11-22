#include "parser.h"
#include <stdio.h>

bool is_file(char c) {
    if (c >= 'a' and c <= 'h') {
        return true;
    }
   return false;
}

bool is_rank(char c) {
    if (c >= '1' and c <= '8') {
        return true;
    }
    return false;
}

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

void skip_spaces() {
    int c;
    while ((c = getc(stdin)) == ' ') {
        ; 
    }
    if (c != EOF) {
        ungetc(c, stdin);
    }
}

void expect_char(char expected) {
    char c = getc(stdin);
    if (c != expected) {
        parse_error(c, "expected");
    }
}

int parse_square(char pre_file) {
    char file, rank;
    
    if (pre_file) {
        file = pre_file;
        rank = getc(stdin);
    } else {
        file = getc(stdin);
        rank = getc(stdin);
    }

    if (!is_file(file)) parse_error(file, "square->file");
    if (!is_rank(rank)) parse_error(rank, "square->rank");
    
    return (rank - '1') * 8 + (file - 'a');
}

bool parse_capture() {
    char c = getc(stdin);
    if (c == 'x') {
        return true;
    }
    ungetc(c, stdin);
    return false;
}

int parse_promotion() {
    char c = getc(stdin);
    if (c != '=') {
        ungetc(c, stdin);
        return -1;
    }
    c = getc(stdin);
    if (!is_piece(c) or c == 'K') parse_error(c, "pawn-promote"); 

    switch(c) {
        case 'Q': return PIECE_QUEEN;
        case 'R': return PIECE_ROOK;
        case 'B': return PIECE_BISHOP;
        case 'N': return PIECE_KNIGHT;
    }
    return -1; 
}

void parse_castle(struct chess_move *move) {
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
        ungetc(c, stdin);
        move->is_long_castle = false;
    }
    
    move->is_castle = true;
    move->piece_id = PIECE_KING;
    move->to_square = -1;  
}

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

int parse_disambiguation(char *from_file, char *from_rank) {
    int c_consumed = 0;
    
    int c = getc(stdin);
    if (c == EOF) return 0;
    
    if (c == 'x') {
        ungetc(c, stdin);
        return 0;
    }
    
    if (is_file(c)) {
        int next_c = getc(stdin);
        ungetc(next_c, stdin);
        ungetc(c, stdin);

        if (is_rank(next_c)) {
            return 0;
        }
        
        c = getc(stdin); 
        if (from_file) *from_file = c;
        c_consumed++;
        
        c = getc(stdin);
        if (is_rank(c)) {
            if (from_rank) *from_rank = c;
            c_consumed++;
        } else {
            ungetc(c, stdin);
        }
    }
    elif (is_rank(c)) {
        if (from_rank) *from_rank = c;
        c_consumed++;
    }
    else {
        ungetc(c, stdin);
    }
    
    return c_consumed;
}