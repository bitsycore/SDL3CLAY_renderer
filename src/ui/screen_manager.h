#ifndef SCREEN_H
#define SCREEN_H

#include "../appstate.h"
#include "../../vendor/clay.h"
#include "../common/uuid.h"

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
    ScreenInitFun on_init;
    ScreenUpdateFun on_update;
    bool destroy_done;
    ScreenDestroyFun on_destroy;
} Screen;

void SetNextScreen(Screen screen);
Screen* GetCurrentScreen();

void InitScreen(AppState * APP);
void UpdateScreen(AppState * APP);
void DestroyScreen(AppState* APP, bool force);

#endif //SCREEN_H
