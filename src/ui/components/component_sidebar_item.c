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

typedef struct onHoverEvent {
	SidebarItem_onClickCallback onClick;
	void* onClickData;
} OnHoverEvent;

static void onHover(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		const OnHoverEvent* event = (OnHoverEvent*) userData;
		if (event && event->onClick) {
			event->onClick(event->onClickData);
		}
	}
}

static void onHoverNoData(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
	if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
		const SidebarItem_onClickCallback event = (SidebarItem_onClickCallback) userData;
		if (event) {
			event(NULL);
		}
	}
}

void SidebarItem_componentWithData(
	const Clay_String text,
	const SidebarItem_onClickCallback onClick,
	Arena* frameData,
	void* onClickData
) {
	CLAY(config()) {
		if (frameData) {
			OnHoverEvent* event = Arena_alloc(frameData, sizeof(OnHoverEvent));
			event->onClick = onClick;
			event->onClickData = onClickData;
			Clay_OnHover(onHover, (intptr_t) event);
		} else {
			Clay_OnHover(onHoverNoData, (intptr_t) onClick);
		}
		CLAY_TEXT(
			text,
			CLAY_TEXT_CONFIG(
				{
				.fontSize = 24,
				.textColor = Clay_Hovered() ? COLOR_LIGHT : COLOR_WHITE_NICE
				}
			)
		);
	}
}

void SidebarItem_component(
	const Clay_String text,
	const SidebarItem_onClickCallback onClick
	) {
	SidebarItem_componentWithData(text, onClick, NULL, NULL);
}