#ifndef SCREEN_H
#define SCREEN_H

#include "../appstate.h"
#include "../../vendor/clay.h"

typedef void (*ScreenFun)(AppState * APP);

void SetCurrentScreen(ScreenFun screen);
ScreenFun GetCurrentScreen();
Clay_RenderCommandArray ProcessCurrentScreen(AppState * APP);

#endif //SCREEN_H
