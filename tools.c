#include "tools.h"
#include "stdbool.h"
#include "board.h"
#include "stdio.h"
#include "stdlib.h"
#include "panic.h"

#define DEBUG true

int from_cords(int x, int y)
{
    return y * GRID_SIZE + x;
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

    return -1;
}

const char *square_string(int id)
{
    int x, y;
    from_id(id, &x, &y);

    char letter = 'a' + x;
    char num = '1' + y;

    char *str = malloc(3 * sizeof(char));
    str[0] = letter;
    str[1] = num;
    str[2] = '\0';

    return str;
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

void print_dynamic(struct dynamic_array *arr)
{
    if (arr->values == NULL) return;

    printf("(%d", arr->values[0]);

    for (unsigned int i = 1; i < arr->current_index; i++)
    {
        printf(", %d", arr->values[i]);
    }
    printf(")\n");
}

/**
 * York University djb2 algorithm
 * http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

struct dynamic_array *init_dynamic()
{
    struct dynamic_array *arr = malloc(sizeof(struct dynamic_array));
    if (arr == NULL)
        return NULL;
    
    arr->values = NULL;
    arr->current_index = 0;
    return arr;
}

bool append_dynamic(struct dynamic_array *arr, unsigned long value)
{
    if (arr == NULL)
        return false;

    if (arr->values == NULL)
    {
        arr->values = malloc(sizeof(unsigned long));
        arr->current_index = 0;

        if (arr->values == NULL) 
        {
            printf("Error mallocating memory with value: %lu", value);
            return false;
        }
    } else {
        unsigned long *temp = realloc(arr->values, (arr->current_index + 1) * sizeof(unsigned long));

        if (temp == NULL) 
        {
            printf("Error reallocating memory with value: %lu at index: %u", value, arr->current_index);
            return false;
        }

        arr->values = temp;
    }

    arr->values[arr->current_index++] = value;

    return true;
}

bool search_dynamic(const struct dynamic_array *arr, unsigned long value)
{
    for (unsigned int i = 0; i < arr->current_index - 1; i++)
        if (arr->values[i] == value)
            return true;

    return false;
}

void free_dynamic(struct dynamic_array *arr)
{
    if (arr == NULL)
        return;
    
    if (arr->values != NULL)
        free(arr->values);
    
    free(arr);
}

void parse_error(char c, char *debug) 
{
    if (DEBUG)
        puts(debug);
    panicf("parse error at character '%c'\n", c);
}

void print_move(struct chess_move move)
{
    printf("from_square: %d, to_square: %d, piece_id: %d, is_capture: %d, is_castle: %d, is_long_castle: %d, promotes_to: %d",
        move.from_square,
        move.to_square,
        move.piece_id,
        move.is_capture,
        move.is_castle,
        move.is_long_castle,
        move.promotes_to_id
    );
}
