#include "board.h"
#include "tools.h"
#include "stdio.h"
#include "stdlib.h"

const char *player_string(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return "white";
    case PLAYER_BLACK:
        return "black";
    }
}

char player_char(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return 'w';
    case PLAYER_BLACK:
        return 'b';
    }
}

const char *piece_string(enum chess_piece piece)
{
    switch (piece)
    {
    case PIECE_PAWN:
        return "pawn";
    case PIECE_KNIGHT:
        return "knight";
    case PIECE_BISHOP:
        return "bishop";
    case PIECE_ROOK:
        return "rook";
    case PIECE_QUEEN:
        return "queen";
    case PIECE_KING:
        return "king";
    }
}

char piece_char(enum chess_piece piece)
{
    switch (piece)
    {
    case PIECE_PAWN:
        return 'p';
    case PIECE_KNIGHT:
        return 'n';
    case PIECE_BISHOP:
        return 'b';
    case PIECE_ROOK:
        return 'r';
    case PIECE_QUEEN:
        return 'q';
    case PIECE_KING:
        return 'k';
    }
}

const char *color_string(enum chess_player color)
{
    switch (color)
    {
    case PLAYER_BLACK:
        return "BLACK";
    case PLAYER_WHITE:
        return "WHITE";
    default:
        return "UNKNOWN";
    }
}

void board_initialize(struct chess_board *board)
{
    board->next_move_player = PLAYER_WHITE;
    
    board->white_can_castle = true;
    board->black_can_castle = true;

    for (int i = 0; i < BOARD_SIZE; i++)
        board->piece_present[i] = false;

    /* --------------- Setup Pawns --------------- */
    int white_cord = from_cords(0, 1);
    int black_cord = from_cords(0, GRID_SIZE - 2);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        board->piece_id[white_cord] = PIECE_PAWN;
        board->piece_id[black_cord] = PIECE_PAWN;

        board->piece_color[white_cord] = PLAYER_WHITE;
        board->piece_color[black_cord] = PLAYER_BLACK;

        board->piece_present[white_cord++] = true;
        board->piece_present[black_cord++] = true;
    }
    /* --------------- Setup Pawns --------------- */

    /* --------------- Bottom Rank --------------- */
    enum chess_piece bottom_rank[] = {PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_QUEEN, PIECE_KING, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK};

    white_cord = from_cords(0, 0);
    black_cord = from_cords(0, GRID_SIZE - 1);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        board->piece_id[white_cord] = bottom_rank[i];
        board->piece_id[black_cord] = bottom_rank[i];

        board->piece_color[white_cord] = PLAYER_WHITE;
        board->piece_color[black_cord] = PLAYER_BLACK;

        board->piece_present[white_cord++] = true;
        board->piece_present[black_cord++] = true;
    }
    /* --------------- Bottom Rank --------------- */
}

bool add_move(struct dynamic_array *moves, struct chess_board board, int x, int y, enum chess_player color)
{
    if (x < 0 or x >= GRID_SIZE or y < 0 or y >= GRID_SIZE)
        return false;

    int id = from_cords(x, y);

    if (board.piece_present[id] and board.piece_color[id] == color)
        return false;

    printf("Legal Move at (%d, %d) with id: %d\n", x, y, id);

    append_dynamic(moves, id);

    return true;
};

unsigned int *generate_legal_moves(enum chess_piece piece, struct chess_board board, int id)
{
    int x, y;
    if (not from_id(id, &x, &y))
        return NULL;

    struct dynamic_array *moves;
    init_dynamic(moves);

    int i;

    enum chess_player player = board.piece_color[id];

    switch (piece)
    {
    case PIECE_PAWN:
        int dir = board.next_move_player == PLAYER_WHITE ? 1 : -1;

        if (not board.piece_present[from_cords(x, y + dir)]) // If no piece in front of the pawn
            add_move(moves, board, x, y + dir, player);
        
        if (x > 0 and board.piece_present[from_cords(x - 1, y + dir)]) // Capturing Left side
            add_move(moves, board, x - 1, y + dir, player);

        if (x < GRID_SIZE - 1 and board.piece_present[from_cords(x + 1, y + dir)]) // Capturing Right side
            add_move(moves, board, x + 1, y + dir, player);

        // TODO: Add en passant
        break;

    case PIECE_KNIGHT:
        add_move(moves, board, x + 1, y + 2, player);
        add_move(moves, board, x - 1, y + 2, player);
        add_move(moves, board, x + 1, y - 2, player);
        add_move(moves, board, x - 1, y - 2, player);

        add_move(moves, board, x + 2, y + 1, player);
        add_move(moves, board, x - 2, y + 1, player);
        add_move(moves, board, x + 2, y - 1, player);
        add_move(moves, board, x - 2, y - 1, player);
        break;

    case PIECE_BISHOP:
        i = 1;
        do {
            if (not add_move(moves, board, x + i, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y - i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x + i, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y - i)]);

        break;

    case PIECE_ROOK:
        i = 1;
        do {
            if (not add_move(moves, board, x + i, y, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y)]);

        i = 1;
        do {
            if (not add_move(moves, board, x, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x, y - i)]);

        break;
    case PIECE_QUEEN:
        i = 1;
        do {
            if (not add_move(moves, board, x + i, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y - i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x + i, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y - i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x + i, y, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x + i, y)]);

        i = 1;
        do {
            if (not add_move(moves, board, x - i, y, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x - i, y)]);

        i = 1;
        do {
            if (not add_move(moves, board, x, y + i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x, y + i)]);

        i = 1;
        do {
            if (not add_move(moves, board, x, y - i, player)) break;
            i++;
        } while (not board.piece_present[from_cords(x, y - i)]);

        break;
    case PIECE_KING:
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 and dy == 0) continue;

                add_move(moves, board, x + dx, y + dy, player);
            }
        }

        break;
    }
    

    return (unsigned int *)(moves->values);
}

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    // TODO: complete the move.
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    // TODO: apply a completed move to the board.

    // The final step is to update the the turn of players in the board state.
    switch (board->next_move_player)
    {
    case PLAYER_WHITE:
        board->next_move_player = PLAYER_BLACK;
        break;
    case PLAYER_BLACK:
        board->next_move_player = PLAYER_WHITE;
        break;
    }
}

void board_summarize(const struct chess_board *board)
{
    // TODO: print the state of the game.
}
