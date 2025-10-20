#include "../include/graphics.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include "../include/global.h"
#include "../include/scores.h"

#define TILE_SIZE_SINGLE 100
#define GRID_SPACING_SINGLE 10
#define TILE_SIZE_DUAL 80
#define GRID_SPACING_DUAL 8
#define DUAL_GRID_MARGIN 40 


void init_graphics(SDL_Renderer* renderer) {
    if(TTF_Init() == -1) {
        printf("Erreur SDL_ttf: %s\n", TTF_GetError());
        exit(1);
    }
    
    // Charger la police (ajuster le chemin selon votre système)
    hud.font = TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Bold.ttf", 24);

    if(!hud.font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        exit(1);
    }

    hud.input_font = TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Light.ttf", 32);
    if (!hud.input_font) {
        printf("ERREUR: Impossible de charger la police pour hud.font (%s)\n", TTF_GetError());
        exit(1);
    }
    
    hud.score_texture = NULL;
    hud.time_texture = NULL;
}

void render_tile(SDL_Renderer* renderer, int x, int y, int value) {
    // Déterminer la taille en fonction du mode
    int tile_size = (game.mode == HUMAN_VS_AI) ? TILE_SIZE_DUAL : TILE_SIZE_SINGLE;
    
    SDL_Color color = get_tile_color(value);
    SDL_Rect rect = {x, y, tile_size, tile_size};
    
    // Dessiner la tuile
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Ajuster la taille du texte si nécessaire
    if(value > 0) {
        char str[12];
        snprintf(str, sizeof(str), "%d", value);
        SDL_Color text_color = {0x77, 0x6E, 0x65, 0xFF};
        
        SDL_Surface* surface = TTF_RenderText_Blended(hud.font, str, text_color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        SDL_Rect text_rect = {
            x + (tile_size - surface->w)/2,
            y + (tile_size - surface->h)/2,
            surface->w,
            surface->h
        };
        
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void render_single_grid(SDL_Renderer* renderer, const Tile grid[4][4], bool is_player) {
    int total_grid_width = 4 * TILE_SIZE_SINGLE + 3 * GRID_SPACING_SINGLE;
    int base_x = (800 - total_grid_width) / 2; // Centrage horizontal
    int base_y = 100; // Position verticale fixe
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            int pos_x = base_x + x * (TILE_SIZE_SINGLE + GRID_SPACING_SINGLE);
            int pos_y = base_y + y * (TILE_SIZE_SINGLE + GRID_SPACING_SINGLE);
            render_tile(renderer, pos_x, pos_y, grid[y][x].value);
        }
    }
}

void render_dual_grid(SDL_Renderer* renderer, const GameData* game) {
    // Calcul des positions
    int total_grid_width = 4 * TILE_SIZE_DUAL + 3 * GRID_SPACING_DUAL;
    int left_start_x = (800 - (2 * total_grid_width + DUAL_GRID_MARGIN)) / 2;
    int right_start_x = left_start_x + total_grid_width + DUAL_GRID_MARGIN;

    // Grille IA (gauche)
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            int pos_x = left_start_x + x * (TILE_SIZE_DUAL + GRID_SPACING_DUAL);
            int pos_y = 150 + y * (TILE_SIZE_DUAL + GRID_SPACING_DUAL);
            render_tile(renderer, pos_x, pos_y, game->ai_grid[y][x].value);
        }
    }

    // Grille Joueur (droite)
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            int pos_x = right_start_x + x * (TILE_SIZE_DUAL + GRID_SPACING_DUAL);
            int pos_y = 150 + y * (TILE_SIZE_DUAL + GRID_SPACING_DUAL);
            render_tile(renderer, pos_x, pos_y, game->player_grid[y][x].value);
        }
    }
}

void render_control_buttons(SDL_Renderer* renderer) {
    // Bouton Pause
    SDL_Rect pause_rect = {230, 545, 150, 40}; // 230, 550, 150, 40
    SDL_SetRenderDrawColor(renderer, 241, 196, 15, 255);
    SDL_RenderFillRect(renderer, &pause_rect);
    
    SDL_Surface* surface = TTF_RenderText_Blended(hud.font, "Pause", (SDL_Color){255, 255, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){270, 550, surface->w, surface->h});
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Bouton Arrêt
    SDL_Rect stop_rect = {440, 545, 150, 40};
    SDL_SetRenderDrawColor(renderer, 231, 76, 60, 255);
    SDL_RenderFillRect(renderer, &stop_rect);
    
    surface = TTF_RenderText_Blended(hud.font, "Arret", (SDL_Color){255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){480, 550, surface->w, surface->h});
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void update_vs_hud(SDL_Renderer* renderer, HUD* hud, const GameData* game) {
    char buffer[50];
    
    // Score IA (uniquement en mode AI ou VS)
    if(game->mode == AI || game->mode == HUMAN_VS_AI) {
        sprintf(buffer, "MC : %d", game->ai_score);
        SDL_Surface* surface = TTF_RenderText_Blended(hud->font, buffer, (SDL_Color){255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){50, 50, surface->w, surface->h});
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Score Joueur (uniquement en mode HUMAN ou VS)
    if(game->mode == HUMAN || game->mode == HUMAN_VS_AI) {
        sprintf(buffer, "Joueur : %d", game->player_score);
        SDL_Surface* surface = TTF_RenderText_Blended(hud->font, buffer, (SDL_Color){255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){602, 50, surface->w, surface->h});
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

                    sprintf(buffer, "Best : %d", get_best_score());
                    surface = TTF_RenderText_Blended(hud->font, buffer, (SDL_Color){255, 255, 255});
                    texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){630, 100, surface->w, surface->h});
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
    }


    // Timer
    Uint32 elapsed;
    if (game->state == PLAYING ) {
    elapsed = (SDL_GetTicks() - game->start_time) / 1000;
    } else {
    elapsed = game->final_time; // Temps figé dans les autres états
    }

    sprintf(buffer, "%02d:%02d", elapsed/60, elapsed%60);
    SDL_Surface* surface = TTF_RenderText_Blended(hud->font, buffer, (SDL_Color){255, 255, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {(800 - surface->w)/2, 50, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

SDL_Color get_tile_color(int value) {
    switch(value) {
        case 2:    return (SDL_Color){238, 228, 218, 255};
        case 4:    return (SDL_Color){237, 224, 200, 255};
        case 8:    return (SDL_Color){242, 177, 121, 255};
        case 16:   return (SDL_Color){245, 149, 99, 255};
        case 32:   return (SDL_Color){246, 124, 95, 255};
        case 64:   return (SDL_Color){246, 94, 59, 255};
        case 128:  return (SDL_Color){237, 207, 114, 255};
        case 256:  return (SDL_Color){237, 204, 97, 255};
        case 512:  return (SDL_Color){237, 200, 80, 255};
        case 1024: return (SDL_Color){237, 197, 63, 255};
        case 2048: return (SDL_Color){237, 194, 46, 255};
        default:   return (SDL_Color){205, 193, 180, 255};
    }
}

void render_game_over_screen(SDL_Renderer* renderer, HUD* hud, const char* score_str) {
    SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
    SDL_RenderClear(renderer);

    if (!hud->font) {
        printf("Erreur: Police non chargée !\n");
        return;
    }

    SDL_Color white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(hud->font, score_str, white);
    if (!surface) {
        printf("Erreur création surface: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {(800 - surface->w)/2, 250, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void destroy_graphics(HUD* hud) {
    TTF_CloseFont(hud->font);
    if(hud->score_texture) SDL_DestroyTexture(hud->score_texture);
    if(hud->time_texture) SDL_DestroyTexture(hud->time_texture);
    TTF_Quit();
}
