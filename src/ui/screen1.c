//
// Created by Doge on 12/02/2025.
//

#include "screen1.h"

#include "colors.h"
#include "components.h"
#include "../../vendor/clay.h"

void Screen1(AppState * APP) {

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
		.backgroundColor = AlphaOver(COLOR_DARK_BLUE, 0.75f),
		.cornerRadius = CLAY_CORNER_RADIUS(8),
		.scroll = {
			.vertical = true
		},
		.border = {
			.width = CLAY_BORDER_OUTSIDE(2),
			.color = AlphaOver(COLOR_BLACK_NICE, 0.5f)
		}
	};

	const Clay_ElementDeclaration MainContent = {
		.id = CLAY_ID("MainContent"),
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIT()},
			.padding = CLAY_PADDING_ALL(64)
		},
		.backgroundColor = COLOR_BLACK_NICE,
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	};

	CLAY(OuterContainer) {
		CLAY(SideBar) {
			ProfileComponent(APP);

			for (int i = 0; i < 30; i++) {
				SidebarItemComponent(i % 5 * 16);
			}

			CLAY(MainContent) { SidebarItemComponent(32); }
		}
	}
}