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

        // determine typeof piece
        switch (c) {
            case 'N': move->piece_id = PIECE_KNIGHT; break;
            case 'B': move->piece_id = PIECE_BISHOP; break;
            case 'R': move->piece_id = PIECE_ROOK; break;
            case 'Q': move->piece_id = PIECE_QUEEN; break;
            case 'K': move->piece_id = PIECE_KING; break;
            default: parse_error(c);
        }

        // check for capture
        if (parse_capture()) {
            move->is_capture = true;
            getc(stdin);
        }

        move->to_square = parse_square();
        move->promotes_to_id = parse_promotion();

        return true;
    }

    // pawn parsing 
    if (is_file(c)) {
        ungetc(c, stdin);
        move->piece_id = PIECE_PAWN;

        char start_file = getc(stdin);
        if (parse_capture()) {
            move->is_capture = true;
            move->from_file = start_file;  
        } else {
            ungetc(start_file, stdin);
        }
        
        move->to_square = parse_square();
        move->promotes_to_id = parse_promotion();

        return true;
    }
    
    parse_error(c);
    return false;
}

void test_parser() {
    struct chess_board board;
    board_initialize(&board);
    struct chess_move move;

    printf("Enter a move: ");
    
    if (parse_move(&move)) {
        printf("Parsed: piece=%d, to=%d, capture=%s\n", 
               move.piece_id, move.to_square, move.is_capture ? "yes" : "no");
    } else {
        printf("Parse failed\n");
    }
}
