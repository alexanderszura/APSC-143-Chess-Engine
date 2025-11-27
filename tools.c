#include "tools.h"
#include "stdbool.h"
#include "board.h"
#include "stdio.h"
#include "stdlib.h"
#include "panic.h"

/// @brief From a set of coordinates returns an ID
/// @param x x coordinate
/// @param y y coordinate
/// @return the id of the coordinate
int from_cords(int x, int y)
{
    return y * GRID_SIZE + x;
}

/// @brief From an id return a set of coordinates
/// @param id the id to compute
/// @param x the x position to return
/// @param y the y position to return
/// @return true if valid coordinates and if set
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

/// @brief From a string code return an id
/// @param code the code to convert
/// @return the id of the string, -1 if invalid
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

/// @brief From an id return a string of the characater code
/// @param id the id to convert
/// @return the string, returns NULL if invalid
char *square_string(int id)
{
    int x, y;
    if (not from_id(id, &x, &y))
        return NULL;

    char letter = 'a' + x;
    char num = '1' + y;

    char *str = malloc(3 * sizeof(char));
    str[0] = letter;
    str[1] = num;
    str[2] = '\0';

    return str;
}

/// @brief Displays the board to the terminal
/// @param board the board to display
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

/// @brief Prints the dynamic array to the terminal for debugging
/// @param arr the array to print
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
 * @deprecated unused
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

/// @brief Initializes a dynamic_array, mallocates memory for the array
/// @return Returns a empty dynamic_array
struct dynamic_array *init_dynamic()
{
    struct dynamic_array *arr = malloc(sizeof(struct dynamic_array));
    if (arr == NULL)
        return NULL;
    
    arr->values = NULL;
    arr->current_index = 0;
    return arr;
}

/// @brief Append a value to a dyanmic_array
/// @param arr the array to append to
/// @param value the value to append
/// @return returns true if succesful, false if there was an issue
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

/// @brief Search a dynamic array for a specific value
/// @param arr the array to search
/// @param value the value to search for
/// @return true if found, false otherwise
/// @deprecated unused
bool search_dynamic(const struct dynamic_array *arr, unsigned long value)
{
    if (arr == NULL or arr->values == NULL)
        return false;
        
    for (unsigned int i = 0; i < arr->current_index; i++)
        if (arr->values[i] == value)
            return true;

    return false;
}

/// @brief Frees the memory of a dynamic_array
/// @param arr the dynamic_array to free
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

/// @brief Creates a deep copy of a board
/// @param board the board to copy from
/// @param cpy the new board to set
void create_board_copy(const struct chess_board *board, struct chess_board *cpy)
{
    cpy->last_check_id    = board->last_check_id;

    cpy->next_move_player = board->next_move_player;
    
    cpy->white_can_castle_left = board->white_can_castle_left;
    cpy->black_can_castle_left = board->black_can_castle_left;

    cpy->white_can_castle_right = board->white_can_castle_right;
    cpy->black_can_castle_right = board->black_can_castle_right;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cpy->piece_present[i] = board->piece_present[i];
        cpy->piece_color[i]   = board->piece_color[i];
        cpy->piece_id[i]      = board->piece_id[i];
    }

    cpy->pawn_double_file = board->pawn_double_file;
}
