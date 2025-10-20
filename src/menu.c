
#include "../include/menu.h"
#include "../include/game.h"
#include "../include/global.h"

void init_main_menu(Button* buttons) {
    // Bouton Joueur
    buttons[0] = (Button){
        .rect = {300, 200, 200, 50},
        .base_color = {46, 204, 113, 255},
        .hover_color = {39, 174, 96, 255},
        .label = "Joueur"
    };

    // Bouton Machine
    buttons[1] = (Button){
        .rect = {300, 260, 200, 50},
        .base_color = {52, 152, 219, 255},
        .hover_color = {41, 128, 185, 255},
        .label = "Machine"
    };

    // Bouton Joueur vs Machine
    buttons[2] = (Button){
        .rect = {300, 320, 200, 50},
        .base_color = {155, 89, 182, 255},
        .hover_color = {142, 68, 173, 255},
        .label = "Joueur vs MC"
    };

    // Bouton Reprendre
    buttons[3] = (Button){
        .rect = {300, 380, 200, 50},
        .base_color = {241, 196, 15, 255},
        .hover_color = {243, 156, 18, 255},
        .label = "Reprise"
    };

    // Bouton Scores
    buttons[4] = (Button){
        .rect = {300, 440, 200, 50},
        .base_color = {231, 76, 60, 255},
        .hover_color = {192, 57, 43, 255},
        .label = "Scores"
    };
    // Exit
    buttons[5] = (Button){
        .rect = {300, 500, 200, 50},
        .base_color = {100, 0, 0, 255},
        .hover_color = {150, 0, 0, 255},
        .label = "Exit"
    };
}

void render_pause_menu(SDL_Renderer* renderer, Button* buttons, int num_buttons) {
    // Fond semi-transparent
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xAA);
    SDL_Rect overlay = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &overlay);

    // Afficher les boutons de pause
    for(int i = 0; i < num_buttons; i++) {
        Button* btn = &buttons[i];
        SDL_Color color = btn->hovered ? btn->hover_color : btn->base_color;
        SDL_SetRenderDrawColor(renderer, btn->base_color.r, btn->base_color.g, btn->base_color.b, btn->base_color.a);

        SDL_RenderFillRect(renderer, &btn->rect);
        
        SDL_Surface* surface = TTF_RenderText_Blended(hud.font, btn->label, (SDL_Color){0xFF, 0xFF, 0xFF});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect text_rect = {
            btn->rect.x + (btn->rect.w - surface->w)/2,
            btn->rect.y + (btn->rect.h - surface->h)/2,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

// Gestion spécifique au menu principal
void handle_main_menu_click(int x, int y, GameData* game) {
    for (int i = 0; i < 6; i++) {
        if (SDL_PointInRect(&(SDL_Point){x, y}, &main_menu_buttons[i].rect)) {
            switch(i) {
                case 0: // Joueur
                    game->mode = HUMAN;
                    reset_game(game);
                    break;
                case 1: // Machine
                    game->mode = AI;
                    reset_game(game);
                    break;
                case 2: // Joueur vs IA
                    game->mode = HUMAN_VS_AI;
                    reset_game(game);
                    break;
                case 3: // REPRISE
                    load_game_state(game);
                    game->state = PLAYING;
                    break;
                case 4: // Scores
                    game->state = SCORES;
                    break;
                case 5: // Exit
                    game->state = QUIT;
                    break;
            }
        }
    }
}

// Gestion spécifique au menu pause
void handle_pause_menu_click(int x, int y, GameData* game) {
    for (int i = 0; i < 3; i++) {
        if (SDL_PointInRect(&(SDL_Point){x, y}, &pause_menu_buttons[i].rect)) {
            switch(i) {
                case 0: // Reprendre
                    toggle_pause(game);
                    break;
                case 1: // Scores
                    game->prev_state = game->state; // Sauvegarder l'état
                    game->state = SCORES;
                    break;
                case 2: // Menu principal
                    game->state = MENU;
                    init_game(game);
                    break;
            }
        }
    }
}

void render_main_menu(SDL_Renderer* renderer, Button* buttons, int num_buttons) {
    if(!renderer || !buttons || num_buttons <= 0) return;

    // Fond
    SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
    SDL_RenderClear(renderer);

    // Titre
    SDL_Surface* surface = TTF_RenderText_Blended(hud.font, "SLIDEGAME", (SDL_Color){255, 255, 255});
    if(surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect title_rect = {(800 - surface->w)/2, 100, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &title_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Boutons
    for(int i = 0; i < num_buttons; i++) {
        Button* btn = &buttons[i];
        SDL_SetRenderDrawColor(renderer, btn->base_color.r, btn->base_color.g, btn->base_color.b, 255);
        SDL_RenderFillRect(renderer, &btn->rect);
        // Vérification de surface
        SDL_Surface* btn_surface = TTF_RenderText_Blended(hud.font, btn->label, (SDL_Color){255, 255, 255});
        if(!btn_surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, btn_surface);
        
        SDL_Rect text_rect = {
            btn->rect.x + (btn->rect.w - btn_surface->w)/2,
            btn->rect.y + (btn->rect.h - btn_surface->h)/2,
            btn_surface->w,
            btn_surface->h
        };
        
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(btn_surface);
        SDL_DestroyTexture(texture);
    }
}

