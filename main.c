#include "board.h"
#include "parser.h"
#include "tools.h"

// void test_parser();

int main()
{
    // test_parser();
    
    // Then run the normal chess game
    struct chess_board board;
    board_initialize(&board);

    display_board(board);

    generate_legal_moves(PIECE_QUEEN, board, from_cords(3, 3));

    struct chess_move move;
    while (parse_move(&move))
    {
        board_complete_move(&board, &move);
        board_apply_move(&board, &move);
    }

    board_summarize(&board);
    return 0;
}
