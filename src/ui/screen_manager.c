//
// Created by Doge on 12/02/2025.
//

#include "screen_manager.h"

#include "../appstate.h"
#include "colors.h"
#include "components.h"

static ScreenFun CURRENT_SCREEN = NULL;

void SetCurrentScreen(const ScreenFun screen) {
	CURRENT_SCREEN = screen;
}

ScreenFun GetCurrentScreen() {
	return CURRENT_SCREEN;
}

Clay_RenderCommandArray ProcessCurrentScreen(AppState * APP) {
	// ========================================
	// Clay Update States
	Clay_SetLayoutDimensions((Clay_Dimensions){(float) APP->window_width, (float) APP->window_height});
	Clay_SetPointerState((Clay_Vector2){APP->mousePositionX, APP->mousePositionY}, APP->isMouseDown);
	Clay_UpdateScrollContainers(true, (Clay_Vector2){ APP->mouseWheelX * APP->scroll_speed, APP->mouseWheelY * APP->scroll_speed }, APP->delta);

	Clay_BeginLayout();

	ButtonDebugComponent();

	if (CURRENT_SCREEN) {
		CURRENT_SCREEN(APP);
	}

	return Clay_EndLayout();
}
