#include "parser.h"
#include "parser_helpers.h"
#include <stdio.h>
#include "panic.h"
#include <assert.h>

/// @brief parses chess moves from standard input
/// @param move current move from standard input to be parsed (parsed data stored in chess_move struct fields)
/// @note helpers in "parser_helpers.c" were used for code organization
/// @return true if a valid move was successfully parsed, false otherwise 
bool parse_move(struct chess_move *move)
{
    // ensure the move pointer is valid, reset all current fields in the move struct, and skip any leading whitespace before parsing.
    assert(move != NULL);
    reset_fields(move);
    skip_spaces();

    // read the first character of the move while handling empty lines or EOF. 
    int c = getc(stdin);
    if (c == '\n' or c == '\r' or c == EOF) {
        ungetc(c, stdin); // push the character back for future processing.
        if (c == '\n' or c == '\r') return false;
    }

    // check for castling notation (O-O or O-O-O)
    if (c == 'O') {
        ungetc(c, stdin);
        parse_castle(move);
        return true;
    }

    // check if the character corresponds to a piece and map it to an internal piece ID.
    if (is_piece(c)) {
        switch (c) {
            case 'N': move->piece_id = PIECE_KNIGHT; break;
            case 'B': move->piece_id = PIECE_BISHOP; break;
            case 'R': move->piece_id = PIECE_ROOK; break;
            case 'Q': move->piece_id = PIECE_QUEEN; break;
            case 'K': move->piece_id = PIECE_KING; break;
            default: parse_error(c, "Move->Piece"); return false; // invalid piece character. 
        }

        // handle disambiguation. 
        char dis_file = '\0', dis_rank = '\0'; // variables to store disambiguation (file & rank)
        int consumed = parse_disambiguation(&dis_file, &dis_rank); 
        
        // disambiguate if necessary.
        if (dis_file) move->from_file = dis_file;
        if (dis_rank) move->from_rank = dis_rank;

        // handle capture moves ('x').
        skip_spaces();
        int capture_char = getc(stdin);
        if (capture_char == 'x') { // check for capture flag ('x')
            move->is_capture = true;
            skip_spaces();
        } else {
            move->is_capture = false;
            ungetc(capture_char, stdin); // not a capture, push back character. 
        }

        move->to_square = parse_square(0); // parse destination square.

        skip_spaces();
    
        move->promotes_to_id = parse_promotion(); // parse promotion piece, if it exists. 
        return true;
    }

    // handle pawn moves; first character is a file letter.
    if (is_file(c)) {
        move->piece_id = PIECE_PAWN;
        
        char next_char = getc(stdin);
        
        if (next_char == 'x') { // pawn capture
            move->from_file = c;
            move->is_capture = true;
            skip_spaces();
            move->to_square = parse_square(0);
        } else {
            ungetc(next_char, stdin); // not a pawn, push back character.
            ungetc(c, stdin); // push back file character.

            move->to_square = parse_square(0);
            move->is_capture = false;
        }

        skip_spaces();

        // parse promotion if it exists
        move->promotes_to_id = parse_promotion();
        return true;
    }

    // if none of the conditions are met, the input is invalid -> output error message.
    parse_error(c, "parse move");
    move->from_square = -1; // invalid move indication. 

    // consume characters to recover from invalid input.
    while ((c = getc(stdin)) != '\n' and c != EOF) { 
        ; 
    }
    return false;
}