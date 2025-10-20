#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

typedef struct {
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Color base_color;
    SDL_Color hover_color;
    char label[50];
    bool hovered;
} Button;

typedef struct {
    SDL_Texture* score_texture;
    SDL_Texture* time_texture;
    TTF_Font* font;
    TTF_Font* input_font; // Ajouter cette ligne
} HUD;

extern HUD hud;
extern int TILE_SIZE_SINGLE;
extern int TILE_SIZE_DUAL;
extern GameData game;
// Nouvelles fonctions
void init_graphics(SDL_Renderer* renderer);
void render_dual_grid(SDL_Renderer* renderer, const GameData* game);
void render_control_buttons(SDL_Renderer* renderer);
void update_vs_hud(SDL_Renderer* renderer, HUD* hud, const GameData* game);
void render_main_menu(SDL_Renderer* renderer, Button* buttons, int num_buttons);
void render_game_over_screen(SDL_Renderer* renderer, HUD* hud, const char* score_str);
SDL_Color get_tile_color(int value);
void destroy_graphics(HUD* hud);
void get_player_name(SDL_Renderer* renderer, HUD* hud, char* name);

#endif
