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

Clay_RenderCommandArray ClayProcess(AppState * APP) {
	// ========================================
	// Clay Update States
	Clay_SetLayoutDimensions((Clay_Dimensions){(float) APP->window_width, (float) APP->window_height});
	Clay_SetPointerState((Clay_Vector2){APP->mousePositionX, APP->mousePositionY}, APP->isMouseDown);
	Clay_UpdateScrollContainers(true, (Clay_Vector2){ APP->mouseWheelX * APP->scroll_speed, APP->mouseWheelY * APP->scroll_speed }, APP->delta);

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
			.childGap = 16,
		},
		.backgroundColor = alphaOverride(COLOR_LIGHT, 0.70f),
		.cornerRadius = CLAY_CORNER_RADIUS(8),
		.scroll =  (Clay_ScrollElementConfig) {
			.vertical = true
		}
	};

	const Clay_ElementDeclaration MainContent = {
		.id = CLAY_ID("MainContent"),
		.layout = {
			.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(64)},
			.padding = CLAY_PADDING_ALL(16)
		},
		.backgroundColor = COLOR_DARK,
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	Clay_BeginLayout();

	CLAY(OuterContainer) {
		CLAY(SideBar) {
			Profile(APP);

			for (int i = 0; i < 30; i++) {
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
	APP->scroll_speed = 3.1f;
	APP->window_height = 720;
	APP->window_width = 1280;

	// Initialize SDL
	if (!SDL_CreateWindowAndRenderer("Hello World", APP->window_width, APP->window_height, SDL_WINDOW_RESIZABLE, &APP->window, &APP->renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	TTF_Init();

	// Initialize Clay
	const uint64_t totalMemorySize = Clay_MinMemorySize();
	const Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

	Clay_Initialize(
		clay_arena,
		(Clay_Dimensions){(float) APP->window_width, (float) APP->window_height},
		(Clay_ErrorHandler){HandleClayErrors }
	);

	SDLCLAY_SetFont("assets/Roboto-Regular.ttf");
	Clay_SetDebugModeEnabled(true);
	Clay_SetMeasureTextFunction(SDLCLAY_MeasureText, *appstate);

	APP->img_profile = IMG_LoadTexture(APP->renderer, "assets/avatar.jpg");
	APP->img_profile2 = IMG_LoadTexture(APP->renderer, "assets/avatar2.png");

	APP->delta_last_time = SDL_GetTicks();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	AppState* APP = appstate;

	switch (event->type) {
		// ===============================
		// WINDOW EVENTS
		case SDL_EVENT_WINDOW_RESIZED:
			APP->window_width = event->window.data1;
			APP->window_height = event->window.data2;
			break;
		// ===============================
		// MOUSE EVENTS
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			APP->isMouseDown = true;
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			APP->isMouseDown = false;
			break;
		case SDL_EVENT_MOUSE_MOTION:
			APP->mousePositionX = event->motion.x;
			APP->mousePositionY = event->motion.y;
		break;
		case SDL_EVENT_MOUSE_WHEEL:
			APP->mouseWheelX = event->wheel.x;
			APP->mouseWheelY = event->wheel.y;
		break;
		// ===============================
		// KEYBOARD EVENTS
		case SDL_EVENT_KEY_DOWN:
			switch (event->key.key) {
				case SDLK_KP_PLUS:
					APP->renderer_zoom += 0.1f;
					break;
				case SDLK_KP_MINUS:
					APP->renderer_zoom -= 0.1f;
					break;
				default:
					break;
			}
			break;
		// ===============================
		// APP EVENT
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
		default:
			break;
	}

	return SDL_APP_CONTINUE;
}

void delta_update(AppState* APP) {
	const Uint64 currentTime = SDL_GetTicks();
	const float delta = (float) (currentTime - APP->delta_last_time) / 1000.0f; // Convert to seconds
	APP->delta_last_time = currentTime;
	APP->delta = delta;
}

void reset_end_loop(AppState *APP) {
	// Reset Mousewheel
	APP->mouseWheelX = 0;
	APP->mouseWheelY = 0;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	AppState* APP = appstate;

	// ==============================
	// Delta Calculation
	delta_update(APP);

	// ===============================
	// SCALE
	const float scale = APP->renderer_zoom;
	SDL_SetRenderScale(APP->renderer, scale, scale);

	// ===============================
	// CLEAR
	SDL_SetRenderDrawColor(APP->renderer, 0, 0, 50, 255);
	SDL_RenderClear(APP->renderer);

	// ===============================
	// CENTERED TEXT RENDER
	const char* message = "Hello World!";
	const float x = ((float) APP->window_width / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
	const float y = ((float) APP->window_height / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;
	SDL_SetRenderDrawColor(APP->renderer, 255, 255, 255, 255);
	SDL_RenderDebugText(APP->renderer, x, y, message);

	// ===============================
	// CLAY UI RENDER
	Clay_RenderCommandArray commands = ClayProcess(APP);
	SDLCLAY_Render(APP->renderer, &commands);

	// ===============================
	// FLIP
	SDL_RenderPresent(APP->renderer);

	reset_end_loop(APP);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	SDL_free(appstate);
}