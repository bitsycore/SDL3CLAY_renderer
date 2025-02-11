#include "appstate.h"

#include <SDL3/SDL.h>

#include "common/memory_leak.h"

AppState* AppState_new() {
	AppState* APP = ml_calloc(1, sizeof(AppState));

	APP->renderer_zoom = 7.0f;
	APP->scroll_speed = 3.1f;
	APP->window_height = 720;
	APP->window_width = 1280;
	APP->delta_last_time = SDL_GetTicks();

    return APP;
}

void AppState_destroy(AppState** app_state) {
	if (!app_state || !*app_state) {
		return;
	}

	ml_free(*app_state);
	*app_state = NULL;
}