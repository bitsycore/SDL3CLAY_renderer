#include "component_debug_button.h"

#include <SDL3_image/SDL_image.h>
#include "../colors.h"

static inline Clay_ElementDeclaration config() {
	return (Clay_ElementDeclaration){
		.id = CLAY_ID("Tooltip"),
		.floating = {
			.attachPoints = {
				.element = CLAY_ATTACH_POINT_RIGHT_TOP,
				.parent = CLAY_ATTACH_POINT_RIGHT_TOP
			},
			.attachTo = CLAY_ATTACH_TO_ROOT
		},
		.border = {
			.width = CLAY_BORDER_OUTSIDE(2),
			.color = COLOR_BLACK_NICE
		},
		.layout = {
			.padding = CLAY_PADDING_ALL(8)
		},
		.backgroundColor = Clay_Hovered() ? Color_alphaOver(COLOR_RED, 1.0f) : Color_alphaOver(COLOR_RED, 0.5f),
	};
}

static void onHover(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		Clay_SetDebugModeEnabled(true);
	}
}

void DebugButton_component() {
	if (!Clay_IsDebugModeEnabled()) {
		CLAY(config()) {
			CLAY_TEXT(CLAY_STRING("DEBUG"), CLAY_TEXT_CONFIG({.fontSize = 15, .textColor = {255, 255, 255, 255}}));
			Clay_OnHover(onHover, 0);
		}
	}
}
