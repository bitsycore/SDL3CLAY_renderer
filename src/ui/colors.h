#ifndef COLORS_H
#define COLORS_H

#include <SDL3/SDL_pixels.h>
#include "../../vendor/clay.h"

#define COLOR_BLACK			 (Clay_Color){ 0, 0, 0, 255 }
#define COLOR_WHITE			 (Clay_Color){ 255, 255, 255, 255 }
#define COLOR_TRANSPARENT	 (Clay_Color){ 0, 0, 0, 0 }

#define COLOR_BLACK_NICE	 (Clay_Color){ 33, 36, 39, 255 }
#define COLOR_WHITE_NICE	 (Clay_Color){ 250, 249, 246, 255 }

#define COLOR_LIGHT			 (Clay_Color){ 211, 193, 224, 255 }
#define COLOR_DARK			 (Clay_Color){ 26, 15, 28, 255 }

#define COLOR_RED 			 (Clay_Color){ 150, 42, 42, 255 }
#define COLOR_ORANGE 		 (Clay_Color){ 225, 138, 50, 255 }
#define COLOR_DARK_BLUE 	 (Clay_Color){ 33, 37, 87, 255 }

#define COLOR_CLAY_EXPLODE(color) color.r, color.g, color.b, color.a

void RGBtoHSL(float r, float g, float b, float* h, float* s, float* l);
void HSLtoRGB(float h, float s, float l, float* r, float* g, float* b);

static inline Clay_Color AlphaOver(const Clay_Color in, const float alpha) {
	return (Clay_Color) {in.r, in.g, in.b, alpha * 255};
}

static inline Clay_Color HueOver(const Clay_Color in, const float hueOffset) {
	float h, s, l;
	RGBtoHSL(in.r, in.g, in.b, &h, &s, &l);

	h += hueOffset; // Adjust hue
	if (h > 1.0f) h -= 1.0f;
	if (h < 0.0f) h += 1.0f;

	Clay_Color out = in; // Keep alpha unchanged
	HSLtoRGB(h, s, l, &out.r, &out.g, &out.b);
	return out;
}

static inline Clay_Color SatOver(const Clay_Color in, const float satOffset) {
	float h, s, l;
	RGBtoHSL(in.r, in.g, in.b, &h, &s, &l);

	s += satOffset; // Adjust saturation
	if (s > 1.0f) s = 1.0f;
	if (s < 0.0f) s = 0.0f;

	Clay_Color out = in; // Keep alpha unchanged
	HSLtoRGB(h, s, l, &out.r, &out.g, &out.b);
	return out;
}

static inline Clay_Color LightOver(const Clay_Color in, const float lightOffset) {
	float h, s, l;
	RGBtoHSL(in.r, in.g, in.b, &h, &s, &l);

	l += lightOffset; // Adjust lightness
	if (l > 1.0f) l = 1.0f;
	if (l < 0.0f) l = 0.0f;

	Clay_Color out = in; // Keep alpha unchanged
	HSLtoRGB(h, s, l, &out.r, &out.g, &out.b);
	return out;
}

static inline Clay_Color Darken(const Clay_Color in, const float amount) {
	return (Clay_Color) {
		.r = in.r * (1.0f - amount),
		.g = in.g * (1.0f - amount),
		.b = in.b * (1.0f - amount),
		.a = in.a
	};
}

static inline Clay_Color Lighten(const Clay_Color in, const float amount) {
	return (Clay_Color) {
		.r = in.r + (255 - in.r) * amount,
		.g = in.g + (255 - in.g) * amount,
		.b = in.b + (255 - in.b) * amount,
		.a = in.a
	};
}

static inline Clay_Color LerpColor(const Clay_Color a, const Clay_Color b, const float t) {
	return (Clay_Color) {
		.r = (a.r + (b.r - a.r) * t),
		.g = (a.g + (b.g - a.g) * t),
		.b = (a.b + (b.b - a.b) * t),
		.a = (a.a + (b.a - a.a) * t)
	};
}

static inline SDL_Color ClayToSdlColor(const Clay_Color color) {
	return (SDL_Color) { (int) color.r, (int) color.g, (int) color.b, (int) color.a};
}

static inline Clay_Color SdlToClayColor(const SDL_Color color) {
	return (Clay_Color) {color.r, color.g, color.b, color.a};
}



#endif //COLORS_H