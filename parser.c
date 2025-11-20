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

    char c = getc(stdin);

    if (c == '\n' or c == '\r' or c == EOF) {
        ungetc(c, stdin);
    // Check if we are at the end of input.
    if (c == '\n' or c == '\r')
        return false;
    }

    if (c == 'O') {
        ungetc(c, stdin);
        parse_castle(move);
        assert(move->piece_id == PIECE_KING);
        return true;
    }

    if (is_piece(c)) {
        switch (c) {
            case 'N': move->piece_id = PIECE_KNIGHT; break;
            case 'B': move->piece_id = PIECE_BISHOP; break;
            case 'R': move->piece_id = PIECE_ROOK; break;
            case 'Q': move->piece_id = PIECE_QUEEN; break;
            case 'K': move->piece_id = PIECE_KING; break;
            default: 
                parse_error(c, "Move->Piece");
                return false;
        }

        char next1 = getc(stdin);
        char next2 = getc(stdin);
        
        if (next2 == EOF) {
            parse_error(next1, "square incomplete");
            return false;
        }

        if ((is_file(next1) and is_rank(next2)) or (next1 == 'x' and is_file(next2))) {
            ungetc(next2, stdin);
            ungetc(next1, stdin);
            
            skip_spaces();
            char capture_char = getc(stdin);
            if (capture_char == 'x') {
                move->is_capture = true;
                skip_spaces();
            } else {
                move->is_capture = false;
                ungetc(capture_char, stdin);
            }
            
            move->to_square = parse_square(0);
        }
        else if (is_file(next1) and is_file(next2)) {
            move->from_file = next1;
            ungetc(next2, stdin);
            
            skip_spaces();
            char capture_char = getc(stdin);
            if (capture_char == 'x') {
                move->is_capture = true;
                skip_spaces();
            } else {
                move->is_capture = false;
                ungetc(capture_char, stdin);
            }
            
            move->to_square = parse_square(0);
        }
        else if (is_rank(next1) and is_file(next2)) {
            move->from_rank = next1;
            ungetc(next2, stdin);
            
            skip_spaces();
            char capture_char = getc(stdin);
            if (capture_char == 'x') {
                move->is_capture = true;
                skip_spaces();
            } else {
                move->is_capture = false;
                ungetc(capture_char, stdin);
            }
            
            move->to_square = parse_square(0);
        }
        else {
            parse_error(next1, "invalid move format");
            return false;
        }

        skip_spaces();
        move->promotes_to_id = parse_promotion();

        return true;
    }

    if (is_file(c)) {
        move->piece_id = PIECE_PAWN;
        move->from_file = c;

        skip_spaces();
        char next_char = getc(stdin);
        if (next_char == 'x') {
            move->is_capture = true;
            skip_spaces();
            move->to_square = parse_square(0);
        } else {
            move->is_capture = false;
            ungetc(next_char, stdin);
            move->to_square = parse_square(c);
        }

        move->promotes_to_id = parse_promotion();
        return true;
    }

    parse_error(c, "parse move");
    move->from_square = -1;
    while ((c = getc(stdin)) != '\n' and c != EOF and c != ' ') { ; }
    return false;
}


// void test_parser() {
//     struct chess_board board;
//     board_initialize(&board);
//     struct chess_move move;

//     printf("Enter a move: ");

//     if (parse_move(&move)) {
//         printf("Parsed: piece=%d, to=%d, capture=%s\n",
//                move.piece_id,
//                move.to_square,
//                move.is_capture ? "yes" : "no");

//         board_complete_move(&board, &move);

//         if (move.from_square != -1) {
//             printf("Move completion successful: from %d to %d\n",
//                    move.from_square,
//                    move.to_square);
//             board_apply_move(&board, &move);
//             printf("Move applied.\n");
//         } else {
//             printf("Move completion failed!\n");
//         }
//     } else {
//         printf("Parse failed\n");
//     }
// }
