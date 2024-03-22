#ifndef INTERFACE_H
#define INTERFACE_H

#include "game.h"

void setConsoleToUTF8();

void clearScreen();

void afficherGrille(Game *game);

void printMenu();

void loadGame();

void printStatistics();

// show and handle the menu
void menu();

// get difficulty settings from user
void getDifficultySettingsFromUser(int *mines, Size *size);

// get action from user
void getActionFromUser(Game *game, Position *position, InGameAction *action);

void getGameInformationFromUser(Game *game);

void print_game_time(int gameTime);

#endif // !INTERFACE_H