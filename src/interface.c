#include"../include/interface.h"
#include "../include/database_management.h"
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
void clearScreen()
{
    system("cls"); //Delete the shell for windows
}

void setConsoleToUTF8()
{
    SetConsoleOutputCP(65001); // Set the console to UTF8 for special caracters
}
#else

int fistLaunch = 1;

//Delete non-windows shell
void clearScreen() {
    system("clear");
}

void setConsoleToUTF8() {
}

#endif


void afficherGrille(Game *game) {
    char *helpList[] = {"Aide de jeu",
                        "Entrez une action suivie des coordonnees (ex: ra4)",
                        "Actions possibles: ",
                        "r: reveler une case",
                        "f: placer un drapeau",
                        "u: enlever un drapeau",
                        "s: sauvegarder la partie",
                        "q: quitter la partie",
                        "Nombre de drapeaux disponibles: ",
                        "■ : Cellule cachée; ⚑ : Drapeau; ☑ : Drapeau juste; ✹ : Mines;",
    };
    int helpListIndex = 0;
    int width = game->grid.size.width;
    int height = game->grid.size.height;
    Cell **cells = game->grid.cells;

    printf("  ");
    for (int i = 0; i < width; ++i)
        printf("  %c ", (char) ('a' + i));
    printf("\n");
    printf("  ╔");
    for (int i = 0; i < width - 1; ++i)
        printf("═══╦");
    printf("═══╗\n");

    for (int i = 0; i < height; ++i) {
        if (i <= 8)
            printf(" %d", i + 1);
        else
            printf("%d", i + 1);

        printf("║");
        for (int j = 0; j < width; ++j) {
            if (!cells[i][j].revealed) {
                if (cells[i][j].flagged) {
                    printf(" ⚑ ║");
                } else {
                    printf(" ■ ║");
                }
            } else {
                if (cells[i][j].flagged && cells[i][j].value == -1) {
                    printf(" ☑ ║");
                } else if (cells[i][j].value == -1)
                    printf(" ✹ ║");
                else {
                    if (cells[i][j].value == 0)
                        printf("   ║");
                    else
                        printf(" %d ║", cells[i][j].value);
                }
            }
        }


        printf("\n");
        if (i < height - 1) {
            printf("  ╠");
            for (int j = 0; j < width - 1; ++j)
                printf("═══╬");
            printf("═══╣");
            if (helpListIndex < 10) printf("\t\t%s", helpList[helpListIndex++]);
            if (helpListIndex - 1 == 8) {
                printf("%d", game->flags);
            }
            printf("\n");
        }
    }

    printf("  ╚");
    for (int i = 0; i < width - 1; ++i)
        printf("═══╩");
    printf("═══╝\n");

    while (helpListIndex < 10) {
        printf("\t\t\t\t\t\t%s", helpList[helpListIndex++]);
        if (helpListIndex - 1 == 8) printf("%d", game->flags);
        printf("\n");
    }
}


void getActionFromUser(Game *game, Position *position, InGameAction *action) {
    char entrees[100] = {'r', 'a', '1'};

    getInput:
    afficherGrille(game);
    printf("\n>> ");
    scanf("%s", entrees);
    getchar();
    entrees[4] = '\0';

    position->x = (int) (tolower(entrees[1]) - 'a'); //to convert the second caracter from char to int
    position->y = (int) (atoi(entrees + 2) - 1);
    *action = (InGameAction) (char) tolower(entrees[0]);


    if ((*action == QUIT || *action == SAVE) && strlen(entrees) == 1) {
        return;
    } else {
        if (*action != REVEAL && *action != FLAG && *action != UNFLAG) {
            clearScreen();
            printf("Action invalide \n");
            goto getInput;
        }
        if (position->x < 0 || position->x >= game->grid.size.width || position->y < 0 ||
            position->y >= game->grid.size.height) {
            clearScreen();
            printf("Coordonnees invalides\n");
            goto getInput;
        }
    }
}

void afficherDemineur() {
    printf("  \t\tLL      EEEEE\t  DDDD     EEEEE  MMM   MMM IIIII NN   NN EEEEE UU   UU RRRR   \n");
    printf("  \t\tLL      EE   \t  DD   DD  EE     MM M M MM   I   NNN  NN EE    UU   UU RR  RR \n");
    printf("  \t\tLL      EEEE \t  DD   DD  EEEE   MM  M  MM   I   NN N NN EEEE  UU   UU RRRR   \n");
    printf("  \t\tLL      EE   \t  DD   DD  EE     MM     MM   I   NN  NNN EE    UU   UU RR  RR \n");
    printf("  \t\tLLLLLL  EEEEE\t  DDDD     EEEEE  MM     MM IIIII NN   NN EEEEE  UUUUU  RR   RR\n\n\n");
    printf("\t\t\t\t\tEntrez une touche pour continuer!");
}

void printMenu() {
//    declaring a static variable to check if it's the first launch of the game

    static int firstLaunch = 1;
    if (firstLaunch) {
        afficherDemineur();
        getchar();
        clearScreen();

        firstLaunch = 0;
    } else {
        printf("MENU\n");
    }
    printf("1. Lancer une nouvelle partie\n");
    printf("2. Charger une partie\n");
    printf("3. Voir les statistiques\n");
    printf("4. Quitter le jeu\n");
}

void menu() {
    int choice;
    clearScreen();
    printMenu();

    printf(">> ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
        case 1:
            playGame(setupNewGame());
            break;
        case 2:
            loadGame();
            break;
        case 3:
            printStatistics();
            break;
        case 4:
            quitGame(NULL);
            break;
        default:
            printf("Choix invalide\n");
            menu();
            break;
    }
}

void getDifficultySettingsFromUser(int *mines, Size *size) {
    int choice;

    clearScreen();
    printf("Choisir la difficulte\n");
    printf("1. Facile\n");
    printf("2. Moyen\n");
    printf("3. Difficile\n");
    printf("4. Retourner au menu\n");

    printf(">> ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
        case 1:
            *mines = EASY_MINES;
            *size = EASY_SIZE;
            break;
        case 2:
            *mines = MEDIUM_MINES;
            *size = MEDIUM_SIZE;
            break;
        case 3:
            *mines = HARD_MINES;
            *size = HARD_SIZE;
            break;
        case 4:
            menu();
            break;
        default:
            printf("Choix invalide\n");
            getDifficultySettingsFromUser(mines, size);
            break;
    }
}

void loadGame() {
    int choice = -1, nbrLigne;
    printf("\nEnsemble des jeux sauvegardes\n");
    nbrLigne = print_for_loading();

    if (nbrLigne == 0) {
        printf("\nTap pour retourner au menu");
        getchar();
        menu();
    }

    do {
        printf("Selectionner une partie à charger (un nombre entre 1 et %d, 0 pour retourner au menu) : ", nbrLigne);

        if (scanf("%d", &choice) != 1 || !((-1 < choice) && (choice <= nbrLigne))) {
            printf("Choix invalide\n");

            while (getchar() != '\n');

            continue;
        }

    } while (!((-1 < choice) && (choice <= nbrLigne)));

    if (choice == 0) {
        menu();
    } else {
        playGame(load_game(choice));
    }
}

void printStatistics() {
    int choice = -1;
    printf("\nEnsemble des statistiques\n");

    while (!(0 <= choice && choice <= 3)) {
        clearScreen();
        printf("\nEnsemble des statistiques\n\n");
        printf("Faites un choix :\n1. Facile\n2. Moyen\n3. Difficile\n0. Retour\n>> ");
        scanf("%d", &choice);
    }

    if (choice == 0) {
        menu();
    } else {
        print_statistics(choice - 1);

        choix:
        printf("\n\nEntrez un chiffre\n0. Retour au menu\n1. Quitter le jeu\n>> ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                menu();
                break;
            case 1:
                quitGame(NULL);
                break;
            default:
                goto choix;
                break;
        }
    }
}

void getGameInformationFromUser(Game *game) { //Enter a name for the game
    printf("Entrer un nom pour la partie : ");
    scanf("%[^\n]", game->name);
}

void print_game_time(int gameTime) { //Fonction to print the time
    printf("Temps de jeu : ");
    if ((int) (gameTime / 3600) != 0) {
        printf("%dh ", gameTime / 3600);
        gameTime = gameTime % 3600;
    }
    if ((int) (gameTime / 60) != 0) {
        printf("%dmin ", gameTime / 60);
        gameTime = gameTime % 60;
    }
    printf("%ds\n", gameTime);
}


