#ifndef CLAY_RENDERER_SDL3_H
#define CLAY_RENDERER_SDL3_H

#include "../../vendor/clay.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// ===================================================================================
// MARK: Core
// ===================================================================================

typedef void (*SDLCLAY_Fun_Logger)(const char* message, ...);
typedef void* (*SDLCLAY_Fun_Malloc)(size_t);
typedef void (*SDLCLAY_Fun_Free)(void*);

/**
 * Set your preferred logger function, default to SDL_Log
 * @param logger Logger callback to use
 */
void SDLCLAY_SetLogger(SDLCLAY_Fun_Logger logger);

/**
 * Set your preferred memory allocator, default to SDL_malloc and SDL_free
 * @param fun_malloc Malloc function to use
 * @param fun_free Free function to use
 */
void SDLCLAY_SetAllocator(SDLCLAY_Fun_Malloc fun_malloc, SDLCLAY_Fun_Free fun_free);

/**
 * Free all resources used by SDLCLAY
 */
void SDLCLAY_Quit();

// ===================================================================================
// MARK: Fonts
// ===================================================================================

// Base capacity of a node of the holder before requiring
// an extra node in the chained list
#define SDLCLAY_FONT_HOLDER_CAPACITY 32
#define SDLCLAY_FONT_MAX_SIZE 256

/**
 * Add font to the SDLCLAY font holder, it will preload the initial size
 * and will lazy load any other requested size. It will load it with TTF_OpenFont
 *
 * You can preload them by just requesting them with SDLCLAY_GetFont
 *
 * @param font_path Font path to load
 * @param init_size Initial size to preload
 * @return Font index to get it back with SDLCLAY_GetFont or to use in Clay elements
 */
int SDLCLAY_AddFont(const char * font_path, int init_size);

/**
 * Add font to the SDLCLAY font holder, it will preload the initial size
 * and will lazy load any other requested size.
 *
 * You can preload them by just requesting them with SDLCLAY_GetFont
 *
 * @param font Font to load
 * @param init_size Initial size to preload
 * @return Font index to get it back with SDLCLAY_GetFont or to use in Clay elements
 */
int SDLCLAY_AddFontRaw(TTF_Font * font, int init_size);

/**
 * Get the font from the SDLCLAY font holder.
 *
 * @param font_index Index of the font in the font holder.
 * @param size Size of the font to retrieve.
 * @return Pointer to the TTF_Font structure.
 */
TTF_Font* SDLCLAY_GetFont(int font_index, int size);

/**
 * Measure Function to Bind to Clay
 */
Clay_Dimensions SDLCLAY_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);

// ===================================================================================
// MARK: Render
// ===================================================================================

#define SDLCLAY_NUM_SEGMENT_CORNER 32

/**
 * Render Clay commands using SDL3 renderer.
 *
 * @param renderer The SDL_Renderer to use for rendering.
 * @param commands_array The array of render commands to execute.
 */
void SDLCLAY_RenderCommands(SDL_Renderer *renderer, Clay_RenderCommandArray *commands_array);

#endif //CLAY_RENDERER_SDL3_H