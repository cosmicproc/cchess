#pragma once

#include "game.h"

void start_game(team first_player);
void end_game(team winner);
void stop_game(void);
void game_loop(team first_player);
void save_game();
void resume_game(char const *filename);
position parse_position(char *str);
int ask_for_move(team player, position *current_pos, position *new_pos);
char ask_for_promotion(team player);