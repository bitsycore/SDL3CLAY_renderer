#include "screen_test_2.h"

#include <clay.h>
#include <SDL3_image/SDL_image.h>

#include "screen_main.h"
#include "../colors.h"
#include "../screen_manager.h"
#include "../components/component_profile.h"
#include "../components/component_sidebar_item.h"
#include "../../common/memory_leak.h"

typedef struct Data {
    SDL_Texture* img_profile1;
} Data;

static void init(AppState *APP, void **screen_state) {
    Data* DATA = ml_malloc(sizeof(Data));

    DATA->img_profile1 = IMG_LoadTexture(APP->renderer, "assets/avatar2.png");

    *screen_state = DATA;
}

static void onClick(void* event) {
    ScreenManager_setNextScreen(ScreenMain_new());
}

static void update(AppState *APP, void *screen_state) {
    Data* DATA = screen_state;

    // ========================================
    // Clay Layout
    const Clay_ElementDeclaration OuterContainer = {
        .id = CLAY_ID("OuterContainer"),
        .layout = {
            .sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0)},
            .padding = CLAY_PADDING_ALL(16)
        }
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
        .backgroundColor = Color_alphaOver(COLOR_DARK_BLUE, 0.75f),
        .cornerRadius = CLAY_CORNER_RADIUS(8),
        .scroll = {
            .vertical = true
        },
        .border = {
            .width = CLAY_BORDER_OUTSIDE(2),
            .color = Color_alphaOver(COLOR_BLACK_NICE, 0.5f)
        }
    };

    CLAY(OuterContainer) {
        CLAY(SideBar) {
            Profile_component(&DATA->img_profile1, NULL, NULL);
            CLAY_TEXT(
                CLAY_STRING("Test_2"),
                CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = COLOR_WHITE_NICE })
            );
            SidebarItem_component(CLAY_STRING("Main Menu"), onClick);
        }
    }
}

static void destroy(AppState *APP, void *screen_state) {
    Data* DATA = screen_state;
    SDL_DestroyTexture(DATA->img_profile1);
    ml_free(DATA);
}

Screen ScreenTest2_new() {
    return (Screen){
        .uuid = UUID_new(),
        .type_id = SCREEN_TEST_1,
        .state = NULL,
        .on_init = init,
        .on_update = update,
        .on_destroy = destroy,
        .init_done = false,
        .destroy_done = false,
        .update_rate_ms = SCREEN_FPS_TO_MS(60)
    };
}