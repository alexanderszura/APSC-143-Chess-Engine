#include "board.h"
#include "parser.h"
#include "tools.h"



int main()
{
    test_parser();
    
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

    board_summarize(&board);
    return 0;
}
