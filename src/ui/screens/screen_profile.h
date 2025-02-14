#ifndef SCREEN1_H
#define SCREEN1_H

#include "../screen_manager.h"

typedef struct Data {
    Arena *arena;
    SDL_Texture* img_profile1;
    SDL_Texture* img_profile2;
} Data;

Screen ScreenProfile_new();

#endif //SCREEN1_H