#ifndef GLOBAL_H
#define GLOBAL_H

#include "graphics.h"

// Déclaration des boutons du menu principal et du menu pause
extern Button main_menu_buttons[6];
extern Button pause_menu_buttons[3];

// Déclaration de l'HUD global
extern HUD hud;
extern SDL_Renderer* renderer;
extern int current_score;
extern int best_score;
extern Uint32 game_time;

#endif
