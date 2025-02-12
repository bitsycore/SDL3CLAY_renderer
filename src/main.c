// ===============================
// SDL
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

// ===============================
// CLAY
#define CLAY_IMPLEMENTATION
#include "../vendor/clay.h"

// ===============================
// SDL3CLAY
#include "renderer/SDL3CLAY.h"

// ===============================
// App
#include "appstate.h"

// ===============================
// STD
#include <stdlib.h>

#include "ui/colors.h"
#include "ui/screen1.h"
#include "ui/screen_manager.h"
#include "common/arena.h"
#include "common/memory_leak.h"

void HandleClayErrors(Clay_ErrorData errorData) {
	SDL_Log("%s", errorData.errorText.chars);
	switch (errorData.errorType) {
		default:
	}
}

// ===================================================================================
//
// MARK: SDL Main Callbacks
//
// ===================================================================================

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	// ===============================
	// Initialize AppState
	AppState* APP = AppState_new();
	*appstate = APP;

	// ===============================
	// Initialize SDL
	if (!SDL_CreateWindowAndRenderer("Hello World", APP->window_width, APP->window_height, SDL_WINDOW_RESIZABLE, &APP->window, &APP->renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	TTF_Init();

	// ===============================
	// Init Test Texture
	APP->img_profile = IMG_LoadTexture(APP->renderer, "assets/avatar.jpg");
	APP->img_profile2 = IMG_LoadTexture(APP->renderer, "assets/avatar2.png");

	// ===============================
	// Initialize Clay
	const uint64_t totalMemorySize = Clay_MinMemorySize();
	void* clay_memory = ml_malloc(totalMemorySize);
	APP->clay_memory = clay_memory;
	const Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clay_memory);

	Clay_Initialize(
		clay_arena,
		(Clay_Dimensions){(float) APP->window_width, (float) APP->window_height},
		(Clay_ErrorHandler){HandleClayErrors }
	);

	Clay_SetDebugModeEnabled(true);

	// ===============================
	// Initialize SDL3CLAY
	SDLCLAY_SetAllocator(ml_callback_malloc, ml_callback_free);
	SDLCLAY_AddFont("assets/Roboto-Regular.ttf", 16);

	Clay_SetMeasureTextFunction(SDLCLAY_MeasureText, NULL);

	SetCurrentScreen(Screen1);

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

SDL_AppResult SDL_AppIterate(void* appstate) {
	AppState* APP = appstate;
	arena_reset(APP->frame_arena);

	// ==============================
	// Delta Calculation
	const Uint64 currentTime = SDL_GetTicks();
	APP->delta = (float) (currentTime - APP->delta_last_time) / 1000.0f;
	APP->delta_last_time = currentTime;

	// ===============================
	// SCALE
	const float scale = APP->renderer_zoom;
	SDL_SetRenderScale(APP->renderer, scale, scale);

	// ===============================
	// CLEAR
	SDL_SetRenderDrawColor(APP->renderer, COLOR_CLAY_EXPLODE(COLOR_DARK));
	SDL_RenderClear(APP->renderer);

	// ===============================
	// CENTERED DEBUG TEST TEXT
	const char* message = "Hello World!";
	const float x = ((float) APP->window_width / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
	const float y = ((float) APP->window_height / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;
	SDL_SetRenderDrawColor(APP->renderer, 255, 255, 255, 255);
	SDL_RenderDebugText(APP->renderer, x, y, message);

	// ===============================
	// CLAY UI RENDER
	Clay_RenderCommandArray commands = ProcessCurrentScreen(APP);
	SDLCLAY_RenderCommands(APP->renderer, &commands);

	// ===============================
	// FLIP
	SDL_RenderPresent(APP->renderer);

	// ===============================
	// Reset Mouse wheel
	APP->mouseWheelX = 0;
	APP->mouseWheelY = 0;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	AppState* APP = appstate;
	SDLCLAY_Quit();
	ml_free(APP->frame_arena);
	ml_free(APP->clay_memory);
	ml_free(APP);
	ml_print_memory_leaks();
}