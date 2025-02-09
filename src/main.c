#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#define CLAY_IMPLEMENTATION
#include <SDL3_image/SDL_image.h>

#include "../vendor/clay.h"

#include "renderer/clay_renderer_SDL3.h"
#include "ui/colors.h"
#include "ui/components.h"

#include "appstate.h"

void HandleClayErrors(Clay_ErrorData errorData) {
	SDL_Log("%s", errorData.errorText.chars);
	switch (errorData.errorType) {
		default:
	}
}

Clay_RenderCommandArray UiProcess(AppState * APP) {
	// ========================================
	// Clay Update States
	Clay_SetLayoutDimensions((Clay_Dimensions){(float) APP->window_width, (float) APP->window_height});
	Clay_SetPointerState((Clay_Vector2){APP->mousePositionX, APP->mousePositionY}, APP->isMouseDown);
	Clay_UpdateScrollContainers(true, (Clay_Vector2){APP->mouseWheelX, APP->mouseWheelY}, 0.6);

	// ========================================
	// Clay Layout
	const Clay_ElementDeclaration OuterContainer = {
		.id = CLAY_ID("OuterContainer"),
		.layout = {
			.sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0)},
			.padding = CLAY_PADDING_ALL(16),
			.childGap = 16
		},
		.backgroundColor = {250, 250, 255, 0},
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	const Clay_ElementDeclaration SideBar = {
		.id = CLAY_ID("SideBar"),
		.layout = {
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = {
				.width = CLAY_SIZING_GROW(300),
				.height = CLAY_SIZING_GROW(0)
			},
			.padding = CLAY_PADDING_ALL(16),
			.childGap = 16
		},
		.backgroundColor = alphaOverride(COLOR_LIGHT, 0.70f),
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	const Clay_ElementDeclaration MainContent = {
		.id = CLAY_ID("MainContent"),
		.layout = {
			.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(64)},
			.padding = CLAY_PADDING_ALL(16)
		},
		.backgroundColor = COLOR_LIGHT,
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	Clay_BeginLayout();

	CLAY(OuterContainer) {
		CLAY(SideBar) {
			Profile(APP);

			for (int i = 0; i < 5; i++) {
				SidebarItemComponent();
			}

			CLAY(MainContent) { SidebarItemComponent(); }
		}
	}

	return Clay_EndLayout();
}

// ===================================================================================
//
// MARK: SDL Main Callbacks
//
// ===================================================================================

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	AppState* APP = SDL_calloc(1, sizeof(AppState));

	*appstate = APP;

	APP->renderer_zoom = 1.0f;
	APP->window_height = 600;
	APP->window_width = 800;

	// Initialize SDL
	if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &APP->window, &APP->renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	TTF_Init();

	// Initialize Clay
	const uint64_t totalMemorySize = Clay_MinMemorySize();
	APP->clay_arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

	Clay_Initialize(
		APP->clay_arena,
		(Clay_Dimensions){(float) APP->window_width, (float) APP->window_height},
		(Clay_ErrorHandler){HandleClayErrors}
	);

	Clay_SDL_SetFont("assets/Roboto-Regular.ttf");
	Clay_SetDebugModeEnabled(true);
	Clay_SetMeasureTextFunction(SDL_Clay_MeasureText, *appstate);

	SDL_Surface* profile = IMG_Load("assets/avatar.jpg");
	SDL_Texture* profileTxt = SDL_CreateTextureFromSurface(APP->renderer, profile);
	APP->img_profile = profileTxt;
	SDL_DestroySurface(profile);

	SDL_Surface* profile2 = IMG_Load("assets/avatar2.png");
	SDL_Texture* profileTxt2 = SDL_CreateTextureFromSurface(APP->renderer, profile2);
	APP->img_profile2 = profileTxt2;
	SDL_DestroySurface(profile2);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	AppState* APP = appstate;

	switch (event->type) {
		case SDL_EVENT_WINDOW_RESIZED:
			APP->window_width = event->window.data1;
			APP->window_height = event->window.data2;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			APP->isMouseDown = true;
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			APP->isMouseDown = false;
			break;
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
		case SDL_EVENT_MOUSE_MOTION:
			APP->mousePositionX = event->motion.x;
			APP->mousePositionY = event->motion.y;
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			APP->mouseWheelX = event->wheel.x;
			APP->mouseWheelY = event->wheel.y;
			break;
		default:
			break;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	AppState* APP = appstate;

	// ===============================
	// SCALE
	const float scale = APP->renderer_zoom;
	SDL_SetRenderScale(APP->renderer, scale, scale);

	// ===============================
	// CLEAR
	SDL_SetRenderDrawColor(APP->renderer, 0, 0, 50, 255);
	SDL_RenderClear(APP->renderer);

	// ===============================
	// TEXT RENDER
	const char* message = "Hello World!";
	const float x = ((float) APP->window_width / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
	const float y = ((float) APP->window_height / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;
	SDL_SetRenderDrawColor(APP->renderer, 255, 255, 255, 255);
	SDL_RenderDebugText(APP->renderer, x, y, message);

	// ===============================
	// CLAY UI RENDER
	Clay_RenderCommandArray commands = UiProcess(APP);
	SDL_Clay_RenderClayCommands(APP->renderer, &commands);

	// ===============================
	// FLIP
	SDL_RenderPresent(APP->renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	SDL_free(appstate);
}