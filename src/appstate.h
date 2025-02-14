#ifndef APPSTATE_H
#define APPSTATE_H

#include <stdbool.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "common/arena.h"

typedef struct {
	float renderer_zoom;
	int window_width, window_height;
	bool isMouseDown;
	float mousePositionX, mousePositionY;
	float mouseWheelX, mouseWheelY;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* img_profile;
	SDL_Texture* img_profile2;
	void* clay_memory;
	Uint64 delta_last_time;
	float delta;
	float scroll_speed;
	SDL_Texture * img_bg;
} AppState;

AppState* AppState_new();
void AppState_destroy(AppState** app_state);

#endif //APPSTATE_H
