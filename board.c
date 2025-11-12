#include "board.h"
#include "tools.h"
#include "stdio.h"

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

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    // TODO: complete the move.
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    int from_index = move->from_square;
    int to_index = move->to_square;

    // move
    board->piece_id[to_index] = move->piece_id;
    board->piece_color[to_index] = board->piece_color[from_index];
    board->piece_present[to_index] = true;
    board->piece_present[from_index] = false;

    // promotion 
    if (move->promotes_to_id != -1) {
        board->piece_id[to_index] = move->promotes_to_id;
    }

    // TODO: handle castling



    // The final step is to update the the turn of players in the board state.
    board->next_move_player = (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

}

void board_summarize(const struct chess_board *board)
{
    // TODO: print the state of the game.
}
