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
    char c;
    while ((c = getc(stdin)) == ' ') {
    ;}
    ungetc(c, stdin);

}

void expect_char(char expected) {
    char c = getc(stdin);
    if (c != expected) {
        parse_error(c, "expected");
    }
}

int parse_square() {
    char file = getc(stdin);
    char rank = getc(stdin);

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
    if (!is_piece(c) || c == 'K') parse_error(c, "pawn-promote"); // cannot promote 2 king

    switch(c) {
        case 'Q': return PIECE_QUEEN;
        case 'R': return PIECE_ROOK;
        case 'B': return PIECE_BISHOP;
        case 'N': return PIECE_KNIGHT;
    }
    return -1; 
}

void parse_castle(struct chess_move *move) {
    expect_char('O');
    expect_char('-');
    expect_char('O');

    char c = getc(stdin);
    if (c == '-') {
        expect_char('O');
        move->is_long_castle = true;
    } else {
        ungetc(c, stdin);
        move->is_long_castle = false;
    }
    move->is_castle = true;
    move->piece_id = PIECE_KING;
    // to_square will be set in board_complete_move based on current player and castle type
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