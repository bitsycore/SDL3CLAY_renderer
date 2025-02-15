#include "screen_main.h"

#include <clay.h>
#include <SDL3_image/SDL_image.h>

#include "screen_test_1.h"
#include "screen_test_2.h"
#include "screen_test_3.h"
#include "../colors.h"
#include "../screen_manager.h"
#include "../components/component_profile.h"
#include "../components/component_sidebar_item.h"
#include "../../common/memory_leak.h"

typedef struct Data {
    Arena *arena;
    SDL_Texture* img_profile1;
    SDL_Texture* img_profile2;
} Data;

static void init(AppState *APP, void **screen_state) {
    Data* DATA = ml_malloc(sizeof(Data));

    const size_t arena_size = Arena_requiredSize(512);
    DATA->arena = Arena_init(ml_malloc(arena_size), arena_size);

    DATA->img_profile1 = IMG_LoadTexture(APP->renderer, "assets/avatar.jpg");
    DATA->img_profile2 = IMG_LoadTexture(APP->renderer, "assets/avatar2.png");

    *screen_state = DATA;
}

static void onClick(void* data) {
    switch ((int)data) {
        case SCREEN_TEST_1:
            ScreenManager_setNextScreen(ScreenTest1_new());
            break;
        case SCREEN_TEST_2:
            ScreenManager_setNextScreen(ScreenTest2_new());
            break;
        case SCREEN_TEST_3:
            ScreenManager_setNextScreen(ScreenTest3_new());
            break;
        default: ;
    }

}

void update(AppState *APP, void *screen_state) {
    Data* DATA = screen_state;
    Arena_reset(DATA->arena);

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
            Profile_component(&DATA->img_profile1, &DATA->img_profile2, DATA->arena);

            SidebarItem_componentWithData(CLAY_STRING("Go to Screen 1"), onClick, DATA->arena, (void*)SCREEN_TEST_1);
            SidebarItem_componentWithData(CLAY_STRING("Go to Screen 2"), onClick, DATA->arena, (void*)SCREEN_TEST_2);
            SidebarItem_componentWithData(CLAY_STRING("Go to Screen 3"), onClick, DATA->arena, (void*)SCREEN_TEST_3);
        }
    }
}

static void destroy(AppState *APP, void *screen_state) {
    const Data* DATA = screen_state;
    SDL_DestroyTexture(DATA->img_profile1);
    SDL_DestroyTexture(DATA->img_profile2);
    ml_free(DATA->arena);
    ml_free(screen_state);
}

Screen ScreenMain_new() {
    return (Screen){
        .uuid = UUID_new(),
        .type_id = SCREEN_MAIN,
        .state = NULL,
        .on_init = init,
        .on_update = update,
        .on_destroy = destroy,
        .init_done = false,
        .destroy_done = false,
        .update_rate_ms = SCREEN_FPS_TO_MS(60)
    };
}