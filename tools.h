#ifndef APSC143__TOOLS_H
#define APSC143__TOOLS_H

#include <stdbool.h>

#define or ||
#define and &&
#define not !
#define elif else if

#define GRID_SIZE 8
#define BOARD_SIZE GRID_SIZE * GRID_SIZE

enum chess_player
{
    PLAYER_WHITE,
    PLAYER_BLACK,
    PLAYER_UNKNOWN = -1,
};

enum chess_piece
{
    PIECE_PAWN,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_ROOK,
    PIECE_QUEEN,
    PIECE_KING,
    PIECE_UNKNOWN = -1,
};

struct chess_board
{
    enum chess_player next_move_player;

    bool piece_present[BOARD_SIZE];

    enum chess_player piece_color[BOARD_SIZE];
    enum chess_piece piece_id    [BOARD_SIZE];

    bool white_can_castle;
    bool black_can_castle;

    int last_check_id;
};

struct chess_move
{
    enum chess_piece piece_id; // The moving piece id

    int from_square;     // From which square id is the piece moving from
    int to_square;       // To   which square id is the piece moving to
    
    bool is_capture;     // True if the moving piece is capturing a piece

    int promotes_to_id;  // If a pawn is promoted, the new piece id; -1 otherwise

    bool is_castle;      // True if the move is a castle
    bool is_long_castle; // True if the move is a long castle

    char from_file; // optional file disambiguation
    char from_rank; // optional rank disambiguation
};

struct dynamic_array 
{
    unsigned long *values;
    unsigned int current_index;
};

int from_cords(int x, int y);
bool from_id(int id, int *x, int *y);
int from_code(char *code);
char *square_string(int id);

void display_board(struct chess_board board);

unsigned long hash(unsigned char *str);

struct dynamic_array *init_dynamic();
bool append_dynamic(      struct dynamic_array *arr, unsigned long value);
bool search_dynamic(const struct dynamic_array *arr, unsigned long value);
void free_dynamic(struct dynamic_array *arr);
void print_dynamic(struct dynamic_array *arr);

void parse_error(char c, char *debug);

void print_move(struct chess_move move);

void create_board_copy(const struct chess_board *board, struct chess_board *cpy);

#endif
