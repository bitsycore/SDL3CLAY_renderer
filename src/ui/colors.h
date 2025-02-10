#ifndef COLORS_H
#define COLORS_H

#include "../../vendor/clay.h"

#define COLOR_BLACK			(Clay_Color) { 0, 0, 0, 255 }
#define COLOR_WHITE			(Clay_Color) { 255, 255, 255, 255 }
#define COLOR_TRANSPARENT	(Clay_Color) { 0, 0, 0, 0 }
#define COLOR_LIGHT			(Clay_Color) { 224, 215, 210, 55 }
#define COLOR_RED 			(Clay_Color) { 168, 66, 28, 255 }
#define COLOR_DARK			(Clay_Color) { 26, 56, 28, 120 }
#define COLOR_ORANGE 		(Clay_Color) { 225, 138, 50, 120 }

Clay_Color Color_AlphaOver(Clay_Color in, float alpha);

#endif //COLORS_H