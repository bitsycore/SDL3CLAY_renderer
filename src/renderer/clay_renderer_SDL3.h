#ifndef CLAY_RENDERER_SDL3_H
#define CLAY_RENDERER_SDL3_H

#include "../../vendor/clay.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void SDL_Clay_RenderClayCommands(SDL_Renderer *renderer, Clay_RenderCommandArray *rcommands);
void Clay_SDL_SetFont(const char * fontPath);
TTF_Font* Clay_SDL_GetFont();
Clay_Dimensions SDL_Clay_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);

#endif //CLAY_RENDERER_SDL3_H