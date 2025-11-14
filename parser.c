#include "parser.h"
#include "parser_helpers.h"
#include <stdio.h>
#include "panic.h"


bool parse_move(struct chess_move *move)
{
    reset_fields(move);
    skip_spaces();

    char c = getc(stdin);

    // Check if we are at the end of input.
    if (c == '\n' || c == '\r')
        return false;

    // castle check 
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
            default: parse_error(c);
        }

        move->from_file = '\0';
        move->from_rank = '\0';

        char next = getc(stdin);
        if (is_file(next)) {
            move->from_file = next;
            next = getc(stdin);
        }
        if (is_rank(next)) {
            move->from_rank = next;
            next = getc(stdin);
        }
        if (next != EOF) ungetc(next, stdin);

        move->is_capture = parse_capture();
        move->to_square = parse_square();
        move->promotes_to_id = parse_promotion();
        return true;
    }

    // pawn parsing 
    if (is_file(c)) {
        ungetc(c, stdin);
        if (move->piece_id == PIECE_UNKNOWN)
            move->piece_id = PIECE_PAWN;

        char start_file = getc(stdin);
        move->from_file = '\0';
        move->from_rank = '\0';

        if (parse_capture()) {
            move->is_capture = true;
            move->from_file = start_file;
            getc(stdin); 
        } else {
            ungetc(start_file, stdin);
            move->is_capture = false;
        }

        move->to_square = parse_square();
        move->promotes_to_id = parse_promotion();
        return true;
    }
    
    parse_error(c);
    return false;
}

void test_parser() {
    struct chess_move move;
    printf("Enter a move: ");
    if (parse_move(&move)) {
        printf("Parsed: piece=%d, to=%d, capture=%s, from_file=%c, from_rank=%c\n",
               move.piece_id, move.to_square,
               move.is_capture ? "yes" : "no",
               move.from_file ? move.from_file : '-',
               move.from_rank ? move.from_rank : '-');
    } else {
        printf("Parse failed\n");
    }
}
