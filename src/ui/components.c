//
// Created by Doge on 09/02/2025.
//

#include "components.h"

#include <SDL3_image/SDL_image.h>

#include "colors.h"

static Clay_ElementDeclaration SidebarItemConfig() {
	return (Clay_ElementDeclaration){
		.border = {
			.width = CLAY_BORDER_ALL(4),
			.color = COLOR_BLACK_NICE
		},
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIT(50)
			},
			.padding = CLAY_PADDING_ALL(24),
			.childAlignment = {
				.x = CLAY_ALIGN_X_CENTER,
				.y = CLAY_ALIGN_Y_CENTER
			},
			.childGap = 16
		},
		.backgroundColor = Clay_Hovered() ? COLOR_BLACK_NICE : COLOR_DARK_BLUE,
		.cornerRadius = CLAY_CORNER_RADIUS(16)
	};
}

static void SidebarItemCallback (Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		Clay_SetDebugModeEnabled(true);
	}
}

void SidebarItemComponent(const int i) {
	CLAY(SidebarItemConfig()) {
		Clay_OnHover(SidebarItemCallback, (intptr_t)NULL);
		CLAY_TEXT(
			CLAY_STRING("Sidebar Item"),
			CLAY_TEXT_CONFIG(
				{
				.fontSize = i,
				.textColor = Clay_Hovered() ? COLOR_LIGHT : COLOR_WHITE_NICE
				}
			)
		);
	}
}

// ===================================================================================
//
// MARK: Profile
//
// ===================================================================================

static void ProfileCallback(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
	if (!userData) {
		SDL_Log("userData is NULL!\n");
		return;
	}

	AppState* APP = (AppState*) userData;

	if (pointerInfo.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
		SDL_Texture* temp = APP->img_profile;
		APP->img_profile = APP->img_profile2;
		APP->img_profile2 = temp;
	}
}


void ProfileComponent(AppState* APP) {
	const Clay_ElementDeclaration ProfilePictureOuterConfig = {
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_GROW(0)
			},
			.padding = CLAY_PADDING_ALL(16),
			.childGap = 16,
			.childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
		},
		.backgroundColor = COLOR_RED,
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	const Clay_ElementDeclaration ProfilePictureConfig = {
		.id = CLAY_ID("ProfilePicture"),
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_FIXED(60),
				.height = CLAY_SIZING_FIXED(60)
			}
		},
		.image = {
			.imageData = APP->img_profile,
			.sourceDimensions = {60, 60},
		}
	};

	CLAY(ProfilePictureOuterConfig) {
		CLAY(ProfilePictureConfig) {
			Clay_OnHover(ProfileCallback, (intptr_t) APP);
		}
		CLAY_TEXT(
			CLAY_STRING("Clay - UI Library"),
			CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = COLOR_WHITE_NICE })
		);
	}
}

// ===================================================================================
//
// MARK: ButtonDebug
//
// ===================================================================================

static Clay_ElementDeclaration ButtonDebugConfig() {
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
		 .backgroundColor = Clay_Hovered() ? AlphaOver(COLOR_RED, 1.0f) : AlphaOver(COLOR_RED, 0.5f),
	};
}

static void ButtonDebugCallback (Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		Clay_SetDebugModeEnabled(true);
	}
}

void ButtonDebugComponent() {
	if (!Clay_IsDebugModeEnabled()) {
		CLAY(ButtonDebugConfig()) {
			CLAY_TEXT(CLAY_STRING("DEBUG"), CLAY_TEXT_CONFIG({.fontSize = 15, .textColor = {255, 255, 255, 255}}));
			Clay_OnHover(ButtonDebugCallback, 0);
		}
	}
}