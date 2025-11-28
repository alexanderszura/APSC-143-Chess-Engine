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
<<<<<<< Updated upstream
    // TODO: print the state of the game.
=======
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

>>>>>>> Stashed changes
}
