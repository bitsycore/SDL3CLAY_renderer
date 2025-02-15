#ifndef SIDEBAR_ITEM_COMPONENTS_H
#define SIDEBAR_ITEM_COMPONENTS_H

#include <clay.h>

#include "../../common/arena.h"

typedef void (*SidebarItem_onClickCallback)(void*);

void SidebarItem_componentWithData(
	Clay_String text,
	SidebarItem_onClickCallback onClick,
	Arena* frameData,
	void* onClickData
);

void SidebarItem_component(
	Clay_String text,
	SidebarItem_onClickCallback onClick
);

#endif //SIDEBAR_ITEM_COMPONENTS_H