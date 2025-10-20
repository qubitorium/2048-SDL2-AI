#include "../include/scores.h"
#include <stdio.h>
#include <string.h>

void load_highscores(Highscore highscores[]) {
    FILE* file = fopen("highscores.dat", "rb");
    if(file) {
        fread(highscores, sizeof(Highscore), MAX_HIGHSCORES, file);
        fclose(file);
    } else {
        memset(highscores, 0, sizeof(Highscore) * MAX_HIGHSCORES);
    }
}

void save_highscores(Highscore highscores[]) {
    FILE* file = fopen("highscores.dat", "wb");
    if(file) {
        fwrite(highscores, sizeof(Highscore), MAX_HIGHSCORES, file);
        fclose(file);
    }
}

void add_highscore(Highscore highscores[], const char* name, int score, Uint32 time) {
    Highscore new_score = {0};
    strncpy(new_score.name, name, 49);
    new_score.score = score;
    new_score.time = time;

    // Trouver la position d'insertion
    int pos = MAX_HIGHSCORES;
    while(pos > 0 && (score > highscores[pos-1].score || 
         (score == highscores[pos-1].score && time < highscores[pos-1].time))) {
        pos--;
    }

    if(pos < MAX_HIGHSCORES) {
        memmove(&highscores[pos+1], &highscores[pos], 
               sizeof(Highscore)*(MAX_HIGHSCORES-pos-1));
        highscores[pos] = new_score;
    }
}

// Ajouter l'implémentation
void render_highscores(SDL_Renderer* renderer, TTF_Font* font) {
    Highscore highscores[MAX_HIGHSCORES];
    load_highscores(highscores);
    
    SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
    SDL_RenderClear(renderer);

    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, "Meilleurs Scores", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {270, 50, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    for(int i = 0; i < MAX_HIGHSCORES; i++) {
        char buffer[100];
        sprintf(buffer, "%d. %s - %d pts - %02d:%02d", 
               i+1, highscores[i].name, highscores[i].score, 
               highscores[i].time/60, highscores[i].time%60);
        
        surface = TTF_RenderText_Blended(font, buffer, color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect = (SDL_Rect){200, 150 + i*50, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

int get_best_score(void) {
    Highscore highscores[MAX_HIGHSCORES];
    load_highscores(highscores);
    return (highscores[0].score > 0) ? highscores[0].score : 0;
}
