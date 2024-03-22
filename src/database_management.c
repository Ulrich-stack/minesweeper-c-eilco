#include "../include/database_management.h"

#define file_games_easy_completed "games_completed_easy.txt"
#define file_games_medium_completed "games_completed_medium.txt"
#define file_games_hard_completed "games_completed_hard.txt"
#define file_games_non_completed "games_non_completed.txt"

// data form in the file : {id;name ;width;height;mines;flags;revealed;score;over}

void print_char(char c, int n) {
    for (int i = 0; i < n; i++) printf("%c", c);
}

void save_game(Game *game) {
    FILE *file, *tmpFile;
    char *fileName, chaine[10000];
    int nbrLignes;

    if (!game) return;

    if (!isGameWon(game)) fileName = file_games_non_completed;
    else {
        if (game_level(game) == 0) fileName = file_games_easy_completed;
        if (game_level(game) == 1) fileName = file_games_medium_completed;
        if (game_level(game) == 2) fileName = file_games_hard_completed;
    }

    file = fopen(fileName, "a+");
    tmpFile = fopen("tmpFile", "a+");
    if (!file) {
        printf("Error on file %s: %s", fileName, strerror(errno));
        if (tmpFile) fclose(tmpFile);
        exit(EXIT_FAILURE);
    }
    if (!tmpFile) {
        printf("Error on file tmpfile: %s", strerror(errno));
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // count the number of lines in the files
    nbrLignes = number_of_lines_in_file(file) + 1;

    nbrLignes = nbrLignes >= 10 ? 9 : nbrLignes;
    game->id = nbrLignes--; // attribution of game id

    // removing spaces
    for (int i = 0; i < strlen(game->name); i++)
        if (game->name[i] == ' ') game->name[i] = '|';

    // writing game's characteristic
    fprintf(tmpFile, "{%d;%s ;%d-%d;%d;%d;%d;%d;%d}", game->id, game->name, game->grid.size.width,
            game->grid.size.height, game->mines, game->flags, game->revealed, game->score, game->over);

    // writing the characteristics of the game cells
    if (!game->over) {
        for (int i = 0; i < game->grid.size.height; i++) {
            for (int j = 0; j < game->grid.size.width; j++) {
                fprintf(tmpFile, "-{%d-%d;%d;%d;%d}", game->grid.cells[i][j].position.x,
                        game->grid.cells[i][j].position.y,
                        game->grid.cells[i][j].revealed, game->grid.cells[i][j].value, game->grid.cells[i][j].flagged);
            }
        }
    }
    fprintf(tmpFile, "\n");

    // we copy the old backup
    rewind(file);
    while ((fgets(chaine, 10000, file)) != NULL && nbrLignes >= 1) {
        chaine[1] = (char) ('0' + nbrLignes--);
        fputs(chaine, tmpFile);
    }
    fclose(file);
    fclose(tmpFile);

    remove(fileName);
    rename("tmpFile", fileName);
}

Game *load_game(int id) {
    FILE *file = NULL, *tmpFile = NULL;
    file = fopen(file_games_non_completed, "r");
    int over, nbrLignes;
    char ch = '1';
    Game *game = malloc(sizeof(Game));

    if (!file) {
        printf("Error on file %s: %s", file_games_non_completed, strerror(errno));
        if (game) free(game);
        exit(EXIT_FAILURE);
    }
    if (!game) {
        printf("Error during allocation");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // count the number of line in the file
    nbrLignes = number_of_lines_in_file(file);

    if (!nbrLignes) {
        printf("Aucune sauvegarde trouvée");
        free(game);
        fclose(file);
        return NULL;
    }
    id = nbrLignes - id + 1;

    while (ch != EOF) {
        // fetch a game
        fscanf(file, "{%d;%s ;%d-%d;%d;%d;%d;%d;%d}", &game->id, game->name, &game->grid.size.width,
               &game->grid.size.height, &game->mines, &game->flags, &game->revealed, &game->score, &over);
        game->over = over;

        // check if it's the right game
        if (game->id == id) {
            // reintroduces spaces
            for (int i = 0; i < strlen(game->name); i++)
                if (game->name[i] == '|') game->name[i] = ' ';

            game->grid.cells = (Cell **) malloc(game->grid.size.height * sizeof(Cell *));
            if (!game->grid.cells) {
                printf("Error during allocation");
                free(game);
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // fetching game cells information
            for (int i = 0; i < game->grid.size.height; i++) {
                game->grid.cells[i] = (Cell *) malloc(game->grid.size.width * sizeof(Cell));
                for (int j = 0, revealed, flagged; j < game->grid.size.width; j++) {
                    fscanf(file, "-{%d-%d;%d;%d;%d}", &game->grid.cells[i][j].position.x,
                           &game->grid.cells[i][j].position.y, &revealed, &game->grid.cells[i][j].value, &flagged);
                    game->grid.cells[i][j].revealed = revealed;
                    game->grid.cells[i][j].flagged = flagged;
                }
            }

            // delete the game laoded
            tmpFile = fopen("tmpFile", "a+");
            if (!tmpFile) {
                printf("Error on file tmpfile: %s", strerror(errno));
                fclose(file);
                exit(EXIT_FAILURE);
            }
            rewind(file);

            char chaine[10000];
            while ((fgets(chaine, 10000, file)) != NULL && nbrLignes >= 1) {
                if ((int) (chaine[1] - '0') == id) continue;
                fputs(chaine, tmpFile);
            }

            fclose(file);
            fclose(tmpFile);

            remove(file_games_non_completed);
            rename("tmpFile", file_games_non_completed);
            return game;
        }

        // line code to go to the next line
        while ((ch = (char) fgetc(file)) != EOF && ch != '\n');
    }

    fclose(file);
    free(game);
    return NULL;
}

int print_for_loading() {
    FILE *file = NULL;
    file = fopen(file_games_non_completed, "r");
    char ch;
    int over, nbrLignes = 0;
    Game game;

    if (!file) {
        printf("Aucune sauvegarde trouvée");
        return nbrLignes;
    }

    // count the number of line in the file
    nbrLignes = number_of_lines_in_file(file);

    if (!nbrLignes) {
        printf("La sauvegarde est vide");
        fclose(file);
        return nbrLignes;
    }


    // table head
    horitontal_line(false);
    table_head(false);
    horitontal_line(false);

    // fetch each game and print it
    rewind(file);
    for (int i = 0; i < nbrLignes; i++) {
        fscanf(file, "{%d;%s ;%d-%d;%d;%d;%d;%d;%d}", &game.id, game.name, &game.grid.size.width,
               &game.grid.size.height, &game.mines, &game.flags, &game.revealed,
               &game.score, &over);
        game.over = over;

        // reintroduces spaces
        for (int j = 0; j < strlen(game.name); j++)
            if (game.name[j] == '|') game.name[j] = ' ';

        game.id = i + 1;
        table_row(&game, game_level(&game) == 0 ? "Facile" : game_level(&game) == 1 ? "Moyen" : "Difficile", false);
        horitontal_line(false);

        // line code to go to the next line
        while ((ch = (char) fgetc(file)) != EOF && ch != '\n');
    }

    fclose(file);
    return nbrLignes;
}

Game *fetch_stats(int level, int *size) {
    char *fileName, ch;
    FILE *file = NULL;
    int nbrLignes = 0;
    Game *listeGames = NULL;

    if (level == 0) fileName = file_games_easy_completed;
    if (level == 1) fileName = file_games_medium_completed;
    if (level == 2) fileName = file_games_hard_completed;

    file = fopen(fileName, "r");

    if (!file) {
        printf("Aucune statistique trouvée");
        return NULL;
    }

    nbrLignes = number_of_lines_in_file(file);
    if (!nbrLignes) {
        printf("La sauvegarde est vide");
        fclose(file);
        return NULL;
    }

    listeGames = (Game *) malloc(nbrLignes * sizeof(Game));

    if (!listeGames) {
        printf("Error during allocation");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    *size = nbrLignes;
    for (int i = 0, over; i < nbrLignes; i++) {
        fscanf(file, "{%d;%s ;%d-%d;%d;%d;%d;%d;%d}", &listeGames[i].id, listeGames[i].name,
               &listeGames[i].grid.size.width,
               &listeGames[i].grid.size.height, &listeGames[i].mines, &listeGames[i].flags, &listeGames[i].revealed,
               &listeGames[i].score, &over);
        listeGames[i].over = over;

        // reintroduces spaces
        for (int j = 0; j < strlen(listeGames[i].name); j++)
            if (listeGames[i].name[j] == '|') listeGames[i].name[j] = ' ';

        listeGames[i].id = i + 1;

        // line code to go to the next line
        while ((ch = (char) fgetc(file)) != EOF && ch != '\n');
    }

    fclose(file);
    trierParTas(listeGames, size);

    return listeGames;
}

void print_statistics(int level) {
    int size = 0;
    Game *listGame = fetch_stats(level, &size);

    if (size <= 0) {
        printf("\n");
        return;
    }

    printf("Affichage des statistiques pour le niveau %s\n\n",
           level == 0 ? "facile" : level == 1 ? "moyen" : "difficile");
    // table head
    horitontal_line(true);
    table_head(true);
    horitontal_line(true);

    for (int i = 0; i < size; i++) {
        listGame[i].id = i + 1;
        table_row(&listGame[i],
                  game_level(&listGame[i]) == 0 ? "Facile" : game_level(&listGame[i]) == 1 ? "Moyen" : "Difficile",
                  true);
        horitontal_line(true);
    }
    free(listGame);
}

int number_of_lines_in_file(FILE *file) {
    int nbrLignes = 0;
    char ch = '1';
    while ((ch = (char) fgetc(file)) != EOF) {
        if (ch == '\n') nbrLignes++;
    }
    rewind(file);
    return nbrLignes;
}

void table_head(bool isStat) {
    printf("| Id ");
    print_char('|', 1);
    print_char(' ', (GAME_NAME_SIZE - strlen("name")) / 2);
    printf("Noms");
    print_char(' ', (GAME_NAME_SIZE - strlen("name")) / 2);
    printf("|");

    if (!isStat) {
        print_char(' ', 3);
        printf("Niveaux");
        print_char(' ', (10 - strlen("niveaux")));
        printf("|");
        printf(" Mines ");
        printf("| ");
    }

    printf(" Drapeaux ");
    printf("|");
    if (!isStat) {
        printf(" Revelees ");
        printf("|");
    }

    print_char(' ', (strlen("23h59m59s") - strlen("temps")) / 2);
    printf(" Temps ");
    print_char(' ', (strlen("23h59m59s") - strlen("temps")) / 2);
    printf("|\n");
}

void table_row(Game *game, char *level, bool isStat) {
    char str[GAME_NAME_SIZE];
    if (game) {
        if (game->id < 10) {
            sprintf(str, " %d", game->id);
        } else {
            sprintf(str, "%d", game->id);
        }
        printf("| %s ", str);
        print_char('|', 1);
        sprintf(str, "%s", game->name);
        print_char(' ', (GAME_NAME_SIZE - (int) strlen(str)) / 2);
        printf("%s", str);
        print_char(' ', (GAME_NAME_SIZE - (int) strlen(str)) / 2 + (strlen(str) % 2 == 1));


        if (!isStat) {
            printf("|");
            if (strlen(level)) {
                print_char(' ', 3);
                printf("%s", level);
                print_char(' ', (int) (10 - strlen(level)));
                printf("|");
            }
            sprintf(str, "%d", game->mines);
            print_char(' ', (int) (strlen("Mines") + 2 - strlen(str)) / 2);
            printf("%s", str);
            print_char(' ', (int) (strlen("Mines") + 2 - strlen(str)) / 2 + (strlen(str) % 2 == 0));
            printf("|");

            sprintf(str, "%d", game->revealed);
            print_char(' ', (int) (strlen("Revelees") + 2 - strlen(str)) / 2);
            printf("%s", str);
            print_char(' ', (int) (strlen("Revelees") + 2 - strlen(str)) / 2 + (strlen(str) % 2 == 1));
            printf(" ");
        }

        printf("|");
        if (game->flags < 10)
            sprintf(str, "%d ", game->flags);
        else
            sprintf(str, "%d", game->flags);

        print_char(' ', (int) (strlen("Drapeaux") + 2 - (int) strlen(str)) / 2);
        printf("%s", str);
        print_char(' ', (int) (strlen("Drapeaux") + 2 - (int) strlen(str)) / 2 + (strlen(str) % 2 == 1));
        printf("|");


        if (game->score >= 60) {
            int gameTime = game->score;
            print_char(' ', (int) (strlen("23h59m59s") - strlen(str)) / 2);
            if ((int) (gameTime / 3600) != 0) {
                sprintf(str, "%dh", gameTime / 3600);
                printf("%s", str);
                gameTime = gameTime % 3600;
            }
            if ((int) (gameTime / 60) != 0) {
                sprintf(str, "%dm", gameTime / 60);
                printf("%s", str);
                gameTime = gameTime % 60;
            }
            sprintf(str, "%ds", gameTime);
            printf("%s", str);
        } else {
            sprintf(str, "%ds", game->score);
            print_char(' ', (int) (strlen("23h59m59s") - strlen(str)) / 2);
            printf(" %s ", str);
        }
        print_char(' ', (int) (strlen("23h59m59s") - strlen(str)) / 2 + (strlen(str) % 2 == 0));
        printf("|\n");
    }
}

void horitontal_line(bool isStat) {
    print_char('+', 1);
    print_char('-', 4);
    print_char('+', 1);
    print_char('-', 40);
    print_char('+', 1);

    if (!isStat) {
        print_char('-', 10 + 3);
        print_char('+', 1);
        print_char('-', strlen("mines") + 2);
        print_char('+', 1);
        print_char('-', 1);
    }

    print_char('-', strlen("Drapeaux") + 2);
    print_char('+', 1);
    if (!isStat) {
        print_char('-', strlen("Revelees") + 2);
        print_char('+', 1);
    }

    print_char('-', strlen("23h59m59s") + 2);
    print_char('+', 1);
    printf("\n");
}

int game_level(Game *game) {
    Size easy = EASY_SIZE;
    Size medium = MEDIUM_SIZE;
    Size hard = HARD_SIZE;

    if (game) {
        if (game->grid.size.width == easy.width && game->grid.size.height == easy.height) return 0;
        if (game->grid.size.width == medium.width && game->grid.size.height == medium.height) return 1;
        if (game->grid.size.width == hard.width && game->grid.size.height == hard.height) return 2;
    }
    return 0;
}

void echanger_tas(Game *tas, int i, int j) {
    if (!tas) return;
    Game stock = tas[i];
    tas[i] = tas[j];
    tas[j] = stock;
}

void inserer(Game *heap, int *size, Game game) {
    if (!heap || !size) return;
    (*size)++;
    heap[*size] = game;
    int i = *size;
    while ((i > 1) && (heap[i].score < heap[i / 2].score)) {
        echanger_tas(heap, i, i / 2);
        i = i / 2;
    }
}

void deleteMin(Game *heap, int *size) {
    int i = 1, j;
    bool stop = false;

    if (!heap || !size) return;

    heap[1] = heap[*size];
    (*size)--;
    while ((i <= (*size) / 2) && !stop) {
        if ((2 * i == *size) || (heap[2 * i].score < heap[2 * i + 1].score)) j = 2 * i;
        else j = 2 * i + 1;

        if (heap[i].score > heap[j].score) {
            echanger_tas(heap, i, j);
            i = j;
        } else stop = true;
    }
}

void trierParTas(Game *tableau, const int *size) {
    if (!size) {
        printf("Error during allocation");
        exit(EXIT_FAILURE);
    }
    Game *heap = NULL;
    int heap_size = 0;

    heap = (Game *) malloc((*size + 1) * sizeof(Game));  // Allocate enough memory for the heap
    if (!heap) {
        printf("Error during allocation");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *size; i++) inserer(heap, &heap_size, tableau[i]);
    for (int i = 0; i < *size; i++) {
        tableau[i] = heap[1];
        deleteMin(heap, &heap_size);
    }
    free(heap);
}