#ifndef SCREEN_H
#define SCREEN_H

#include "../appstate.h"
#include <clay.h>
#include "../common/uuid.h"

#define SCREEN_FPS_TO_MS(FPS) 1000.0f/(FPS)

typedef enum ScreenList {
    SCREEN_MAIN = 0,
    SCREEN_TEST_1 = 1,
    SCREEN_TEST_2 = 2,
    SCREEN_TEST_3 = 3,
} ScreenList;

typedef void* (*ScreenInitFun)(AppState * APP);
typedef void (*ScreenUpdateFun)(AppState * APP, void * screenState);
typedef void (*ScreenDestroyFun)(AppState * APP, void * screenState);

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
void ScreenManager_runScreenDestroy(AppState* APP);

void ScreenManager_end(AppState* APP);

bool ScreenManager_isScreenReadyToUpdate();

#endif //SCREEN_H
