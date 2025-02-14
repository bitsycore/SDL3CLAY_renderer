#include "screen_profile.h"

#include <clay.h>
#include <SDL3_image/SDL_image.h>
#include "../colors.h"
#include "../screen_manager.h"
#include "../components/component_profile.h"
#include "../components/component_sidebar_item.h"
#include "../../common/memory_leak.h"

static void ScreenProfile_init(AppState *APP, void **screen_state) {
    Data* DATA = ml_malloc(sizeof(Data));

    const size_t arena_size = Arena_requiredSize(128);
    DATA->arena = Arena_init(ml_malloc(arena_size), arena_size);

    DATA->img_profile1 = IMG_LoadTexture(APP->renderer, "assets/avatar2.png");
    DATA->img_profile2 = IMG_LoadTexture(APP->renderer, "assets/avatar.jpg");

    *screen_state = DATA;
}

static void ScreenProfile_update(AppState *APP, void *screen_state) {
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

    const Clay_ElementDeclaration MainContent = {
        .id = CLAY_ID("MainContent"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIT()
            },
            .padding = CLAY_PADDING_ALL(64)
        },
        .backgroundColor = COLOR_BLACK_NICE,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    };

    CLAY(OuterContainer) {
        CLAY(SideBar) {
            Profile_component(&DATA->img_profile1, &DATA->img_profile2, DATA->arena);

            SidebarItem_component(11 % 5 * 16);

            CLAY(MainContent) { SidebarItem_component(32); }
        }
    }
}

static void ScreenProfile_destroy(AppState *APP, void *screen_state) {
    const Data* DATA = screen_state;
    SDL_DestroyTexture(DATA->img_profile1);
    SDL_DestroyTexture(DATA->img_profile2);
    ml_free(DATA->arena);
    ml_free(screen_state);
}

Screen ScreenProfile_new() {
    return (Screen){
        .uuid = UUID_new(),
        .type_id = SCREEN_LIST_PROFILE,
        .state = NULL,
        .on_init = ScreenProfile_init,
        .on_update = ScreenProfile_update,
        .on_destroy = ScreenProfile_destroy,
        .init_done = false,
        .destroy_done = false,
        .update_rate_ms = SCREEN_FPS_TO_MS(30)
    };
}