#ifndef DATABASE_MANAGEMENT_H
#define DATABASE_MANAGEMENT_H

#include "game.h"

void save_game(Game *game);

Game *load_game(int id);

int print_for_loading();

Game *fetch_stats(int level, int *size);

void print_statistics(int level);

void horitontal_line(bool isStat);

void table_row(Game *game, char *level, bool isStat);

void table_head(bool isStat);

int number_of_lines_in_file(FILE *file);

int game_level(Game *game);

void echanger_tas(Game *tas, int i, int j);

void inserer(Game *heap, int *size, Game game);

void deleteMin(Game *heap, int *size);

void trierParTas(Game *tableau, const int *size);

#endif