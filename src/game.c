#include "../include/game.h"
#include <SDL2/SDL_events.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../include/scores.h"
#include "../include/menu.h"
#include "../include/global.h"

static const int SPAWN_PROBABILITY[2] = {90, 10};

void init_game(GameData* game) {
    srand(time(NULL));
    memset(game, 0, sizeof(GameData));
    game->state = MENU;
    game->prev_state = MENU; // Garder seulement l'état initial
}

void reset_game(GameData* game) {
    memset(game->player_grid, 0, sizeof(game->player_grid));
    memset(game->ai_grid, 0, sizeof(game->ai_grid));

    game->player_score = 0;
    game->ai_score = 0;
    game->start_time = SDL_GetTicks();
    // Réinitialisation conditionnelle selon le mode
    if(game->mode == HUMAN) {
        add_random_tile(game->player_grid);
        add_random_tile(game->player_grid);
    } 
    else if(game->mode == AI) {
        add_random_tile(game->ai_grid);
        add_random_tile(game->ai_grid);
    }
    else if(game->mode == HUMAN_VS_AI) {
        add_random_tile(game->player_grid);
        add_random_tile(game->ai_grid);
    }

    game->player_score = 0;
    game->ai_score = 0;
    game->start_time = SDL_GetTicks(); // Timer démarre ici
    game->state = PLAYING;
}

bool move_player_tiles(GameData* game, Direction dir) {
    bool moved = false;
    switch(dir) {
        case DIR_UP:    moved = slide_up(game->player_grid, &game->player_score);    break;
        case DIR_DOWN:  moved = slide_down(game->player_grid, &game->player_score);  break;
        case DIR_LEFT:  moved = slide_left(game->player_grid, &game->player_score);  break;
        case DIR_RIGHT: moved = slide_right(game->player_grid, &game->player_score); break;
        default: return false;
    }
    if(moved)
        add_random_tile(game->player_grid);
    return moved;
}


void ai_move(GameData* game) {
    Tile (*target_grid)[GRID_SIZE];
    int* target_score;

    if(game->mode == AI) {
        target_grid = game->ai_grid; // Utiliser la grille du joueur en mode solo AI
        target_score = &game->ai_score;
    } 
    else if(game->mode == HUMAN_VS_AI) {
        target_grid = game->ai_grid; // Utiliser la grille de l'IA en mode versus
        target_score = &game->ai_score;
    }
    else {
        return;
    }

    ai_make_move(target_grid, target_score);
    add_random_tile(target_grid);
}

void add_random_tile(Tile grid[4][4]) {
    int empty_cells[16][2];
    int count = 0;

    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(grid[y][x].value == 0) {
                empty_cells[count][0] = x;
                empty_cells[count][1] = y;
                count++;
            }
        }
    }

    if(count > 0) {
        int index = rand() % count;
        int x = empty_cells[index][0];
        int y = empty_cells[index][1];
        int r = rand() % 100;
        grid[y][x].value = (r < SPAWN_PROBABILITY[0]) ? 2 : 4;
    }
}

bool is_game_over(const Tile grid[4][4]) {
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(grid[y][x].value == 0) return false;
            if(x < 3 && grid[y][x].value == grid[y][x+1].value) return false;
            if(y < 3 && grid[y][x].value == grid[y+1][x].value) return false;
        }
    }
    return true;
}

void toggle_pause(GameData* game) {
    if(game->state == PLAYING) {
        game->paused_time = SDL_GetTicks();
        game->state = PAUSED;
    } else if(game->state == PAUSED) {
        game->start_time += (SDL_GetTicks() - game->paused_time);
        game->state = PLAYING;
    }
}


// Nouvelle logique adaptée pour la structure Tile
bool slide_left(Tile grid[GRID_SIZE][GRID_SIZE], int* score) {
    bool moved = false;
    for (int i = 0; i < GRID_SIZE; ++i) {
        int last_merged = -1;
        for (int j = 1; j < GRID_SIZE; ++j) {
            if (grid[i][j].value == 0) continue; // Accès à la valeur
            
            int target = j;
            while (target > 0 && grid[i][target - 1].value == 0) {
                target--;
            }
            
            if (target > 0 && 
                grid[i][target - 1].value == grid[i][j].value && 
                !grid[i][target - 1].merged && // Vérifier le flag merged
                last_merged != target - 1) 
            {
                grid[i][target - 1].value *= 2;
                grid[i][target - 1].merged = true; // Marquer comme fusionné
                *score += grid[i][target - 1].value;
                grid[i][j].value = 0;
                moved = true;
                last_merged = target - 1;
            } 
            else if (target != j) {
                grid[i][target].value = grid[i][j].value;
                grid[i][j].value = 0;
                moved = true;
            }
        }
    }
    
    // Réinitialiser les flags merged après le mouvement
    for(int y = 0; y < GRID_SIZE; y++) {
        for(int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].merged = false;
        }
    }
    return moved;
}
bool slide_right(Tile grid[GRID_SIZE][GRID_SIZE], int* score) {
    bool moved = false;
    for (int i = 0; i < GRID_SIZE; ++i) {
        int last_merged = GRID_SIZE; // Initialiser hors de la plage
        for (int j = GRID_SIZE - 2; j >= 0; --j) {
            if (grid[i][j].value == 0) continue;

            int target = j;
            while (target < GRID_SIZE - 1 && grid[i][target + 1].value == 0) {
                target++;
            }

            if (target < GRID_SIZE - 1 && 
                grid[i][target + 1].value == grid[i][j].value && 
                !grid[i][target + 1].merged && 
                last_merged != target + 1) 
            {
                grid[i][target + 1].value *= 2;
                grid[i][target + 1].merged = true;
                *score += grid[i][target + 1].value;
                grid[i][j].value = 0;
                moved = true;
                last_merged = target + 1;
            } 
            else if (target != j) {
                grid[i][target].value = grid[i][j].value;
                grid[i][j].value = 0;
                moved = true;
            }
        }
    }

    // Réinitialiser les flags merged
    for(int y = 0; y < GRID_SIZE; y++) {
        for(int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].merged = false;
        }
    }
    return moved;
}
// Adapter slide_right(), slide_up(), slide_down() de la même manière
bool slide_up(Tile grid[GRID_SIZE][GRID_SIZE], int* score) {
    bool moved = false;
    for (int j = 0; j < GRID_SIZE; ++j) {
        int last_merged = -1;
        for (int i = 1; i < GRID_SIZE; ++i) {
            if (grid[i][j].value == 0) continue;

            int target = i;
            while (target > 0 && grid[target - 1][j].value == 0) {
                target--;
            }

            if (target > 0 && 
                grid[target - 1][j].value == grid[i][j].value && 
                !grid[target - 1][j].merged && 
                last_merged != target - 1) 
            {
                grid[target - 1][j].value *= 2;
                grid[target - 1][j].merged = true;
                *score += grid[target - 1][j].value;
                grid[i][j].value = 0;
                moved = true;
                last_merged = target - 1;
            } 
            else if (target != i) {
                grid[target][j].value = grid[i][j].value;
                grid[i][j].value = 0;
                moved = true;
            }
        }
    }

    // Réinitialiser les flags merged
    for(int y = 0; y < GRID_SIZE; y++) {
        for(int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].merged = false;
        }
    }
    return moved;
}

bool slide_down(Tile grid[GRID_SIZE][GRID_SIZE], int* score) {
    bool moved = false;
    for (int j = 0; j < GRID_SIZE; ++j) {
        int last_merged = GRID_SIZE;
        for (int i = GRID_SIZE - 2; i >= 0; --i) {
            if (grid[i][j].value == 0) continue;

            int target = i;
            while (target < GRID_SIZE - 1 && grid[target + 1][j].value == 0) {
                target++;
            }

            if (target < GRID_SIZE - 1 && 
                grid[target + 1][j].value == grid[i][j].value && 
                !grid[target + 1][j].merged && 
                last_merged != target + 1) 
            {
                grid[target + 1][j].value *= 2;
                grid[target + 1][j].merged = true;
                *score += grid[target + 1][j].value;
                grid[i][j].value = 0;
                moved = true;
                last_merged = target + 1;
            } 
            else if (target != i) {
                grid[target][j].value = grid[i][j].value;
                grid[i][j].value = 0;
                moved = true;
            }
        }
    }

    // Réinitialiser les flags merged
    for(int y = 0; y < GRID_SIZE; y++) {
        for(int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].merged = false;
        }
    }
    return moved;
}

SwipeDirection detect_swipe(int x1, int y1, int x2, int y2) {
    const int THRESHOLD = 30; // Seuil minimal de déplacement en pixels
    int dx = x2 - x1;
    int dy = y2 - y1;

    if(abs(dx) < THRESHOLD && abs(dy) < THRESHOLD) return SWIPE_NONE;

    if(abs(dx) > abs(dy)) {
        return (dx > 0) ? SWIPE_RIGHT : SWIPE_LEFT;
    } else {
        return (dy > 0) ? SWIPE_DOWN : SWIPE_UP;
    }
}

int evaluate_grid(Tile grid[GRID_SIZE][GRID_SIZE]) {
    int empty_count = 0;
    int merge_potential = 0;

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j].value == 0) { // Accès à la valeur
                empty_count++;
            }
            // Vérifications des voisins
            if (i < GRID_SIZE - 1 && grid[i][j].value == grid[i + 1][j].value) {
                merge_potential += grid[i][j].value;
            }
            if (j < GRID_SIZE - 1 && grid[i][j].value == grid[i][j + 1].value) {
                merge_potential += grid[i][j].value;
            }
        }
    }
    return empty_count * 10 + merge_potential;
}


void ai_make_move(Tile grid[GRID_SIZE][GRID_SIZE], int* score) {
    int best_score = -1;
    Direction best_dir = DIR_LEFT;

    // Sauvegarder l'état original
    Tile original_grid[GRID_SIZE][GRID_SIZE];
    memcpy(original_grid, grid, sizeof(Tile) * GRID_SIZE * GRID_SIZE);
    int original_score = *score;

    // Tester les 4 directions
    Direction directions[] = {DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN};
    for(int i = 0; i < 4; i++) {
        // Copie de travail pour la simulation
        Tile test_grid[GRID_SIZE][GRID_SIZE];
        memcpy(test_grid, original_grid, sizeof(Tile) * GRID_SIZE * GRID_SIZE);
        int temp_score = original_score;

        bool moved = false;
        switch(directions[i]) {
            case DIR_LEFT:  moved = slide_left(test_grid, &temp_score);  break;
            case DIR_RIGHT: moved = slide_right(test_grid, &temp_score); break;
            case DIR_UP:    moved = slide_up(test_grid, &temp_score);    break;
            case DIR_DOWN:  moved = slide_down(test_grid, &temp_score);  break;
            default: break;
        }

        if(moved) {
            int current_eval = evaluate_grid(test_grid);
            if(current_eval > best_score) {
                best_score = current_eval;
                best_dir = directions[i];
            }
        }
    }

    // Exécuter le meilleur mouvement
    switch(best_dir) {
        case DIR_LEFT:  slide_left(grid, score);  break;
        case DIR_RIGHT: slide_right(grid, score); break;
        case DIR_UP:    slide_up(grid, score);    break;
        case DIR_DOWN:  slide_down(grid, score);  break;
        default: break;
    }
}

void get_player_name(SDL_Renderer* renderer, HUD* hud, char* name) {
    SDL_StartTextInput();
    SDL_Event e;
    bool done = false;
    name[0] = '\0';

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
                game.state = QUIT;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) done = true;
                else if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    int len = strlen(name);
                    if (len > 0) name[len - 1] = '\0';
                }
            } else if (e.type == SDL_TEXTINPUT) {
                if (strlen(name) < 49) strcat(name, e.text.text);
            }
        }

        // Rendu
        SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
        SDL_RenderClear(renderer);

        SDL_Color white = {255, 255, 255};
        SDL_Color red = {255,0, 0};
        
        // "Game Over!"
        SDL_Surface* surface = TTF_RenderText_Blended(hud->font, "GAME OVER!", red);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {(800 - surface->w)/2, 100, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }


        // "Entrez votre nom:"
        surface = TTF_RenderText_Blended(hud->font, "Entrez votre nom:", white);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {(800 - surface->w)/2, 200, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }

        // Nom saisi
        if (strlen(name) > 0) {
            surface = TTF_RenderText_Blended(hud->font, name, white);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {(800 - surface->w)/2, 250, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }
        }

        
        // Score et temps
        char score_str[100];
        sprintf(score_str, "Score: %d       Temps: %02d:%02d", 
               current_score, game_time/60, game_time%60);
        surface = TTF_RenderText_Blended(hud->font, score_str, white);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {(800 - surface->w)/2, 450, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }

        SDL_RenderPresent(renderer);
    }
    SDL_StopTextInput();
}


void display_ai_game_over(SDL_Renderer* renderer, HUD* hud, int score, Uint32 time) {
    char score_str[100];
    sprintf(score_str, "Score IA: %d   Temps: %02d:%02d", score, time/60, time%60);

    SDL_SetRenderDrawColor(renderer, 0x1A, 0x1A, 0x1A, 0xFF);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255};
    
    // "Game Over!"
    SDL_Surface* surface = TTF_RenderText_Blended(hud->font, "Game Over!", white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {(800 - surface->w)/2, 100, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Score et temps
    surface = TTF_RenderText_Blended(hud->font, score_str, white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {(800 - surface->w)/2, 200, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(3000); // Affichage pendant 3 secondes
}

void handle_game_over(GameData* game) {
    if (game->state == GAMEOVER) {
        if (game->mode == HUMAN || game->mode == HUMAN_VS_AI) {
            current_score = game->player_score;
            game_time = (SDL_GetTicks() - game->start_time) / 1000;
            
            char player_name[50] = {0};
            get_player_name(renderer, &hud, player_name);

            Highscore highscores[MAX_HIGHSCORES];
            load_highscores(highscores);

            if (current_score > 0) {
                add_highscore(highscores, player_name, current_score, game_time);
                save_highscores(highscores);
            }
        } 
        else if (game->mode == AI) {
            // Nouveau cas pour l'IA
            current_score = game->ai_score;
            game_time = (SDL_GetTicks() - game->start_time) / 1000;
            display_ai_game_over(renderer, &hud, current_score, game_time);
        }
        
        game->state = MENU;
    }
}
