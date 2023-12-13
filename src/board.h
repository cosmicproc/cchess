#pragma once

#include <stdbool.h>

#define W_ROOK "♜"
#define W_KNIGHT "♞"
#define W_BISHOP "♝"
#define W_QUEEN "♛"
#define W_KING "♚"
#define W_PAWN "♟"
#define B_ROOK "♖"
#define B_KNIGHT "♘"
#define B_BISHOP "♗"
#define B_QUEEN "♕"
#define B_KING "♔"
#define B_PAWN "♙"

#define is_rook(X) (str_equal(X, W_ROOK) || str_equal(X, B_ROOK))
#define is_knight(X) (str_equal(X, W_KNIGHT) || str_equal(X, B_KNIGHT))
#define is_bishop(X) (str_equal(X, W_BISHOP) || str_equal(X, B_BISHOP))
#define is_queen(X) (str_equal(X, W_QUEEN) || str_equal(X, B_QUEEN))
#define is_king(X) (str_equal(X, W_KING) || str_equal(X, B_KING))
#define is_pawn(X) (str_equal(X, W_PAWN) || str_equal(X, B_PAWN))
#define is_white(X) (str_equal(X, W_ROOK) || str_equal(X, W_KNIGHT) || str_equal(X, W_BISHOP) || \
                        str_equal(X, W_QUEEN) || str_equal(X, W_KING) || str_equal(X, W_PAWN))
#define is_black(X) (str_equal(X, B_ROOK) || str_equal(X, B_KNIGHT) || str_equal(X, B_BISHOP) || \
                        str_equal(X, B_QUEEN) || str_equal(X, B_KING) || str_equal(X, B_PAWN))
#define is_symbol(X) (is_black(X) || is_white(X))

#define EMPTY " "
#define BOARD_VERTICAL_LINE "|"
#define BOARD_HORIZONTAL_LINE "―"

typedef enum team
{
    WHITE,
    BLACK,
    NONE
} team;

typedef struct position
{
    short x;
    short y;
} position;

#define POS(X, Y) ((position){X, Y})

typedef struct piece
{
    char *symbol;
    bool has_moved;
} piece;

#define get_piece(X) (board[X.y][X.x])

#define BOARD_HEIGTH 8
#define BOARD_WIDTH 8

extern piece board[BOARD_HEIGTH][BOARD_WIDTH];

void generate_board(void);
void print_board(team t);
void extract_board(FILE * file);
int load_board(FILE *file);
bool is_board_valid(void);
bool is_position_valid(position pos);
bool is_position_empty(position pos);
int change_position(position current_pos, position new_pos);
void replace_position(position pos, char *new_symbol);
void empty_position(position pos);
bool is_any_piece_in_between(position pos1, position pos2);
team get_piece_team(position pos);
team opposite_team(team t);
position get_promotable(team t);
bool is_checked(team t);
bool is_checkmate(position pos);
position get_attacker(position pos, team attacker);
bool is_under_attack(position pos, team attacker);
bool is_checked(team t);
bool is_checked_after_move(team t, position current_pos, position new_pos);
team get_winner_team(void);
bool is_draw(void);
bool is_over(void);