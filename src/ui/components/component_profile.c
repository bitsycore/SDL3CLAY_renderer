#include "component_profile.h"

#include <SDL3_image/SDL_image.h>
#include "../colors.h"

struct HoverEvent {
	SDL_Texture** img1;
	SDL_Texture** img2;
};

static void onHover(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
	if (pointerInfo.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
		if (userData) {
			const struct HoverEvent * event = (struct HoverEvent *)userData;
			SDL_Texture* temp = *event->img1;
			*event->img1 = *event->img2;
			*event->img2 = temp;
		}
	}
}

void Profile_component(SDL_Texture** IMG1, SDL_Texture** IMG2, Arena* FRAME_ARENA) {
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
			.imageData = *IMG1,
			.sourceDimensions = {60, 60},
		}
	};

	CLAY(ProfilePictureOuterConfig) {
		CLAY(ProfilePictureConfig) {
			if (FRAME_ARENA) {
				struct HoverEvent * event =  Arena_alloc(FRAME_ARENA, sizeof(struct HoverEvent));
				event->img1 = IMG1;
				event->img2 = IMG2;
				Clay_OnHover(onHover, (intptr_t)event);
			}
			else {
				Clay_OnHover(onHover, (intptr_t) NULL);
			}

		}
		CLAY_TEXT(
			CLAY_STRING("Clay - UI Library"),
			CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = COLOR_WHITE_NICE })
		);
	}
}