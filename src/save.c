#include "../include/save.h"
#include <stdio.h>

void save_game_state(const GameData* game) {
    FILE* file = fopen("savedata.bin", "wb");
    if(file) {
        fwrite(game, sizeof(GameData), 1, file);
        fclose(file);
    }
}

void load_game_state(GameData* game) {
    FILE* file = fopen("savedata.bin", "rb");
    if(file) {
        fread(game, sizeof(GameData), 1, file);
        fclose(file);

        // Correction du temps lors de la reprise
        if(game->state == PAUSED) {
            Uint32 current_time = SDL_GetTicks();
            game->start_time += (current_time - game->paused_time);
        }
    } else {
        memset(game, 0, sizeof(GameData));
    }
}
