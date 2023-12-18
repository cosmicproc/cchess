#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "helpers.h"

static position last_moves[(REQUIRED_MOVES_DRAW + 2) * 2][2];
static size_t last_moves_cursor = 0;
static bool enough_moves_passed = false;

bool is_valid_move_rook(position current_pos, position new_pos)
{
    return (current_pos.x == new_pos.x ||
            current_pos.y == new_pos.y) &&
           !is_any_piece_in_between(current_pos, new_pos);
}

bool is_valid_move_knight(position current_pos, position new_pos)
{
    return (abs(new_pos.x - current_pos.x) == 2 && abs(new_pos.y - current_pos.y) == 1) ||
           (abs(new_pos.x - current_pos.x) == 1 && abs(new_pos.y - current_pos.y) == 2);
}

bool is_valid_move_bishop(position current_pos, position new_pos)
{
    return (abs(new_pos.x - current_pos.x) == abs(new_pos.y - current_pos.y)) &&
           !is_any_piece_in_between(current_pos, new_pos);
}

bool is_valid_move_queen(position current_pos, position new_pos)
{
    return is_valid_move_bishop(current_pos, new_pos) ||
           is_valid_move_rook(current_pos, new_pos);
}

bool can_castle(position current_pos, position new_pos)
{
    return current_pos.x == 4 && (current_pos.y == 0 || current_pos.y == 7) && !get_piece(current_pos).has_moved &&
           ((new_pos.x == 2 && !board[current_pos.y][0].has_moved &&
             !is_any_piece_in_between(current_pos, POS(0, current_pos.y)) &&
             !is_under_attack(POS(3, current_pos.y), opposite_team(get_piece_team(current_pos))) &&
             !is_under_attack(POS(2, current_pos.y), opposite_team(get_piece_team(current_pos))) &&
             !is_under_attack(POS(1, current_pos.y), opposite_team(get_piece_team(current_pos)))) ||
            (new_pos.x == 6 && !board[current_pos.y][7].has_moved &&
             !is_any_piece_in_between(current_pos, POS(7, current_pos.y)) &&
             !is_under_attack(POS(5, current_pos.y), opposite_team(get_piece_team(current_pos))) &&
             !is_under_attack(POS(6, current_pos.y), opposite_team(get_piece_team(current_pos))))) &&
           new_pos.y == current_pos.y;
}

bool is_valid_move_king(position current_pos, position new_pos)
{
    return (!is_under_attack(new_pos, opposite_team(get_piece_team(current_pos))) &&
            abs(new_pos.x - current_pos.x) <= 1 && abs(new_pos.y - current_pos.y) <= 1) ||
           can_castle(current_pos, new_pos);
}

bool can_en_passant(position current_pos, position new_pos)
{
    return abs(new_pos.x - current_pos.x) == 1 && new_pos.y - current_pos.y == (get_piece_team(current_pos) == WHITE ? 1 : -1) &&
           (str_equal(board[current_pos.y][current_pos.x - 1].symbol, (get_piece_team(current_pos) == WHITE ? B_PAWN : W_PAWN)) ||
            str_equal(board[current_pos.y][current_pos.x + 1].symbol, (get_piece_team(current_pos) == WHITE ? B_PAWN : W_PAWN))) &&
           last_moves[last_moves_cursor - 1][0].y - current_pos.y == (get_piece_team(current_pos) == WHITE ? 2 : -2);
}

bool is_valid_move_pawn(position current_pos, position new_pos)
{
    return (new_pos.x == current_pos.x && new_pos.y - current_pos.y == (get_piece_team(current_pos) == WHITE ? 1 : -1) && is_position_empty(new_pos)) ||
           (new_pos.x == current_pos.x && new_pos.y - current_pos.y == (get_piece_team(current_pos) == WHITE ? 2 : -2) && is_position_empty(new_pos) &&
            !get_piece(current_pos).has_moved && !is_any_piece_in_between(current_pos, new_pos)) ||
           (new_pos.y - current_pos.y == (get_piece_team(current_pos) == WHITE ? 1 : -1) && abs(new_pos.x - current_pos.x) == 1 &&
            !is_position_empty(new_pos)) ||
           can_en_passant(current_pos, new_pos);
}

bool is_valid_move(position current_pos, position new_pos)
{
    if (current_pos.x == new_pos.x && current_pos.y == new_pos.y)
        return false;

    if (is_rook(get_piece(current_pos).symbol))
        return is_valid_move_rook(current_pos, new_pos);
    else if (is_knight(get_piece(current_pos).symbol))
        return is_valid_move_knight(current_pos, new_pos);
    else if (is_bishop(get_piece(current_pos).symbol))
        return is_valid_move_bishop(current_pos, new_pos);
    else if (is_queen(get_piece(current_pos).symbol))
        return is_valid_move_queen(current_pos, new_pos);
    else if (is_king(get_piece(current_pos).symbol))
        return is_valid_move_king(current_pos, new_pos);
    else if (is_pawn(get_piece(current_pos).symbol))
        return is_valid_move_pawn(current_pos, new_pos);
    return false;
}

bool can_move_to(position current_pos, position new_pos)
{
    if (!is_position_valid(current_pos) || !is_position_valid(new_pos) || get_piece_team(current_pos) == get_piece_team(new_pos) ||
        (is_checked(get_piece_team(current_pos)) && is_checked_after_move(get_piece_team(current_pos), current_pos, new_pos)))
        return false;
    return is_valid_move(current_pos, new_pos);
}

void castle(position current_pos, position new_pos)
{
    if (new_pos.x == 2)
    {
        change_position(current_pos, new_pos);
        change_position(POS(0, current_pos.y), POS(3, current_pos.y));
    }
    else if (new_pos.x == 6)
    {
        change_position(current_pos, new_pos);
        change_position(POS(7, current_pos.y), POS(5, current_pos.y));
    }
}

void en_passant(position current_pos, position new_pos)
{
    if (board[current_pos.y][current_pos.x - 1].symbol == (get_piece_team(current_pos) == WHITE ? B_PAWN : W_PAWN))
        empty_position(POS(current_pos.x - 1, current_pos.y));
    else
        empty_position(POS(current_pos.x + 1, current_pos.y));
    change_position(current_pos, new_pos);
}

void make_move(position current_pos, position new_pos)
{
    if (can_castle(current_pos, new_pos))
        castle(current_pos, new_pos);
    else if (can_en_passant(current_pos, new_pos))
        en_passant(current_pos, new_pos);
    else
        change_position(current_pos, new_pos);
    log_move(current_pos, new_pos);
}

bool can_do_any_move(team t)
{
    for (size_t y = 0; y < BOARD_HEIGTH; y++)
        for (size_t x = 0; x < BOARD_WIDTH; x++)
            if (t == WHITE ? is_white(board[y][x].symbol) : is_black(board[y][x].symbol))
                for (size_t y2 = 0; y2 < BOARD_HEIGTH; y2++)
                    for (size_t x2 = 0; x2 < BOARD_WIDTH; x2++)
                        if (is_valid_move(POS(x, y), POS(x2, y2)))
                            return true;
    return false;
}

void log_move(position current_pos, position new_pos)
{
    last_moves[last_moves_cursor][0] = current_pos;
    last_moves[last_moves_cursor][1] = new_pos;
    if (++last_moves_cursor > (REQUIRED_MOVES_DRAW + 2) * 2 - 1)
    {
        last_moves_cursor = 0;
        enough_moves_passed = true;
    }
}

bool are_last_moves_same(void)
{
    if (!enough_moves_passed)
        return false;

    position pos1, pos2;

    for (size_t j = 0; j < 2; j++)
    {
        pos1 = last_moves[j][0], pos2 = last_moves[j][1];
        for (size_t i = 4 + j; i < (REQUIRED_MOVES_DRAW + 2) * 2; i += 4)
            if (!(last_moves[i][0].x == pos1.x && last_moves[i][0].y == pos1.y &&
                  last_moves[i][1].x == pos2.x && last_moves[i][1].y == pos2.y))
                return false;
    }

    return true;
}