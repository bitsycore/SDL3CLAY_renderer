#include "colors.h"

Clay_Color alphaOverride(const Clay_Color in, const float alpha){
	return (Clay_Color) {in.r, in.g, in.b, alpha * 255};
}