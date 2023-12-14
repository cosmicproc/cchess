#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "aliases.h"
#include "game.h"

static char *board_layout[BOARD_HEIGTH][BOARD_WIDTH] = {
    {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK},
    {W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN},
    {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK},
};

piece board[BOARD_HEIGTH][BOARD_WIDTH];

void generate_board(void)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            board[y][x] = (piece){board_layout[y][x], false};
}

void print_board(team t)
{
    printf("  ");
    for (size_t i = 0; i < BOARD_WIDTH * 2 + 1; i++)
        printf("%s", BOARD_HORIZONTAL_LINE);
    printf("\n");
    if (t == BLACK)
        for (size_t y = 0; y < BOARD_HEIGTH; y++)
        {
            printf("%zu " BOARD_VERTICAL_LINE, y + 1);
            for (size_t x = 0; x < BOARD_WIDTH; x++)
            {
                if (board[y][BOARD_WIDTH - x - 1].symbol)
                    printf("%s", board[y][BOARD_WIDTH - x - 1].symbol);
                else
                    printf(" ");
                printf("%s", BOARD_VERTICAL_LINE);
            }
            printf("\n");
        }
    else
        for (size_t y = 0; y < BOARD_HEIGTH; y++)
        {
            printf("%zu " BOARD_VERTICAL_LINE, BOARD_HEIGTH - y);
            for (size_t x = 0; x < BOARD_WIDTH; x++)
            {
                if (board[BOARD_HEIGTH - y - 1][x].symbol)
                    printf("%s", board[BOARD_HEIGTH - y - 1][x].symbol);
                else
                    printf(" ");
                printf("%s", BOARD_VERTICAL_LINE);
            }
            printf("\n");
        }

    printf("  ");
    for (size_t i = 0; i < BOARD_WIDTH * 2 + 1; i++)
        printf("%s", BOARD_HORIZONTAL_LINE);
    printf("\n  ");
    if (t == BLACK)
        for (size_t i = 0; i < BOARD_WIDTH; i++)
            printf(" %c", 'h' - (char)i);
    else
        for (size_t i = 0; i < BOARD_WIDTH; i++)
            printf(" %c", 'a' + (char)i);

    printf("\n");
}

void extract_board(FILE *file)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
    {
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            fprintf(file, "%1s%1d ", board[y][x].symbol, board[y][x].has_moved);
        fprintf(file, "\n");
    }
}

int load_board(FILE *file)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
    {
        for (size_t x = 0; x < BOARD_WIDTH; x++)
        {
            char buffer[10];
            char *symbol;
            char c;
            if (!fgets(buffer, 4, file))
                return -1;
            if (buffer[0] != ' ')
            {
                if (!is_symbol(buffer))
                    return -1;
                c = getc(file);
                symbol = strdup(buffer);
                symbol[4] = '\0';
                getc(file);
            }
            else
            {
                c = buffer[1];
                symbol = " ";
            }
            if (c != '0' && c != '1')
                return -1;
            board[y][x] = (piece){symbol, buffer[1] == '1' ? true : false};
        }
        getc(file);
    }
    return 0;
}

bool is_board_valid(void)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            if (!board[y][x].symbol)
                return false;
    return true;
}

bool is_position_valid(position pos)
{
    return pos.x >= 0 && pos.x < BOARD_WIDTH && pos.y >= 0 && pos.y < BOARD_HEIGTH;
}

bool is_position_empty(position pos)
{
    return str_equal(get_piece(pos).symbol, EMPTY);
}

int change_position(position current_pos, position new_pos)
{
    get_piece(new_pos) = get_piece(current_pos);
    get_piece(new_pos).has_moved = true;
    get_piece(current_pos).symbol = " ";
    return 0;
}

void replace_position(position pos, char *new_symbol)
{
    get_piece(pos).symbol = new_symbol;
    get_piece(pos).has_moved = true;
}

void empty_position(position pos)
{
    replace_position(pos, EMPTY);
}

bool is_any_piece_in_between(position pos1, position pos2)
{
    assert(((pos1.x == pos2.x) != (pos1.y == pos2.y) ||
            (abs(pos1.x - pos2.x) == abs(pos1.y - pos2.y) && abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y) != 0)));
    position vector = {0, 0};
    if (pos1.x == pos2.x)
    {
        if (pos1.y > pos2.y)
            vector = POS(0, -1);
        else
            vector = POS(0, 1);
    }
    else if (pos1.y == pos2.y)
    {
        if (pos1.x > pos2.x)
            vector = POS(-1, 0);
        else
            vector = POS(1, 0);
    }
    else
    {
        if (pos1.x > pos2.x)
            vector.x = -1;
        else
            vector.x = 1;
        if (pos1.y > pos2.y)
            vector.y = -1;
        else
            vector.y = 1;
    }
    for (position i = {pos1.x + vector.x, pos1.y + vector.y}; i.x != pos2.x || i.y != pos2.y; i.x += vector.x, i.y += vector.y)
        if (!is_position_empty(i))
            return true;
    return false;
}

team get_piece_team(position pos)
{
    if (is_position_empty(pos))
        return NONE;
    if (is_white(get_piece(pos).symbol))
        return WHITE;
    return BLACK;
}

team opposite_team(team t)
{
    if (t == WHITE)
        return BLACK;
    else if (t == BLACK)
        return WHITE;
    else
        return NONE;
}

position get_promotable(team t)
{
    unsigned line_number = t == WHITE ? 7 : 0;
    for (size_t x = 0; x < BOARD_WIDTH; x++)
        if (get_piece_team(POS(x, line_number)) == t && is_pawn(board[line_number][x].symbol))
            return POS(x, line_number);
    return POS(-1, -1);
}

bool can_king_attack(position current_pos, position target_pos)
{
    return abs(target_pos.x - current_pos.x) <= 1 && abs(target_pos.y - current_pos.y) <= 1;
}

position get_attacker(position pos, team attacker)
{
    assert(attacker != NONE);
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            if (get_piece_team(POS(x, y)) == attacker)
            {
                if (is_king(board[y][x].symbol))
                {
                    if (can_king_attack(POS(x, y), pos))
                        return POS(x, y);
                }
                else if (is_valid_move(POS(x, y), pos))
                    return POS(x, y);
            }
    return POS(-1, -1);
}

bool is_under_attack(position pos, team attacker)
{
    return get_attacker(pos, attacker).x != -1;
}

bool is_checked(team t)
{
    assert(t != NONE);
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            if (str_equal(board[y][x].symbol, t == WHITE ? W_KING : B_KING))
                return is_under_attack(POS(x, y), opposite_team(t));
    return false;
}

bool is_checked_after_move(team t, position current_pos, position new_pos)
{
    piece board_copy[BOARD_HEIGTH][BOARD_WIDTH];
    memcpy(&board_copy, &board, sizeof(board_copy));
    change_position(current_pos, new_pos);
    bool r = is_checked(t);
    memcpy(&board, &board_copy, sizeof(board));
    return r;
}

bool is_checkmate(position pos)
{
    for (int i = -1; i < 2; i++)
        for (int j = -1; j < 2; j++)
        {
            position near_pos = POS(pos.x + j, pos.y + i);
            position attacker, defender;
            if (is_position_valid(near_pos) &&
                ((!is_under_attack(near_pos, opposite_team(get_piece_team(pos))) &&
                  get_piece_team(pos) != get_piece_team(near_pos)) ||
                 (i == 0 && j == 0 && (!is_checked(get_piece_team(pos)) || (((defender = get_attacker((attacker = get_attacker(near_pos, opposite_team(get_piece_team(pos)))), get_piece_team(pos))).x != -1) && !is_checked_after_move(get_piece_team(pos), defender, attacker))))))
                return false;
        }
    return true;
}

team get_winner_team(void)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            if (is_king(board[y][x].symbol) && is_checkmate(POS(x, y)))
                return opposite_team(get_piece_team(POS(x, y)));
    return NONE;
}

bool is_draw(void)
{
    return !can_do_any_move(WHITE) || !can_do_any_move(BLACK) || are_last_moves_same();
}

bool is_over(void)
{
    return is_draw() || get_winner_team() != NONE;
}