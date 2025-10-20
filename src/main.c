#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include "../include/game.h"
#include "../include/graphics.h"
#include "../include/scores.h"
#include "../include/global.h"
#include "../include/menu.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

GameData game;
SDL_Window* window;
SDL_Renderer* renderer;

SDL_Event e;
static SDL_Point touch_start = {0, 0};
static bool dragging = false;

void handle_input(SDL_Event* e) {
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT) {
            game.state = QUIT;
        }

        // Gestion clavier en mode PLAYING
        if (game.state == PLAYING && (game.mode == HUMAN || game.mode == HUMAN_VS_AI)) {
            switch(e->type) {
                case SDL_MOUSEBUTTONDOWN:
                    touch_start.x = e->button.x;
                    touch_start.y = e->button.y;
                    dragging = true;
                    break;

                case SDL_MOUSEMOTION:
                    if(dragging) {
                        SDL_Point current = {e->motion.x, e->motion.y};
                        SwipeDirection dir = detect_swipe(touch_start.x, touch_start.y, current.x, current.y);
                        
                        if(dir != SWIPE_NONE) {
                            switch(dir) {
                                case SWIPE_LEFT:  move_player_tiles(&game, DIR_LEFT);  break;
                                case SWIPE_RIGHT: move_player_tiles(&game, DIR_RIGHT); break;
                                case SWIPE_UP:    move_player_tiles(&game, DIR_UP);   break;
                                case SWIPE_DOWN:  move_player_tiles(&game, DIR_DOWN); break;
                                default: break;
                            }
                            dragging = false; // Réinitialiser après le mouvement
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    dragging = false;
                    break;
            }




            if (e->type == SDL_KEYDOWN) { // Avec clavier
                switch(e->key.keysym.sym) {
                    case SDLK_UP:    move_player_tiles(&game, DIR_UP);    break;
                    case SDLK_DOWN:  move_player_tiles(&game, DIR_DOWN);  break;
                    case SDLK_LEFT:  move_player_tiles(&game, DIR_LEFT);  break;
                    case SDLK_RIGHT: move_player_tiles(&game, DIR_RIGHT); break;
                }
            }
        }else if(game.state == SCORES && SDL_GetTicks() % 1000 < 500) { // Clignotement
            if(SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
                game.state = game.prev_state;
            }
        }

        // Gestion souris
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (game.state == MENU ) {
                handle_main_menu_click(x, y, &game);
            } else if (game.state == PAUSED){
                handle_pause_menu_click(x, y, &game);
            }else if (game.state == PLAYING) {
                // Boutons Pause/Arrêt
                SDL_Rect pause_rect = {300, 550, 150, 40};
                SDL_Rect stop_rect = {470, 550, 150, 40};
                
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pause_rect)) {
                    toggle_pause(&game);
                } else if (SDL_PointInRect(&(SDL_Point){x, y}, &stop_rect)) {
                    save_game_state(&game);
                    game.state = MENU;
                    game.final_time = (SDL_GetTicks() - game.start_time) / 1000; // Enregistrer le temps final
                }
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        printf("Erreur d'initialisation: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("2048 VS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_game(&game);
    init_graphics(renderer);

    while (game.state != QUIT) {
        SDL_Event e;
        handle_input(&e);

        // Logique de l'IA
        if (game.state == PLAYING && (game.mode == AI || game.mode == HUMAN_VS_AI)) {
            SDL_Delay(200);
            ai_move(&game);
        }

        if (game.state == PLAYING) {
            bool game_over = false;
            if (game.mode == HUMAN || game.mode == HUMAN_VS_AI) {
                game_over = is_game_over(game.player_grid);
            }
            if (game.mode == AI || game.mode == HUMAN_VS_AI) {
                game_over = game_over || is_game_over(game.ai_grid);
            }
            if (game_over) {
                game.state = GAMEOVER;
                game.final_time = (SDL_GetTicks() - game.start_time) / 1000;
                handle_game_over(&game); // Passer renderer
            }
        }
        

        // Rendu
        SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
        SDL_RenderClear(renderer);

        switch (game.state) {
            case MENU:
                render_main_menu(renderer, main_menu_buttons, 6);
                break;

            case PLAYING:
                if(game.mode == HUMAN_VS_AI)
                    render_dual_grid(renderer, &game);
                else
                render_single_grid(renderer, 
                          (game.mode == HUMAN) ? game.player_grid : game.ai_grid, 
                          (game.mode == HUMAN));
                render_control_buttons(renderer);
                update_vs_hud(renderer, &hud, &game);
                break;

            case PAUSED:
                switch(game.mode) {
                    case HUMAN:
                        render_single_grid(renderer, game.player_grid, true);
                        break;
                    case AI:
                        render_single_grid(renderer, game.ai_grid, false);
                        break;
                    case HUMAN_VS_AI:
                        render_dual_grid(renderer, &game);
                        break;
                }
                render_pause_menu(renderer, pause_menu_buttons, 3);
                break;
            case GAMEOVER:
                if (game.mode == HUMAN || game.mode == HUMAN_VS_AI) {
                    char score_str[100];
                    sprintf(score_str, "Score: %d | Time: %02d:%02d", 
                        current_score, game_time/60, game_time%60);
                    render_game_over_screen(renderer, &hud, score_str);
                } else {
                    char score_str[100];
                    sprintf(score_str, "AI Score: %d | Time: %02d:%02d", 
                        current_score, game_time/60, game_time%60);
                    render_game_over_screen(renderer, &hud, score_str);
                }
                break;
            case SCORES:
                render_highscores(renderer, hud.font);
                break;
            default:
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
