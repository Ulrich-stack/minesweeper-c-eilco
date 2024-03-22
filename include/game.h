#ifndef GAME_H
#define GAME_H

#define GAME_NAME_SIZE 40

#define EASY_MINES 10
#define MEDIUM_MINES 40
#define HARD_MINES 99


#define EASY_SIZE (Size){11, 11}
//#define EASY_SIZE (Size){9, 9}
#define MEDIUM_SIZE (Size){16, 16}
#define HARD_SIZE (Size){26, 16}

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// Let's define the game structures

typedef struct position {
    int x;
    int y;
} Position;

typedef struct size {
    int width;
    int height;
} Size;

// now to the cell structure
typedef struct cell {
    Position position; // position of the cell in the grid
    bool revealed; // is the cell revealed?
    int value; // value of the cell
    bool flagged; // is the cell flagged?
} Cell;

// grid is a 2D array of integers
typedef struct grid {
    Cell **cells;
    Size size;
} Grid;

// now to the game structure
typedef struct game {
    int id; // id of the game
    char name[GAME_NAME_SIZE]; // name of the game
    Grid grid; // the grid of the game
    int mines; // number of mines in the game
    int flags; // number of flags in the game
    int revealed; // number of revealed cells in the game
    int score; // score of the game
    bool over; // is the game over?
} Game;

typedef enum difficulty {
    EASY,
    MEDIUM,
    HARD,
    NONE
} Difficulty;

typedef enum inGameAction {
    REVEAL = 'r',
    FLAG = 'f',
    UNFLAG = 'u',
    SAVE = 's',
    QUIT = 'q'
} InGameAction;

// now to the functions
Cell createCell(int x, int y);

// create a new grid and fills it with 0
Grid newGrid(Size size);

// create a new game
Game *newGameGeneration(int id, char *name, Size size, int mines);

// initialize a game (fill the grid with mines and values)
void initGame(Game *game, int x, int y);

// generate mines from shuffling
void generateMinesFromShuffling(Game *game, int *guessingList);

// add one to the value of the cells around a mine
void addOneToCellsAroundMine(Grid *grid, int haut, int larg);

// print a grid (for debug purposes)
void printGrid(Grid grid);

// print a grid from a player's perspective (for debug purposes)
void printGridFromPlayerPerspective(Grid grid);

int removeSurroundingCells(const Game *game, int x, int y, int *guessingList, int range);

void generateMinesFromRandomGuess(Game *game, int *guessingList, int range);

void generateFromShuffling(const Game *game, int *guessingList);

void revealAdjacentCells(Game *pGame, int x, int y);

// reveal a cell
void revealCell(Game *game, int x, int y);

bool isGameWon(const Game *game);

// reveal all cells
void revealAllCells(Game *game);

// game finish
void handleGameFinish(Game *game, time_t time1, bool won);

// create new game
Game *setupNewGame();

// play a game
void playGame(Game *game);

// launch minesweeper
void launchMinesweeper();

void quitGame(Game *game);

#endif // GAME_H
