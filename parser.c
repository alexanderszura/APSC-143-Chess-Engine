#include "parser.h"
#include <stdio.h>
#include "panic.h"

bool parse_move(struct chess_move *move)
{
    char c;

    // Get the first character of the move, ignoring any initial spaces.
    do
    {
        c = getc(stdin);
    } while (c == ' ');

    // Check if we are at the end of input.
    if (c == '\n' || c == '\r')
        return false;

    switch (c)
    {
    // TODO: parse the move, starting from the first character. You are free to
    // start from this switch/case as a template or use a different approach.
    default:
        panicf("parse error at character '%c'\n", c);
    }
}

bool check_move(struct chess_move move, struct chess_board board) {
    // Illegal castling.
    if (move.is_castle || move.is_long_castle) {
        switch(board.next_move_player) {
            case PLAYER_WHITE: // 0
                if (!board.white_can_castle) {
                    printf("%d", "Move Completion Error: WHITE KING to " + move.to_square);
                    return false;
                }
            case PLAYER_BLACK: // 1
                if (!board.black_can_castle) {
                    printf("%d", "Move Completion Error: BLACK KING to " + move.to_square);
                    return false;
                }
            }
        }

    // Illegal move - move on occupied square (without taking)
    

}
