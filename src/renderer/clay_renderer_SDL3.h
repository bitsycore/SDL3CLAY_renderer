#ifndef CLAY_RENDERER_SDL3_H
#define CLAY_RENDERER_SDL3_H

#include "../../vendor/clay.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void SDLCLAY_Render(SDL_Renderer *renderer, Clay_RenderCommandArray *commands_array);
void SDLCLAY_SetFont(const char * fontPath);
TTF_Font* SDLCLAY_GetFont();
Clay_Dimensions SDLCLAY_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);

#endif //CLAY_RENDERER_SDL3_H