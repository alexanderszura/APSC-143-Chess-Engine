#include "board.h"
#include "parser.h"
#include "tools.h"
#include "stdio.h"

void test_parser();

int main()
{    
    // Then run the normal chess game
    struct chess_board board;
    board_initialize(&board);

    display_board(board);

    struct chess_move move;
    while (parse_move(&move))
    {
        board_complete_move(&board, &move);
        
        // Only apply move if completion was successful
        if (move.from_square != -1) {
            board_apply_move(&board, &move);
            // puts("----------------------------------------");
            // display_board(board);
        }
    }

    puts("----------------------------------------");
    display_board(board);

    puts("Summarizing Board");

    board_summarize(&board);

    puts("Succesfully finished the program");

    return 0;
}
