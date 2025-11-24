#include "parser.h"
#include "parser_helpers.h"
#include <stdio.h>
#include "panic.h"
#include <assert.h>

bool parse_move(struct chess_move *move)
{
    assert(move != NULL);

    reset_fields(move);
    skip_spaces();

    int c = getc(stdin);
    if (c == '\n' or c == '\r' or c == EOF) {
        ungetc(c, stdin);
        if (c == '\n' or c == '\r') return false;
    }

    if (c == 'O') {
        ungetc(c, stdin);
        parse_castle(move);
        return true;
    }

    if (is_piece(c)) {
        switch (c) {
            case 'N': move->piece_id = PIECE_KNIGHT; break;
            case 'B': move->piece_id = PIECE_BISHOP; break;
            case 'R': move->piece_id = PIECE_ROOK; break;
            case 'Q': move->piece_id = PIECE_QUEEN; break;
            case 'K': move->piece_id = PIECE_KING; break;
            default: parse_error(c, "Move->Piece"); return false;
        }

        char dis_file = '\0', dis_rank = '\0';
        int consumed = parse_disambiguation(&dis_file, &dis_rank);
        
        if (dis_file) move->from_file = dis_file;
        if (dis_rank) move->from_rank = dis_rank;

        skip_spaces();
        int capture_char = getc(stdin);
        if (capture_char == 'x') {
            move->is_capture = true;
            skip_spaces();
        } else {
            move->is_capture = false;
            ungetc(capture_char, stdin);
        }

        move->to_square = parse_square(0);

        skip_spaces();
        move->promotes_to_id = parse_promotion();
        return true;
    }

    if (is_file(c)) {
        move->piece_id = PIECE_PAWN;
        
        char next_char = getc(stdin);
        
        if (next_char == 'x') {
            move->from_file = c;
            move->is_capture = true;
            skip_spaces();
            move->to_square = parse_square(0);
        } else {
            ungetc(next_char, stdin);
            ungetc(c, stdin);
            move->to_square = parse_square(0);
            move->is_capture = false;
        }

        skip_spaces();
        move->promotes_to_id = parse_promotion();
        return true;
    }

    parse_error(c, "parse move");
    move->from_square = -1;
    while ((c = getc(stdin)) != '\n' and c != EOF) { 
        ; 
    }
    return false;
}