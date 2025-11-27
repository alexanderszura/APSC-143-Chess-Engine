
#include "board.h"
#include "tools.h"
#include "stdio.h"
#include "stdlib.h"
#include "moves.h"

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
        int en_passant_range = player == PLAYER_WHITE ? GRID_SIZE - 4 : 3;

        if (not board.piece_present[from_cords(x, y + dir)]) // If no piece in front of the pawn
        {
            add_move(moves, board, x, y + dir, player);
            if (y == pawn_spawn and not board.piece_present[from_cords(x, y + 2 * dir)])
            {
                board.pawn_double_file = x;
                add_move(moves, board, x, y + 2 * dir, player);
            }
        }
        
        if (x > 0 and board.piece_present[from_cords(x - 1, y + dir)]) // Capturing Left side
            add_move(moves, board, x - 1, y + dir, player);

        if (x < GRID_SIZE - 1 and board.piece_present[from_cords(x + 1, y + dir)]) // Capturing Right side
            add_move(moves, board, x + 1, y + dir, player);

        // En Passant
        if (board.pawn_double_file >= 0 and y == en_passant_range and abs(board.pawn_double_file - x) == 1)
            add_move(moves, board, board.pawn_double_file, y + dir, player);

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

        break;   
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
    
    if (remove_moves_when_checked)
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

/*
* Searches board for all pieces of the correct type and colour, and applies & checks which candidate can legally move to target square. 
-> params: pointer to current board state, parsed move struct, the player whose move it is (color).
-> return: board index of piece to execute the move, or -1 if no legal piece matches. 

*/
int select_piece_for_move(const struct chess_board *board, struct chess_move *move, enum chess_player color)
{
    int candidate = -1; // tracks the candidate; which piece matches the move.

    // iterate over every square on the board to find matching pieces.
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        // skip empty and irrelevant (does not belong to player) squares.
        if (!board->piece_present[i]) continue;
        if (board->piece_color[i] != color) continue;
        if (board->piece_id[i] != move->piece_id) continue;

        int x, y;
        if (!from_id(i, &x, &y)) continue; // index -> coordinate conversion

        // disambiguate if hint on file or rank.
        if (move->from_file and x != move->from_file - 'a') continue;
        if (move->from_rank and y != move->from_rank - '1') continue;

        struct dynamic_array *legal = generate_legal_moves(move->piece_id, *board, i, true, true); // generate legal moves for the square. 
        if (!legal) continue;

        // check if destination matches the move. 
        for (int j = 0; j < legal->current_index; j++)
        {
            if (legal->values[j] == move->to_square)
            {
                // invalid if candidate was already found, it's ambiguous.
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
        if (candidate != -1) break; // stop searching once correct piece is found. 
    }

    if (candidate == -1) // no matching piece was found.
    {
        printf("move completion error: %s %s to %s\n",
               color_string(color), piece_string(move->piece_id), square_string(move->to_square));
        return -1;
    }

    // simulate move on a temporary board to ensure it doesn't leave king in check.
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

/*
*Completes a move by identifying which exact piece moves. 
-> params: pointer to current board, move structure to be completed.

*uses select piece for move & castling helpers for code organization.
*/
void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    enum chess_player color = board->next_move_player;

    // castling detection. 
    if (move->is_castle and move->piece_id == PIECE_KING)
    {
        if (!handle_castle_move(board, move)) return;
    }
    else
    {
        // resolve which piece makes the move. 
        int candidate = select_piece_for_move(board, move, color);
        if (candidate == -1) return;
        move->from_square = candidate;
    }
}

/*
*Determines correct destination square for king (based on short v long castle), verifies the legality of castling given the current board state, and updates move fields accordingly.
->params: pointer to current chess board state, pointer to move structure being built.
->return: boolean: true if castling is legal, move fields are filled, false if castling is illegal, move->from_square is set to -1.
*/
bool handle_castle_move(const struct chess_board *board, struct chess_move *move)
{
    // idenfify whose turn it is, the row where the player's king starts, and compute its board index. 
    enum chess_player color = board->next_move_player; /
    int home_y = color == PLAYER_WHITE ? 0 : GRID_SIZE-1;
    int king_home = from_cords(KING_X_LOCATION, home_y);
    
    if (move->is_long_castle) {
        move->to_square = from_cords(2, home_y);
    } else { // short castle.
        move->to_square = from_cords(6, home_y);
    }
    
    // verify that the correct king os actually standing on its home square. 
    if (board->piece_id[king_home] == PIECE_KING and
        board->piece_color[king_home] == color) { 
        
        bool castle_left = false, castle_right = false;

        // call to rules checker for which castling options are legal.
        if (check_for_castle(*board, &castle_left, &castle_right)) {
            bool king_side = !move->is_long_castle;
            if ((king_side and castle_right) or (!king_side and castle_left)) { // ensure chosen direction (short/long) is permitted. 
                move->from_square = king_home;
                return true; // castling move is valid. 
            }
        }
        // castling is illegal; mark move as invalid.
        printf("illegal move: %s castle\n", color_string(color));
        move->from_square = -1; 
        return false;
    }
    // impossible castling; mark as invalid. 
    printf("illegal move: %s king not in starting position\n", color_string(color));
    move->from_square = -1;
    return false;
}


void update_castling(struct chess_board *board, int id)
{
    if (id == from_cords(0, 0))
        board-> white_can_castle_left = false;
    elif (id == from_cords(7, 0))
        board->white_can_castle_right = false;
    elif (id == from_cords(0, 7))
        board-> black_can_castle_left = false;
    elif (id == from_cords(7, 7))
        board->black_can_castle_right = false;
}

/*
*Applies move to the actual board; performing captures, en passant, promotions, etc., while updating states.
-> params: pointer to board to modify, fully determined move. 
*/
void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    int from_id = move->from_square;
    int to_id   = move->to_square;

    if (from_id == -1)
        return; // invalid move state. 

    // get coordinates for current & destination positions from indices. 
    int from_x = from_id % GRID_SIZE;
    int from_y = from_id / GRID_SIZE;
    int to_x   = to_id % GRID_SIZE;
    int to_y   = to_id / GRID_SIZE;

    // store which piece is moving and its color. 
    enum chess_piece piece = move->piece_id;
    enum chess_player color = board->piece_color[from_id];

    // handle en passant captures; pawn moving diagonally into an empty square.  
    if (piece == PIECE_PAWN and
        abs(to_x - from_x) == 1 and 
        !board->piece_present[to_id]) 
    {
        int captured_y = (color == PLAYER_WHITE) ? to_y - 1 : to_y + 1; // determine rank of the pawn that may be captures. (one behind for white, one ahead for white)
        int captured_square = from_cords(to_x, captured_y); // convert into board_index.

        // verify the eligbility of en passant. 
        // check if the oponnent's pawn moved two squares in the previous turn and is currently sitting at capture square. 
        if (board->pawn_double_file == to_x and
            board->piece_present[captured_square] and
            board->piece_id[captured_square] == PIECE_PAWN and
            board->piece_color[captured_square] != color)
        {
            // remove captured pawn. 
            board->piece_present[captured_square] = false;
            board->piece_id[captured_square] = PIECE_UNKNOWN;
            board->piece_color[captured_square] = PLAYER_UNKNOWN;
        }
    }

    // if capturing a rook, update castling rights. 
    if (board->piece_present[to_id])
        update_castling(board, to_id);

    move_piece(board, from_id, to_id); 

    // track availability of en passant.
    if (piece == PIECE_PAWN and abs(to_y - from_y) == 2)
        board->pawn_double_file = from_x;
    else
        board->pawn_double_file = -1;

    // handle pawn promotion.
    if (move->promotes_to_id != PIECE_UNKNOWN)
        board->piece_id[to_id] = move->promotes_to_id;

    // handle king movement, including castling.
    if (piece == PIECE_KING)
    {
        int home_y = (color == PLAYER_WHITE) ? 0 : GRID_SIZE - 1;

        if (move->is_castle) {
            if (move->is_long_castle) {
                move_piece(board, from_cords(0, home_y), from_cords(3, home_y));
            } else { // short castle
                move_piece(board, from_cords(7, home_y), from_cords(5, home_y));
            }
        }

        // king moved, disable castling.
        if (color == PLAYER_WHITE)
        {
            board->white_can_castle_left = false;
            board->white_can_castle_right = false;
        }
        else
        {
            board->black_can_castle_left = false;
            board->black_can_castle_right = false;
        }
    }


    // if a rook moved, update castling rights accordingly.
    if (piece == PIECE_ROOK)
        update_castling(board, from_id);

    // switch the active player. 
    board->next_move_player = (color == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
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

void print_recommended_move(struct chess_move *move, enum chess_player player)
{
    /*
        If you are attempting the bonus, then in the case where the output is game incomplete, you must also
        print out a second line of output, of the following form, describing the suggested move:
        suggest PLAYER PIECE from SQUARE to SQUARE
        If the suggested move is a castle, follow the convention of describing the king’s movement. If the suggested
        move is a pawn promotion, then use the following extended form:
        suggest PLAYER pawn from SQUARE to SQUARE promoting to PIECE
    */

    const char *player_str = color_string(player);

    const char *piece = NULL;
    char *from = square_string(move->from_square);
    char *to   = square_string(move->to_square);
    
    if (move->promotes_to_id != PIECE_UNKNOWN)
    {
        piece = piece_string(move->promotes_to_id);
        printf("%s pawn from %s to %s promoting to %s\n", player_str, from, to, piece);
    } elif (move->is_castle) {
        puts(move->is_long_castle ? "O-O-O" : "O-O");
    } else {
        piece = piece_string(move->piece_id);
        printf("%s %s from %s to %s\n", player_str, piece, from, to);
    }
    
    free(from);
    free(to);
}

void move_piece(struct chess_board *board, int from, int to) {
    board->piece_id[to] = board->piece_id[from];
    board->piece_color[to] = board->piece_color[from];
    board->piece_present[to] = true;
    board->piece_present[from] = false;
}