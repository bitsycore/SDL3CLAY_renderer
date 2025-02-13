//
// Created by Doge on 12/02/2025.
//

#include "screen_manager.h"

#include "../appstate.h"
#include "components.h"

Screen CURRENT_SCREEN = {0}, NEXT_SCREEN = {0};
bool NEXT_SCREEN_READY = false;

void SetNextScreen(const Screen screen) {
	NEXT_SCREEN = screen;
	NEXT_SCREEN_READY = true;
}

Screen* GetCurrentScreen() {
	return &CURRENT_SCREEN;
}

void InitScreen(AppState* APP) {
	if (!CURRENT_SCREEN.init_done) {
		if (CURRENT_SCREEN.on_init) {
			void *ptr = NULL;
			CURRENT_SCREEN.on_init(APP, &ptr);
			CURRENT_SCREEN.state = ptr;
		}

		CURRENT_SCREEN.init_done = true;
	}
}

void UpdateScreen(AppState *APP) {
	if (CURRENT_SCREEN.on_update) {
		CURRENT_SCREEN.on_update(APP, CURRENT_SCREEN.state);
	}
}

void DestroyScreen(AppState *APP, const bool force) {
	if (NEXT_SCREEN_READY || force) {
		if (CURRENT_SCREEN.on_destroy) {
			CURRENT_SCREEN.on_destroy(APP, CURRENT_SCREEN.state);
		}
		CURRENT_SCREEN = NEXT_SCREEN;
		NEXT_SCREEN_READY = false;
	}
}