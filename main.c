#include "board.h"
#include "parser.h"
#include "tools.h"
#include "stdio.h"

// void test_parser();

int main()
{    
    // Then run the normal chess game
    struct chess_board board;
    board_initialize(&board);

    display_board(board);

    struct chess_move move;
    while (parse_move(&move, &board))
    {
        board_complete_move(&board, &move);
        board_apply_move(&board, &move);
    }

    display_board(board);

    board_summarize(&board);
    return 0;
}
