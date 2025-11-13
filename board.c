#include "board.h"
#include "tools.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_DEPTH 3 // TODO: Increase
#define KING_X_LOCATION 4

char player_char(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return 'w';
    case PLAYER_BLACK:
        return 'b';
    case PLAYER_UNKNOWN:
        return '?';
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
    case PIECE_UNKNOWN:
        return '?';
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
    case PLAYER_UNKNOWN:
        return "?????";
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
    {
        board->piece_present[i] = false;
        board->piece_color[i] = PLAYER_UNKNOWN;
        board->piece_id[i] = PIECE_UNKNOWN;
    }

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

    // printf("Legal Move at (%d, %d) with id: %d\n", x, y, id);

    append_dynamic(moves, id);

    return true;
};

bool check_for_castle(struct chess_board board, bool *castle_left, bool *castle_right)
{
    if (not board.white_can_castle and board.next_move_player == PLAYER_WHITE)
        return false;

    if (not board.black_can_castle and board.next_move_player == PLAYER_BLACK)
        return false;

    int y = board.next_move_player == PLAYER_WHITE ? 0 : GRID_SIZE - 1;

    if (player_in_check(&board, from_cords(KING_X_LOCATION, y)))
        return false;

    int rook_id = from_cords(0, y);

    if (castle_left != NULL)
        *castle_left = true;

    if (board.piece_present[rook_id] and board.piece_id[rook_id] == PIECE_ROOK)
        for (int x = 1; x < KING_X_LOCATION; x++)
            if (board.piece_present[from_cords(x, y)]) 
            {
                if (castle_left != NULL)
                    *castle_left = false;
                break;
            }

    rook_id = from_cords(GRID_SIZE - 1, y);

    if (castle_right != NULL)
        *castle_right = true;

    if (board.piece_present[rook_id] and board.piece_id[rook_id] == PIECE_ROOK)
        for (int x = KING_X_LOCATION + 1; x < GRID_SIZE; x++)
            if (board.piece_present[from_cords(x, y)]) 
            {
                if (castle_right != NULL)
                    *castle_right = false;

                break;
            }
    
    return *castle_left or *castle_right;
}

struct dynamic_array *generate_legal_moves(enum chess_piece piece, struct chess_board board, int id)
{
    int x, y;
    if (not from_id(id, &x, &y))
        return NULL;

    struct dynamic_array *moves = init_dynamic();
    if (moves == NULL)
        return NULL;

    int i;

    enum chess_player player = board.piece_color[id];

    switch (piece)
    {
    case PIECE_PAWN:
        int dir = player == PLAYER_WHITE ? 1 : -1;

        int pawn_spawn = player == PLAYER_WHITE ? 1 : GRID_SIZE - 2;

        if (not board.piece_present[from_cords(x, y + dir)]) // If no piece in front of the pawn
            add_move(moves, board, x, y + dir, player);

        if (y == pawn_spawn and not board.piece_present[from_cords(x, y + 2 * dir)]) // Two moves off spawn
            add_move(moves, board, x, y + 2 * dir, player);
        
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

        bool castle_left, castle_right;

        if (check_for_castle(board, &castle_left, &castle_right))
        {
            int y = player == PLAYER_WHITE ? 0 : GRID_SIZE - 1;

            if (castle_left)
                add_move(moves, board, KING_X_LOCATION + 1, y, player);

            if (castle_right)
                add_move(moves, board, KING_X_LOCATION - 1, y, player);
        }

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

        bool left_castle, right_castle;

        if (check_for_castle(board, &left_castle, &right_castle))
        {
            int y = player == PLAYER_WHITE ? 0 : GRID_SIZE - 1;

            if (left_castle)
                add_move(moves, board, x - 2, y, player);
            if (right_castle)
                add_move(moves, board, x + 2, y, player);
        }

        break;
    }
    

    return moves;
}

static void move_piece(struct chess_board *board, int from, int to) {
    board->piece_id[to] = board->piece_id[from];
    board->piece_color[to] = board->piece_color[from];
    board->piece_present[to] = true;
    board->piece_present[from] = false;
}

// fill in missing info in a parsed board state, report move completion error if cant be determined 
void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    enum chess_player color = board->next_move_player; 
    int to_id = move->to_square;

    // castling
    if (move->piece_id == PIECE_KING) {
        int home_y = color == PLAYER_WHITE ? 0 : GRID_SIZE-1;
        int king_home = from_cords(KING_X_LOCATION, home_y);

        if ((to_id == from_cords(6, home_y) or to_id == from_cords(2, home_y)) and
            board->piece_id[king_home] == PIECE_KING and
            board->piece_color[king_home] == color) {
            move->from_square = king_home;
            return;
        }
    }

    int candidate = -1;

    struct dynamic_array *legal;

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (not board->piece_present[i]) continue;
        if (board->piece_color[i] != color) continue;
        if (board->piece_id[i] != move->piece_id) continue;

        legal = generate_legal_moves(move->piece_id, *board, i);
        if (!legal) continue;

        for (int j = 0; j < legal->current_index; j++) {
            if (legal->values[j] == to_id) {
                candidate = i;
                break;
            }
        }
    }
    
    free_dynamic(legal);

    if (candidate != -1) {
        move->from_square = candidate;
    } else {
        printf("move completion error: %s %s to %s\n",
        color_string(color),
        piece_string(move->piece_id),
        square_string(to_id));
    }
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    int from_id = move->from_square;
    int to_id   = move->to_square;
    
    enum chess_piece piece = move->piece_id;
    enum chess_player color = board->piece_color[from_id];

    move_piece(board, from_id, to_id);

    // promotion 
    if (move->promotes_to_id != -1) {
        board->piece_id[to_id] = move->promotes_to_id;
    }

    // castling
    if (piece == PIECE_KING) {
        bool castle_left = false, castle_right = false;

        check_for_castle(*board, &castle_left, &castle_right);

        int home_y = (color == PLAYER_WHITE) ? 0 : GRID_SIZE - 1;

        // king-side castle
        if (castle_right and to_id == from_cords(6, home_y)) {
            move_piece(board, from_cords(7, home_y), from_cords(5, home_y));
            if (color == PLAYER_WHITE) board->white_can_castle = false;
            else board->black_can_castle = false;
        }

        // queen-side castle
        if (castle_left and to_id == from_cords(2, home_y)) {
            move_piece(board, from_cords(0, home_y), from_cords(3, home_y));
            if (color == PLAYER_WHITE) board->white_can_castle = false;
            else board->black_can_castle = false;
        }

        if (color == PLAYER_WHITE) board->white_can_castle = false;
        else board->black_can_castle = false;
    }

    if (piece == PIECE_ROOK) {
        if (color == PLAYER_WHITE and (from_id == from_cords(0,0) or from_id == from_cords(7,0)))
            board->white_can_castle = false;
        if (color == PLAYER_BLACK and (from_id == from_cords(0, GRID_SIZE-1) or from_id == from_cords(7, GRID_SIZE-1)))
            board->black_can_castle = false;
    }

    // switch player
    board->next_move_player = (color == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
}

bool player_in_check(const struct chess_board *board, int id_to_check)
{
    struct dynamic_array *attacking_squares = init_dynamic();

    enum chess_player player_color = board->piece_color[id_to_check];

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board->piece_present[i] and board->piece_color[i] != player_color)
        {
            attacking_squares = init_dynamic();

            attacking_squares = generate_legal_moves(board->piece_id[i], *board, i);
            for (int i = 0; i < attacking_squares->current_index; i++) 
            {
                if (attacking_squares->values[i] == id_to_check) 
                {
                    free_dynamic(attacking_squares);
                    return true;
                }
            }

            free_dynamic(attacking_squares);
        }
    }

    return false;
}

// Classify the state of the board, printing one of the following:
// - game incomplete
// - white wins by checkmate
// - black wins by checkmate
// - draw by stalemate
void board_summarize(const struct chess_board *board)
{
    for (int depth = 0; depth < MAX_DEPTH; depth++) 
    {

    }
}
