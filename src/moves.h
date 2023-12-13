#pragma once

#include "board.h"

bool is_valid_move_rook(position current_pos, position new_pos);
bool is_valid_move_knight(position current_pos, position new_pos);
bool is_valid_move_bishop(position current_pos, position new_pos);
bool is_valid_move_queen(position current_pos, position new_pos);
bool can_castle(position current_pos, position new_pos);
bool is_valid_move_king(position current_pos, position new_pos);
bool is_valid_move_pawn(position current_pos, position new_pos);
bool is_valid_move(position current_pos, position new_pos);
bool can_move_to(position current_pos, position new_pos);
int castle(position current_pos, position new_pos);
void make_move(position current_pos, position new_pos);
bool can_do_any_move(team t);
void log_move(position current_pos, position new_pos);
bool are_last_moves_same(void);

#define REQUIRED_MOVES_DRAW 3