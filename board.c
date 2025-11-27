#include "board.h"
#include "tools.h"
#include "stdio.h"
#include "stdlib.h"
#include "moves.h"

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
    board->pawn_double_file = -1;

    board->next_move_player = PLAYER_WHITE;
    
    board->white_can_castle_left  = true;
    board->white_can_castle_right = true;
    board->black_can_castle_left  = true;
    board->black_can_castle_right = true;

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

bool check_for_castle(struct chess_board board, bool *castle_left, bool *castle_right)
{
    if (board.next_move_player == PLAYER_WHITE)
    {
        *castle_left  = board.white_can_castle_left;
        *castle_right = board.white_can_castle_right;
    } 
    else 
    {
        *castle_left  = board.black_can_castle_left;
        *castle_right = board.black_can_castle_right;
    }

    int y = board.next_move_player == PLAYER_WHITE ? 0 : GRID_SIZE - 1;

    if (player_in_check(&board, from_cords(KING_X_LOCATION, y)))
    {
        // If in check disallow castling
        *castle_left = false;
        *castle_right = false;
        return false;
    }

    // Check left castle
    int rook_id = from_cords(0, y);
    if (*castle_left and board.piece_present[rook_id] and board.piece_id[rook_id] == PIECE_ROOK)
    {
        // squares between rook and king: x = 1,2,3
        for (int x = 1; x < KING_X_LOCATION; x++)
        {
            if (board.piece_present[from_cords(x, y)])
            {
                *castle_left = false;
                break;
            }
        }

        // king passing through attacked squares: d1,c1
        if (*castle_left)
        {
            for (int x = KING_X_LOCATION - 1; x >= 2; x--)
            {
                if (player_in_check(&board, from_cords(x, y)))
                {
                    *castle_left = false;
                    break;
                }
            }
        }
    }
    else
        *castle_left = false;

    // Check right castle
    rook_id = from_cords(GRID_SIZE - 1, y);
    if (*castle_right and board.piece_present[rook_id] and board.piece_id[rook_id] == PIECE_ROOK)
    {
        // squares f1,g1 → KING_X+1 to KING_X+2
        for (int x = KING_X_LOCATION + 1; x <= KING_X_LOCATION + 2; x++)
        {
            if (board.piece_present[from_cords(x, y)])
            {
                *castle_right = false;
                break;
            }
        }

        // pass-through attacked: f1,g1
        if (*castle_right)
        {
            for (int x = KING_X_LOCATION + 1; x <= KING_X_LOCATION + 2; x++)
            {
                if (player_in_check(&board, from_cords(x, y)))
                {
                    *castle_right = false;
                    break;
                }
            }
        }
    }
    else
        *castle_right = false;

    return *castle_left or *castle_right;
}

/// @brief Returns true if the king of the given color is in check
/// @param board The current board state
/// @param player The player to check
/// @return true if in check false if not
bool king_in_check(struct chess_board *board, enum chess_player player)
{
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        // Find the colored king
        if (not board->piece_present[id]     ) continue;
        if (board->piece_color[id] != player ) continue;
        if (board->piece_id[id] != PIECE_KING) continue;

        return player_in_check(board, id);
    }
    
    return false;
}

/// @brief Returns true if the piece given is under attack
/// @param board The current board state
/// @param id_to_check The id to check
/// @return true if under attack false if not
bool player_in_check(struct chess_board *board, int id_to_check)
{
    enum chess_player player_color = board->piece_color[id_to_check];

    if (board->last_check_id != -1)
    {
        int id = board->last_check_id;

        /*
            Optimization:
            Mostly for king checks,
            check the last piece that attacked the king is still attacking the king.
        */
        if (board->piece_present[id] and board->piece_color[id] != player_color)
        {
            struct dynamic_array *attacking_squares = generate_legal_moves(board->piece_id[id], *board, id, false, false);
            
            if (attacking_squares != NULL) {
                for (int j = 0; j < attacking_squares->current_index; j++) {
                    if (attacking_squares->values[j] == id_to_check) {
                        free_dynamic(attacking_squares);
                        return true;
                    }
                }
                free_dynamic(attacking_squares);
            }
        }
    }

    // Loop through all of the pieces
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board->piece_present[i] and board->piece_color[i] != player_color)
        {
            if (i == board->last_check_id) continue; // Removed the optimized moved

            // Check if any of the moves a piece can make can capture the piece to check
            struct dynamic_array *attacking_squares = generate_legal_moves(board->piece_id[i], *board, i, false, false);
            
            if (attacking_squares == NULL)
                continue;
            
            for (int j = 0; j < attacking_squares->current_index; j++) {
                if (attacking_squares->values[j] == id_to_check) {
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

/// @brief Returns true if there is a way that which ever piece is next has a forced mate in 3 turns
/// @param board the current board state
/// @param depth the depth of the function, start at 0 if calling this from outside the function
/// @param recommended_move the move to recommend
/// @return true if theres a forced mate false otherwise.
bool find_forced_mate(struct chess_board *board, int depth, struct chess_move *recommended_move)
{
    struct dynamic_array *moves;
    bool has_legal_move = false;
    
    // Check if theres legal moves
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        // If it's not the pieces turn or there isn't a piece present then disregard
        if (not board->piece_present[id] or board->piece_color[id] != board->next_move_player) 
            continue;

        moves = generate_legal_moves(board->piece_id[id], *board, id, true, true);
        
        if (moves == NULL) continue;
        
        if (moves->current_index > 0)
            has_legal_move = true;
        
        free_dynamic(moves);
        
        if (has_legal_move) break;
    }
    
    if (not has_legal_move) // Checkmate if true
        return king_in_check(board, board->next_move_player);
    
    if (depth >= MAX_DEPTH * 2) // Exit if we are at the max depth
        return false;
        
    bool is_attacker_turn = depth % 2 == 0;
    
    for (int id = 0; id < BOARD_SIZE; id++)
    {
        // If it's not the pieces turn or there isn't a piece present then disregard
        if (not board->piece_present[id] or board->piece_color[id] != board->next_move_player) 
            continue;

        moves = generate_legal_moves(board->piece_id[id], *board, id, true, true);
        
        if (moves == NULL) continue;

        int promotion_level = PIECE_UNKNOWN;

        for (int move_index = 0; move_index < moves->current_index; move_index++)
        {
            struct chess_board cpy;
            struct chess_move move;
            
            // Create a copy of the board to apply pieces and look into the future
            create_board_copy(board, &cpy);
            create_move(&move, &cpy, id, moves->values[move_index]);

            int x, y;
            from_id(move.to_square, &x, &y);

            // If the pawn is promoting, try all of the possible pawn promotions
            if (move.piece_id == PIECE_PAWN and (y == 0 or y == GRID_SIZE - 1))
            {
                if (promotion_level == PIECE_UNKNOWN)
                    promotion_level = PIECE_KNIGHT;
                elif (promotion_level++ == PIECE_QUEEN)
                {
                    promotion_level = PIECE_UNKNOWN;
                    continue;
                }
                
                move_index--;
                move.promotes_to_id = promotion_level;
            }

            board_apply_move(&cpy, &move);

            bool mate_in_this_line = find_forced_mate(&cpy, depth + 1, recommended_move);
            
            if (is_attacker_turn)
            {
                if (mate_in_this_line)
                {
                    free_dynamic(moves);

                    // Recommend the first move in the line
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

// Classify the state of the board, printing one of the following:
// - game incomplete
// - white wins by checkmate
// - black wins by checkmate
// - draw by stalemate
void board_summarize(struct chess_board *board)
{
    bool has_legal_move = false;
    
    // Check if there is legal moves
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
        // Check if there the king is in check
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

    puts("game incomplete");

    struct chess_move recommended_move;

    if (find_forced_mate(board, 0, &recommended_move))
    {
        print_recommended_move(&recommended_move, board->next_move_player);
        return;
    }

    // EDWARDS MOVE recommendation
}
