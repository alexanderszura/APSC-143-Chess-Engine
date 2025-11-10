#include "tools.h"
#include <stdbool.h>
#include "board.h"
#include "stdio.h"

int from_cords(int x, int y)
{
    int id = y * GRID_SIZE + x;

    if (id < 0 or id >= BOARD_SIZE)
    {
        printf("from_cords returned an invalid id of \"%d\" with the values x: %d and y: %d\n", id, x, y);
        return -1;
    }

    return id;
}

bool from_id(int id, int *x, int *y)
{
    if (id < 0 or id >= BOARD_SIZE)
    {
        printf("from_id was given an invalid id of \"%d\"\n", id);
        return false;
    }

    *y = id / GRID_SIZE;
    *x = id % GRID_SIZE;

    return true;
}

int from_code(char *code)
{
    char letter = *code++;
    char number = *code - '0';

    int i = 0;
    for (int c = 'A'; c <= 'H'; c++) {
        if (c == letter)
            return from_cords(i, number - 1);
        i++;
    }

    i = 0;
    for (int c = 'a'; c <= 'h'; c++) {
        if (c == letter)
            return from_cords(i, number - 1);
        i++;
    }

    return 0;
}

void display_board(struct chess_board board)
{
    for (int y = GRID_SIZE - 1; y >= 0; y--)
    {
        for (int x = 0; x < GRID_SIZE; x++) {
            int i = from_cords(x, y);

            if (board.piece_present[i])
                printf("[%c-%c]", player_char(board.piece_color[i]), piece_char(board.piece_id[i]));
            else
                printf("[   ]");
        }

        printf("\n");
    }
}