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
    default:
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
    case PIECE_UNKNOWN:
    default:
        return "unknown";
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
    default:
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
    board->last_check_id = PIECE_UNKNOWN;

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
    if (castle_left != NULL)  *castle_left = false;
    if (castle_right != NULL) *castle_right = false;

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

struct dynamic_array *generate_legal_moves(enum chess_piece piece, struct chess_board board, int id, bool include_castle, bool remove_moves_when_checked)
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
        {
            add_move(moves, board, x, y + dir, player);

            if (y == pawn_spawn and not board.piece_present[from_cords(x, y + 2 * dir)])
                add_move(moves, board, x, y + 2 * dir, player);
        }
        
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
        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y + i, player)) break;
        } while (not board.piece_present[from_cords(x + i, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x - i, y + i, player)) break;
        } while (not board.piece_present[from_cords(x - i, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x - i, y - i, player)) break;
        } while (not board.piece_present[from_cords(x - i, y - i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y - i, player)) break;
        } while (not board.piece_present[from_cords(x + i, y - i)]);

        break;

    case PIECE_ROOK:
        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y, player)) break;
        } while (not board.piece_present[from_cords(x + i, y)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x - i, y, player)) break;
        } while (not board.piece_present[from_cords(x - i, y)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x, y + i, player)) break;
        } while (not board.piece_present[from_cords(x, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x, y - i, player)) break;
        } while (not board.piece_present[from_cords(x, y - i)]);
    case PIECE_QUEEN:
        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y + i, player)) break;
        } while (not board.piece_present[from_cords(x + i, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x - i, y + i, player)) break;
        } while (not board.piece_present[from_cords(x - i, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x - i, y - i, player)) break;
        } while (not board.piece_present[from_cords(x - i, y - i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y - i, player)) break;
        } while (not board.piece_present[from_cords(x + i, y - i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x + i, y, player)) break;
        } while (not board.piece_present[from_cords(x + i, y)]);

        i = 0;
        do {     
            i++;
            if (not add_move(moves, board, x - i, y, player)) break;
        } while (not board.piece_present[from_cords(x - i, y)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x, y + i, player)) break;
        } while (not board.piece_present[from_cords(x, y + i)]);

        i = 0;
        do {
            i++;
            if (not add_move(moves, board, x, y - i, player)) break;
        } while (not board.piece_present[from_cords(x, y - i)]);

        break;
    case PIECE_KING:
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 and dy == 0) continue;

                add_move(moves, board, x + dx, y + dy, player);
            }
        }

        if (not include_castle)
            break;

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
    
    if (remove_moves_when_checked and king_in_check(&board, player))
    {
        struct chess_board cpy;
        struct dynamic_array *legal_moves = init_dynamic();
        struct chess_move move;

        for (int move_index = 0; move_index < moves->current_index; move_index++)
        {
            create_board_copy(&board, &cpy);

            create_move(&move, &cpy, id, moves->values[move_index]);
            board_apply_move(&cpy, &move);

            if (not king_in_check(&cpy, player))
                append_dynamic(legal_moves, moves->values[move_index]);
        }

        free_dynamic(moves);
        return legal_moves;
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
bool handle_castle_move(const struct chess_board *board, struct chess_move *move)
{
    enum chess_player color = board->next_move_player;
    int home_y = color == PLAYER_WHITE ? 0 : GRID_SIZE-1;
    int king_home = from_cords(KING_X_LOCATION, home_y);
    move->to_square = move->is_long_castle ? from_cords(2, home_y) : from_cords(6, home_y);

    if (board->piece_id[king_home] == PIECE_KING and board->piece_color[king_home] == color)
    {
        bool left = false, right = false;
        if (check_for_castle(*board, &left, &right))
        {
            bool king_side = not move->is_long_castle;
            if ((king_side and right) or (not king_side and left))
            {
                move->from_square = king_home;
                return true;
            }
        }
    }

    printf("illegal move: %s king from %s to %s\n",
           color_string(color), square_string(king_home), square_string(move->to_square));
    move->from_square = -1;
    return false;
}

int select_piece_for_move(const struct chess_board *board, struct chess_move *move, enum chess_player color)
{
    int candidate = -1;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (not board->piece_present[i]) continue;
        if (board->piece_color[i] != color) continue;
        if (board->piece_id[i] != move->piece_id) continue;

        int x, y;
        from_id(i, &x, &y);
        if (move->from_file and x != move->from_file - 'a') continue;
        if (move->from_rank and y != move->from_rank - '1') continue;

        struct dynamic_array *legal = generate_legal_moves(move->piece_id, *board, i, true, true);
        if (not legal) continue;

        for (int j = 0; j < legal->current_index; j++)
        {
            if (legal->values[j] == move->to_square)
            {
                if (candidate != -1)
                {
                    free_dynamic(legal);
                    printf("move completion error: %s %s to %s\n",
                           color_string(color), piece_string(move->piece_id), square_string(move->to_square));
                    return -1;
                }
                candidate = i;
            }
        }
        free_dynamic(legal);
    }

    if (candidate == -1)
    {
        printf("move completion error: %s %s to %s\n",
               color_string(color), piece_string(move->piece_id), square_string(move->to_square));
        return -1;
    }

    struct chess_board temp = *board;
    temp.piece_present[candidate] = false;
    temp.piece_present[move->to_square] = true;
    temp.piece_id[move->to_square] = move->piece_id;
    temp.piece_color[move->to_square] = color;

    int king_square = -1;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (temp.piece_present[i] and temp.piece_color[i] == color and temp.piece_id[i] == PIECE_KING)
        {
            king_square = i;
            break;
        }
    }

    if (king_square != -1 and player_in_check(&temp, king_square))
    {
        printf("illegal move: %s %s from %s to %s\n",
               color_string(color), piece_string(move->piece_id),
               square_string(candidate), square_string(move->to_square));
        return -1;
    }

    return candidate;
}


// fill in missing info in a parsed board state, report move completion error if cant be determined 
void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    enum chess_player color = board->next_move_player;

    if (move->is_castle and move->piece_id == PIECE_KING)
    {
        if (!handle_castle_move(board, move)) return;
    }
    else
    {
        int candidate = select_piece_for_move(board, move, color);
        if (candidate == -1) return;
        move->from_square = candidate;
    }
}


void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    int from_id = move->from_square;
    int to_id   = move->to_square;
    
    if (from_id == -1) {
        return;
    }
    
    enum chess_piece piece = move->piece_id;
    enum chess_player color = board->piece_color[from_id];

    move_piece(board, from_id, to_id);

    // promotion 
    if (move->promotes_to_id != -1) {
        board->piece_id[to_id] = move->promotes_to_id;
    }

    // castling
    if (piece == PIECE_KING) {
        int home_y = (color == PLAYER_WHITE) ? 0 : GRID_SIZE - 1;
        bool castle_left = false, castle_right = false;

        check_for_castle(*board, &castle_left, &castle_right);

        // king-side castle
        if (from_id == from_cords(KING_X_LOCATION, home_y) and to_id == from_cords(6, home_y) and castle_right) {
            move_piece(board, from_cords(7, home_y), from_cords(5, home_y));
        }

        // queen-side castle
        if (from_id == from_cords(KING_X_LOCATION, home_y) and to_id == from_cords(2, home_y) and castle_left) {
            move_piece(board, from_cords(0, home_y), from_cords(3, home_y));
        }

        if (color == PLAYER_WHITE) board->white_can_castle = false;
        else board->black_can_castle = false;
    }

    if (piece == PIECE_ROOK) {
        if (color == PLAYER_WHITE and (from_id == from_cords(0, 0) or from_id == from_cords(7, 0)))
            board->white_can_castle = false;
        if (color == PLAYER_BLACK and (from_id == from_cords(0, GRID_SIZE-1) or from_id == from_cords(7, GRID_SIZE-1)))
            board->black_can_castle = false;
    }

    // switch player
    board->next_move_player = (color == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
}

bool king_in_check(struct chess_board *board, enum chess_player player)
{
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        if (not board->piece_present[id]     ) continue;
        if (board->piece_color[id] != player ) continue;
        if (board->piece_id[id] != PIECE_KING) continue;

        return player_in_check(board, id);
    }
    
    return false;
}

bool player_in_check(struct chess_board *board, int id_to_check)
{

    // Optimize, check if the preivous check is still valid
    struct dynamic_array *attacking_squares;
    enum chess_player player_color = board->piece_color[id_to_check];

    if (board->last_check_id != -1)
    {
        int id = board->last_check_id;

        if (board->piece_present[id] and board->piece_color[id] != player_color)
        {
            attacking_squares = generate_legal_moves(board->piece_id[id], *board, id, false, false);
            
            if (attacking_squares != NULL)            
                for (int j = 0; j < attacking_squares->current_index; j++) 
                {
                    if (attacking_squares->values[j] == id_to_check)
                    {
                        free_dynamic(attacking_squares);
                        return true;
                    }
                }
            
            free_dynamic(attacking_squares);
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board->piece_present[i] and board->piece_color[i] != player_color)
        {
            if (i == board->last_check_id) continue;

            attacking_squares = generate_legal_moves(board->piece_id[i], *board, i, false, false);
            
            if (attacking_squares == NULL)
                continue;
            
            for (int j = 0; j < attacking_squares->current_index; j++) 
            {
                if (attacking_squares->values[j] == id_to_check)
                {
                    board->last_check_id = i;
                    free_dynamic(attacking_squares);
                    return true;
                }
            }

            free_dynamic(attacking_squares);
        }
    }

    board->last_check_id = -1;

    return false;
}

void create_move(struct chess_move *move, struct chess_board *board, int from_id, int to_id)
{
    /*
        enum chess_piece piece_id; // The moving piece id

        int from_square;     // From which square id is the piece moving from
        int to_square;       // To   which square id is the piece moving to
        
        bool is_capture;     // True if the moving piece is capturing a piece

        int promotes_to_id;  // If a pawn is promoted, the new piece id; -1 otherwise

        bool is_castle;      // True if the move is a castle
        bool is_long_castle; // True if the move is a long castle

        char from_file; // optional file disambiguation
        char from_rank; // optional rank disambiguation
    */

    // Rest of the fields are optional for apply_move

    move->piece_id = board->piece_id[from_id];

    move->from_square = from_id;
    move->to_square = to_id;

    move->promotes_to_id = PIECE_UNKNOWN;
}

bool find_forced_mate(struct chess_board *board, int depth, struct chess_move *recommended_move)
{
    struct dynamic_array *moves;
    bool has_legal_move = false;
    
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        if (not board->piece_present[id] or 
            board->piece_color[id] != board->next_move_player) 
            continue;

        moves = generate_legal_moves(board->piece_id[id], *board, id, true, true);
        
        if (moves == NULL) continue;
        
        if (moves->current_index > 0)
            has_legal_move = true;
        
        free_dynamic(moves);
        
        if (has_legal_move) break;
    }
    
    if (not has_legal_move)
        return king_in_check(board, board->next_move_player);
    
    if (depth >= MAX_DEPTH * 2)
        return false;
        
    bool is_attacker_turn = depth % 2 == 0;
    
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        if (not board->piece_present[id] or board->piece_color[id] != board->next_move_player) 
            continue;

        moves = generate_legal_moves(board->piece_id[id], *board, id, true, true);
        
        if (moves == NULL) continue;

        for (int move_index = 0; move_index < moves->current_index; move_index++)
        {
            struct chess_board cpy;
            struct chess_move move;
            
            create_board_copy(board, &cpy);
            create_move(&move, &cpy, id, moves->values[move_index]);
            board_apply_move(&cpy, &move);

            bool mate_in_this_line = find_forced_mate(&cpy, depth + 1, recommended_move);
            
            if (is_attacker_turn)
            {
                if (mate_in_this_line)
                {
                    free_dynamic(moves);

                    if (depth == 0)
                        *recommended_move = move;

                    return true;
                }
            } else {
                if (not mate_in_this_line)
                {
                    free_dynamic(moves);
                    return false;
                }
            }
        }
        
        free_dynamic(moves);
    }
    
    return not is_attacker_turn;
}

void print_recommended_move(struct chess_move *move)
{
    const char *piece = piece_string(move->piece_id);
    char *from = square_string(move->from_square);
    char *to   = square_string(move->to_square);

    // TODO: implement promotion and castling
    
    printf("%s from %s to %s\n", piece, from, to);
}

// Classify the state of the board, printing one of the following:
// - game incomplete
// - white wins by checkmate
// - black wins by checkmate
// - draw by stalemate
void board_summarize(struct chess_board *board)
{
    bool has_legal_move = false;
    
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        if (not board->piece_present[id] or board->piece_color[id] != board->next_move_player) 
            continue;

        struct dynamic_array *moves = generate_legal_moves(board->piece_id[id], *board, id, true, true);
        
        if (moves != NULL and moves->current_index > 0)
        {
            has_legal_move = true;
            free_dynamic(moves);
            break;
        }
        
        if (moves != NULL)
            free_dynamic(moves);
    }
    
    if (not has_legal_move)
    {
        if (king_in_check(board, board->next_move_player))
        {
            if (board->next_move_player == PLAYER_WHITE)
                puts("black wins by checkmate");
            else
                puts("white wins by checkmate");
        } else
            puts("draw by stalemate");
        
        return;
    }

    /* ------------ Move recommendations ------------ */

    /*
        If you are attempting the bonus, then in the case where the output is game incomplete, you must also
        print out a second line of output, of the following form, describing the suggested move:
        suggest PLAYER PIECE from SQUARE to SQUARE
        If the suggested move is a castle, follow the convention of describing the king’s movement. If the suggested
        move is a pawn promotion, then use the following extended form:
        suggest PLAYER pawn from SQUARE to SQUARE promoting to PIECE
    */

    struct chess_move *recommended_move = NULL; 

    if (find_forced_mate(board, 0, recommended_move))
    {
        print_recommended_move(recommended_move);
        return;
    }


    // EDWARDS MOVE recommendation
}