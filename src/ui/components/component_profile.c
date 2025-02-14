#include "component_profile.h"

#include <SDL3_image/SDL_image.h>
#include "../colors.h"

static void onHover(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
	if (!userData) {
		SDL_Log("userData is NULL!\n");
		return;
	}

	if (pointerInfo.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
		SDL_Log("Profile Picture Clicked!\n");
	}
}


void Profile_component(SDL_Texture* IMG1, SDL_Texture* IMG2, AppState* APP) {
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
			.imageData = IMG1,
			.sourceDimensions = {60, 60},
		}
	};

	CLAY(ProfilePictureOuterConfig) {
		CLAY(ProfilePictureConfig) {
			Clay_OnHover(onHover, (intptr_t) APP);
		}
		CLAY_TEXT(
			CLAY_STRING("Clay - UI Library"),
			CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = COLOR_WHITE_NICE })
		);
	}
}