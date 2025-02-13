#include "colors.h"

void Color_RGBtoHSL(const float r, const float g, const float b, float* h, float* s, float* l) {
	const float max = r > (g > b ? g : b) ? r : g > b ? g : b;
	const float min = r < (g < b ? g : b) ? r : g < b ? g : b;

	*l = (max + min) / 2.0f;

	if (max == min) {
		*h = *s = 0.0f;
		return;
	}

	const float d = max - min;
	*s = *l > 0.5f ? d / (2.0f - max - min) : d / (max + min);

	if (max == r)
		*h = (g - b) / d + (g < b ? 6.0f : 0.0f);
	else if (max == g)
		*h = (b - r) / d + 2.0f;
	else
		*h = (r - g) / d + 4.0f;

	*h /= 6.0f;
}

static float HueToRGB(const float p, const float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

void Color_HSLtoRGB(const float h, const float s, const float l, float* r, float* g, float* b) {
	if (s == 0.0f) {
		*r = *g = *b = l;
		return;
	}

	const float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
	const float p = 2.0f * l - q;

	*r = HueToRGB(p, q, h + 1.0f / 3.0f);
	*g = HueToRGB(p, q, h);
	*b = HueToRGB(p, q, h - 1.0f / 3.0f);
}