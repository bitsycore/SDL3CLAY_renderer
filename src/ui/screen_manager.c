#include "screen_manager.h"

#include "../appstate.h"
#include <SDL3/SDL.h>

Screen CURRENT_SCREEN = {0}, NEXT_SCREEN = {0};
bool NEXT_SCREEN_READY = false;

void ScreenManager_setNextScreen(const Screen screen) {
	NEXT_SCREEN = screen;
	NEXT_SCREEN_READY = true;
}

Screen* ScreenManager_getCurrentScreen() {
	return &CURRENT_SCREEN;
}

void ScreenManager_runScreenInit(AppState* APP) {
	if (!CURRENT_SCREEN.init_done) {
		if (CURRENT_SCREEN.on_init) {
			CURRENT_SCREEN.state = CURRENT_SCREEN.on_init(APP);
		}
		CURRENT_SCREEN.init_done = true;
	}
}

void ScreenManager_runScreenUpdate(AppState* APP) {
	if (CURRENT_SCREEN.on_update) {
		CURRENT_SCREEN.on_update(APP, CURRENT_SCREEN.state);
	}
}

void ScreenManager_runScreenDestroy(AppState* APP) {
	if (!CURRENT_SCREEN.destroy_done && NEXT_SCREEN_READY ) {
		if (CURRENT_SCREEN.on_destroy) {
			CURRENT_SCREEN.on_destroy(APP, CURRENT_SCREEN.state);
		}
		CURRENT_SCREEN = NEXT_SCREEN;
		NEXT_SCREEN_READY = false;
	}
}

void ScreenManager_end(AppState* APP) {
	if (!CURRENT_SCREEN.destroy_done) {
		if (CURRENT_SCREEN.on_destroy) {
			CURRENT_SCREEN.on_destroy(APP, CURRENT_SCREEN.state);
		}
		CURRENT_SCREEN = NEXT_SCREEN;
		NEXT_SCREEN_READY = false;
	}
}

bool ScreenManager_isScreenReadyToUpdate() {
	if (CURRENT_SCREEN.update_rate_ms <= 0) {
		return true;
	}

	const uint64_t current_time = SDL_GetTicks();
	const uint64_t elapsed_time = current_time - CURRENT_SCREEN.last_update_time;

	if ((float)elapsed_time >= CURRENT_SCREEN.update_rate_ms) {
		CURRENT_SCREEN.last_update_time = current_time;
		return true;
	}

	return false;
}
