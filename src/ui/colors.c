#ifndef COLORS_H
#define COLORS_H

#include "../../vendor/clay.h"

#define COLOR_LIGHT		(Clay_Color){224, 215, 210, 255}
#define COLOR_RED 		(Clay_Color){168, 66, 28, 255}
#define COLOR_ORANGE 	(Clay_Color){225, 138, 50, 255}

inline Clay_Color alphaOverride(const Clay_Color in, const float alpha);

#endif //COLORS_H