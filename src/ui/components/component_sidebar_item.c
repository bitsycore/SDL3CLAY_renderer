#include "component_sidebar_item.h"

#include "../colors.h"

static inline Clay_ElementDeclaration config() {
	return (Clay_ElementDeclaration){
		.border = {
			.width = CLAY_BORDER_ALL(4),
			.color = COLOR_BLACK_NICE
		},
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIT(50)
			},
			.padding = CLAY_PADDING_ALL(24),
			.childAlignment = {
				.x = CLAY_ALIGN_X_CENTER,
				.y = CLAY_ALIGN_Y_CENTER
			},
			.childGap = 16
		},
		.backgroundColor = Clay_Hovered() ? COLOR_BLACK_NICE : COLOR_DARK_BLUE,
		.cornerRadius = CLAY_CORNER_RADIUS(16)
	};
}

static void onHover (Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		Clay_SetDebugModeEnabled(true);
	}
}

void SidebarItem_component(const int i) {
	CLAY(config()) {
		Clay_OnHover(onHover, (intptr_t)NULL);
		CLAY_TEXT(
			CLAY_STRING("Sidebar Item"),
			CLAY_TEXT_CONFIG(
				{
				.fontSize = i,
				.textColor = Clay_Hovered() ? COLOR_LIGHT : COLOR_WHITE_NICE
				}
			)
		);
	}
}