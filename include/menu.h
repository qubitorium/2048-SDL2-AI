#ifndef MENU_H
#define MENU_H

#include "graphics.h"

void init_main_menu(Button* buttons);
void render_pause_menu(SDL_Renderer* renderer, Button* buttons, int num_buttons);
void handle_main_menu_click(int x, int y, GameData* game);
void handle_pause_menu_click(int x, int y, GameData* game);

#endif
