//
// Created by Doge on 09/02/2025.
//

#include "components.h"
#include "colors.h"

void SidebarItemComponent() {
	const Clay_ElementDeclaration sidebarItemConfig = {
		.layout = {
			.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIXED(50)
			}
		},
		.backgroundColor = COLOR_ORANGE
	};
	CLAY(sidebarItemConfig) {
		CLAY_TEXT(CLAY_STRING("Sidebar Item"), CLAY_TEXT_CONFIG({.fontSize = 16, .textColor = {255, 255, 255, 255}}));
	}
}