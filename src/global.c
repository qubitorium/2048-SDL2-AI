#include "../include/global.h"
#include "../include/game.h"

// Définition des boutons du menu principal
Button main_menu_buttons[6] = {
    // Bouton Joueur
  {
        .rect = {300, 200, 200, 50},
        .base_color = {46, 204, 113, 255},
        .hover_color = {39, 174, 96, 255},
        .label = "Joueur"
    },
    {
    // Bouton Machine
        .rect = {300, 260, 200, 50},
        .base_color = {52, 152, 219, 255},
        .hover_color = {41, 128, 185, 255},
        .label = "Machine"
    },
    {

    // Bouton Joueur vs Machine
        .rect = {300, 320, 200, 50},
        .base_color = {155, 89, 182, 255},
        .hover_color = {142, 68, 173, 255},
        .label = "Joueur vs MC"
    },
    {

    // Bouton Reprendre
        .rect = {300, 380, 200, 50},
        .base_color = {241, 196, 15, 255},
        .hover_color = {243, 156, 18, 255},
        .label = "Reprise"
    },
    {

    // Bouton Scores
        .rect = {300, 440, 200, 50},
        .base_color = {231, 76, 60, 255},
        .hover_color = {192, 57, 43, 255},
        .label = "Scores"
    },
      {
        .rect = {300, 500, 200, 50}, // Positionné sous "Scores"
        .base_color = {100, 0, 0, 255}, // Rouge foncé
        .hover_color = {150, 0, 0, 255},
        .label = "Exit"
    }

};
// Définition des boutons du menu pause
Button pause_menu_buttons[3] = {
    { .rect = {300, 200, 200, 50}, 
      .base_color = {46, 204, 113, 255}, 
      .hover_color = {39, 174, 96, 255}, 
      .label = "Reprendre" },
    
    { .rect = {300, 260, 200, 50}, 
      .base_color = {52, 152, 219, 255}, 
      .hover_color = {41, 128, 185, 255}, 
      .label = "Scores" },
    
    { .rect = {300, 320, 200, 50}, 
      .base_color = {231, 76, 60, 255}, 
      .hover_color = {192, 57, 43, 255}, 
      .label = "Menu Principal" }
};

// Définition de l'HUD global
HUD hud = {0};

int current_score = 0;
int best_score = 0;
Uint32 game_time = 0;
