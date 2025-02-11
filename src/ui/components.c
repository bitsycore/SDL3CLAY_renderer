//
// Created by Doge on 09/02/2025.
//

#include "components.h"

#include <SDL3_image/SDL_image.h>

#include "colors.h"

void SidebarItemComponent(const int i) {
	CLAY({
		.border = {
			.width = CLAY_BORDER_ALL(i),
			.color = COLOR_RED
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
		.backgroundColor = Clay_Hovered() ? COLOR_DARK : COLOR_ORANGE,
		.cornerRadius = Clay_Hovered() ? CLAY_CORNER_RADIUS(16) : CLAY_CORNER_RADIUS(32)
	}) {
		CLAY_TEXT(CLAY_STRING("Sidebar Item"), CLAY_TEXT_CONFIG({.fontSize = i, .textColor = {255, 255, 255, 255}}));
	}
}

typedef struct {
	AppState *app_state;
} ButtonData ;

void HandleButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
	if (!userData) {
		SDL_Log("userData is NULL!\n");
		return;
	}

	const ButtonData *buttonData = (ButtonData*)userData;
	if (!buttonData->app_state) {
		SDL_Log("buttonData->app_state is NULL!\n");
		return;
	}

	if (pointerInfo.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
		SDL_Texture* temp = buttonData->app_state->img_profile;
		buttonData->app_state->img_profile = buttonData->app_state->img_profile2;
		buttonData->app_state->img_profile2 = temp;
	}
}

ButtonData BUTTON_DATA = (ButtonData) {};

void Profile(AppState* APP) {
	const Clay_ElementDeclaration ProfilePictureOuter = {
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

	const Clay_ElementDeclaration ProfilePicture = {
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

	CLAY(ProfilePictureOuter) {
		CLAY(ProfilePicture) {
			BUTTON_DATA.app_state = APP;
			Clay_OnHover(HandleButtonInteraction, (intptr_t) &BUTTON_DATA);
		}
		CLAY_TEXT(
			CLAY_STRING("Clay - UI Library"),
			CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = {255, 255, 255, 255} })
		);
	}
}
