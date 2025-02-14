#ifndef SCREEN_H
#define SCREEN_H

#include "../appstate.h"
#include <clay.h>
#include "../common/uuid.h"

#define SCREEN_FPS_TO_MS(FPS) 1000.0f/(FPS)

typedef enum ScreenList {
    SCREEN_LIST_PROFILE = 0,
} ScreenList;

typedef void (*ScreenInitFun)(AppState * APP, void ** screen_state);
typedef void (*ScreenUpdateFun)(AppState * APP, void * screen_state);
typedef void (*ScreenDestroyFun)(AppState * APP, void * screen_state);

typedef struct Screen {
    UUID uuid;
    int type_id;
    void* state;
    bool init_done;
    bool destroy_done;
    ScreenInitFun on_init;
    ScreenUpdateFun on_update;
    ScreenDestroyFun on_destroy;
    float update_rate_ms;
    uint64_t last_update_time;
} Screen;

void ScreenManager_setNextScreen(Screen screen);
Screen* ScreenManager_getCurrentScreen();

void ScreenManager_runScreenInit(AppState * APP);
void ScreenManager_runScreenUpdate(AppState * APP);
void ScreenManager_runScreenDestroy(AppState* APP, bool is_app_quit);

bool ScreenManager_isScreenReadyToUpdate();

#endif //SCREEN_H
