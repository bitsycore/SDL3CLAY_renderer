#ifndef APPSTATE_H
#define APPSTATE_H

#include <stdbool.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "common/arena.h"

typedef struct {

	// Frame State
	Uint64 delta_last_time;
	float delta;

	// Input State
	bool isMouseDown;
	float mousePositionX, mousePositionY;
	float mouseWheelX, mouseWheelY;

	// SDL State
	SDL_Window* window;
	SDL_Renderer* renderer;

	// Window/Renderer State
	float renderer_zoom;
	int window_width, window_height;

	// Clay State
	void* clay_memory;

} AppState;

AppState* AppState_new();
void AppState_destroy(AppState** app_state);

#endif //APPSTATE_H
