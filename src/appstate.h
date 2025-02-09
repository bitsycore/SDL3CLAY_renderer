//
// Created by Doge on 09/02/2025.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include "../vendor/clay.h"
#include <stdbool.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

typedef struct {
	float renderer_zoom;
	int window_width, window_height;
	bool isMouseDown;
	float mousePositionX, mousePositionY;
	float mouseWheelX, mouseWheelY;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Clay_Arena clay_arena;
	SDL_Texture* img_profile;
	SDL_Texture* img_profile2;
} AppState;

#endif //APPSTATE_H
