#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef enum {
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER,
    QUIT,
    SCORES
} GameState;

typedef enum {
    HUMAN,
    AI,
    HUMAN_VS_AI
} GameMode;

typedef enum {
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    SWIPE_NONE,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN
} SwipeDirection;

SwipeDirection detect_swipe(int x1, int y1, int x2, int y2);

typedef struct {
    int value;
    bool merged;
} Tile;

typedef struct {
    Tile player_grid[4][4];  // Grille du joueur (droite)
    Tile ai_grid[4][4];      // Grille de l'IA (gauche)
    int player_score;
    int ai_score;
    Tile grid[4][4];
    int score;
    int best_score;
    Uint32 final_time;
    Uint32 start_time;
    Uint32 paused_time;
    GameState state;
    GameState prev_state;
    GameMode mode;
} GameData;

void init_game(GameData* game);
void reset_game(GameData* game);
bool move_tiles(GameData* game, Direction dir);
void add_random_tile(Tile grid[4][4]);
bool is_game_over(const Tile grid[4][4]);
void toggle_pause(GameData* game);
void save_game_state(const GameData* game);
void load_game_state(GameData* game);
bool is_grid_full(const Tile grid[4][4]);
bool move_player_tiles(GameData* game, Direction dir); 
#define GRID_SIZE 4 // Si pas déjà défini

// Déclarations des nouvelles fonctions
bool slide_left(Tile grid[GRID_SIZE][GRID_SIZE], int* score);
bool slide_right(Tile grid[GRID_SIZE][GRID_SIZE], int* score);
bool slide_up(Tile grid[GRID_SIZE][GRID_SIZE], int* score);
bool slide_down(Tile grid[GRID_SIZE][GRID_SIZE], int* score);
void ai_move(GameData* game);
void ai_make_move(Tile grid[GRID_SIZE][GRID_SIZE], int* score);
void render_single_grid(SDL_Renderer* renderer, const Tile grid[4][4], bool is_player);


void handle_game_over(GameData* game);

#endif
