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
#include <clay.h>

// ===============================
// SDL3CLAY
#include "renderer/SDL3CLAY.h"

// ===============================
// App
#include "appstate.h"

// ===============================
// STD
#include <assert.h>
#include <stdlib.h>

#include "common/arena.h"
#include "common/memory_leak.h"
#include "ui/colors.h"
#include "ui/screen_manager.h"
#include "ui/screens/screen_main.h"
#include "ui/components/component_debug_button.h"

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
	APP->img_bg = IMG_LoadTexture(APP->renderer, "assets/bg.jpg");

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

	// ===============================
	// Initialize SDL3CLAY
	SDLCLAY_SetAllocator(ml_callback_malloc, ml_callback_free);
	SDLCLAY_AddFont("assets/Roboto-Regular.ttf", 16);
	Clay_SetMeasureTextFunction(SDLCLAY_MeasureText, NULL);

	// ==============================
	// Set Screen to Load
	ScreenManager_setNextScreen(ScreenMain_new());

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
	if (ScreenManager_isScreenReadyToUpdate()) {
		AppState* APP = appstate;

		// ==============================
		// Delta Calculation
		const Uint64 currentTime = SDL_GetTicks();
		APP->delta = (float) (currentTime - APP->delta_last_time) / 1000.0f;
		APP->delta_last_time = currentTime;

		// ===============================
		// SDL Update
		SDL_SetRenderScale(APP->renderer, APP->renderer_zoom, APP->renderer_zoom);
		SDL_SetRenderDrawColor(APP->renderer, COLOR_CLAY_EXPLODE(COLOR_DARK));
		SDL_RenderClear(APP->renderer);
		SDL_RenderTexture(APP->renderer, APP->img_bg, NULL, NULL);

		// ========================================
		// Clay Update
		Clay_SetLayoutDimensions((Clay_Dimensions){(float) APP->window_width, (float) APP->window_height});
		Clay_SetPointerState((Clay_Vector2){APP->mousePositionX, APP->mousePositionY}, APP->isMouseDown);
		Clay_UpdateScrollContainers(true, (Clay_Vector2){
			                            APP->mouseWheelX * APP->scroll_speed, APP->mouseWheelY * APP->scroll_speed
		                            }, APP->delta);
		Clay_BeginLayout();
		DebugButton_component();

		// ========================================
		// Screen Management

		ScreenManager_runScreenInit(APP);
		ScreenManager_runScreenUpdate(APP);
		ScreenManager_runScreenDestroy(APP, false);

		// ========================================
		// Clay Render
		Clay_RenderCommandArray commands = Clay_EndLayout();
		SDLCLAY_RenderCommands(APP->renderer, &commands);

		// ===============================
		// SDL FLIP BUFFER
		SDL_RenderPresent(APP->renderer);

		// ===============================
		// Reset Mouse wheel
		APP->mouseWheelX = 0;
		APP->mouseWheelY = 0;
	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	AppState* APP = appstate;
	ScreenManager_runScreenDestroy(APP, true);
	SDLCLAY_Quit();
	ml_free(APP->clay_memory);
	ml_free(APP);
	ml_print_memory_leaks();
}