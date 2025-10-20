#ifndef SCORES_H
#define SCORES_H

#include "game.h"

#define MAX_HIGHSCORES 5

typedef struct {
    char name[50];
    int score;
    Uint32 time;
} Highscore;

void load_highscores(Highscore highscores[]);
void save_highscores(Highscore highscores[]);
void add_highscore(Highscore highscores[], const char* name, int score, Uint32 time);
void render_highscores(SDL_Renderer* renderer, TTF_Font* font);
int get_best_score(void);

#endif
