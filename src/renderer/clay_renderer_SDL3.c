#include "clay_renderer_SDL3.h"

#include <SDL3_ttf/SDL_ttf.h>

static TTF_Font* gFonts[1];
static int NUM_CIRCLE_SEGMENTS = 32;

void Clay_SDL_SetFont(const char* fontPath) {
	gFonts[0] = TTF_OpenFont(fontPath, 16);
	if (gFonts[0] == NULL) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
	}
}

TTF_Font* Clay_SDL_GetFont() {
	return gFonts[0];
}

static void Clay_SDL_DrawColor(SDL_Renderer* renderer, const Clay_Color color) {
	SDL_SetRenderDrawColor(renderer, (int) color.r, (int) color.g, (int) color.b, (int) color.a);
}

//all rendering is performed by a single SDL call, avoiding multiple RenderRect + plumbing choice for circles.
static void SDL_RenderFillRoundedRect(
	SDL_Renderer* renderer,
	const SDL_FRect rect,
	const float cornerRadius,
	const Clay_Color _color
) {
	const SDL_FColor color = {_color.r / 255, _color.g / 255, _color.b / 255, _color.a / 255};

	int indexCount = 0, vertexCount = 0;

	const float minRadius = SDL_min(rect.w, rect.h) / 2.0f;
	const float clampedRadius = SDL_min(cornerRadius, minRadius);

	const int numCircleSegments = SDL_max(NUM_CIRCLE_SEGMENTS, (int) clampedRadius * 0.5f);

	int totalVertices = 4 + 4 * (numCircleSegments * 2) + 2 * 4;
	int totalIndices = 6 + 4 * (numCircleSegments * 3) + 6 * 4;

	SDL_Vertex vertices[totalVertices];
	int indices[totalIndices];

	//define center rectangle
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + clampedRadius, rect.y + clampedRadius}, color, {0, 0}};
	//0 center TL
	vertices[vertexCount++] = (SDL_Vertex){
		{rect.x + rect.w - clampedRadius, rect.y + clampedRadius}, color, {1, 0}
	}; //1 center TR
	vertices[vertexCount++] = (SDL_Vertex){
		{rect.x + rect.w - clampedRadius, rect.y + rect.h - clampedRadius}, color, {1, 1}
	}; //2 center BR
	vertices[vertexCount++] = (SDL_Vertex){
		{rect.x + clampedRadius, rect.y + rect.h - clampedRadius}, color, {0, 1}
	}; //3 center BL

	indices[indexCount++] = 0;
	indices[indexCount++] = 1;
	indices[indexCount++] = 3;
	indices[indexCount++] = 1;
	indices[indexCount++] = 2;
	indices[indexCount++] = 3;

	//define rounded corners as triangle fans
	const float step = SDL_PI_F / 2.0f / (float) numCircleSegments;
	for (int i = 0; i < numCircleSegments; i++) {
		const float angle1 = (float) i * step;
		const float angle2 = ((float) i + 1.0f) * step;

		for (int j = 0; j < 4; j++) {
			// Iterate over four corners
			float cx, cy, signX, signY;

			switch (j) {
				case 0: cx = rect.x + clampedRadius;
					cy = rect.y + clampedRadius;
					signX = -1;
					signY = -1;
					break; // Top-left
				case 1: cx = rect.x + rect.w - clampedRadius;
					cy = rect.y + clampedRadius;
					signX = 1;
					signY = -1;
					break; // Top-right
				case 2: cx = rect.x + rect.w - clampedRadius;
					cy = rect.y + rect.h - clampedRadius;
					signX = 1;
					signY = 1;
					break; // Bottom-right
				case 3: cx = rect.x + clampedRadius;
					cy = rect.y + rect.h - clampedRadius;
					signX = -1;
					signY = 1;
					break; // Bottom-left
				default: return;
			}

			vertices[vertexCount++] = (SDL_Vertex){
				{cx + SDL_cosf(angle1) * clampedRadius * signX, cy + SDL_sinf(angle1) * clampedRadius * signY}, color,
				{0, 0}
			};
			vertices[vertexCount++] = (SDL_Vertex){
				{cx + SDL_cosf(angle2) * clampedRadius * signX, cy + SDL_sinf(angle2) * clampedRadius * signY}, color,
				{0, 0}
			};

			indices[indexCount++] = j; // Connect to corresponding central rectangle vertex
			indices[indexCount++] = vertexCount - 2;
			indices[indexCount++] = vertexCount - 1;
		}
	}

	//Define edge rectangles
	// Top edge
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + clampedRadius, rect.y}, color, {0, 0}}; //TL
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + rect.w - clampedRadius, rect.y}, color, {1, 0}}; //TR

	indices[indexCount++] = 0;
	indices[indexCount++] = vertexCount - 2; //TL
	indices[indexCount++] = vertexCount - 1; //TR
	indices[indexCount++] = 1;
	indices[indexCount++] = 0;
	indices[indexCount++] = vertexCount - 1; //TR
	// Right edge
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + rect.w, rect.y + clampedRadius}, color, {1, 0}}; //RT
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + rect.w, rect.y + rect.h - clampedRadius}, color, {1, 1}}; //RB

	indices[indexCount++] = 1;
	indices[indexCount++] = vertexCount - 2; //RT
	indices[indexCount++] = vertexCount - 1; //RB
	indices[indexCount++] = 2;
	indices[indexCount++] = 1;
	indices[indexCount++] = vertexCount - 1; //RB
	// Bottom edge
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + rect.w - clampedRadius, rect.y + rect.h}, color, {1, 1}}; //BR
	vertices[vertexCount++] = (SDL_Vertex){{rect.x + clampedRadius, rect.y + rect.h}, color, {0, 1}}; //BL

	indices[indexCount++] = 2;
	indices[indexCount++] = vertexCount - 2; //BR
	indices[indexCount++] = vertexCount - 1; //BL
	indices[indexCount++] = 3;
	indices[indexCount++] = 2;
	indices[indexCount++] = vertexCount - 1; //BL
	// Left edge
	vertices[vertexCount++] = (SDL_Vertex){{rect.x, rect.y + rect.h - clampedRadius}, color, {0, 1}}; //LB
	vertices[vertexCount++] = (SDL_Vertex){{rect.x, rect.y + clampedRadius}, color, {0, 0}}; //LT

	indices[indexCount++] = 3;
	indices[indexCount++] = vertexCount - 2; //LB
	indices[indexCount++] = vertexCount - 1; //LT
	indices[indexCount++] = 0;
	indices[indexCount++] = 3;
	indices[indexCount++] = vertexCount - 1; //LT

	// Render everything
	SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);
}

static void SDL_RenderArc(
	SDL_Renderer* renderer,
	const SDL_FPoint center,
	const float radius,
	const float startAngle,
	const float endAngle,
	const float thickness,
	const Clay_Color color
) {
	Clay_SDL_DrawColor(renderer, color);

	const float radStart = startAngle * (SDL_PI_F / 180.0f);
	const float radEnd = endAngle * (SDL_PI_F / 180.0f);

	const int numCircleSegments = SDL_max(NUM_CIRCLE_SEGMENTS, (int)(radius * 1.5f));
	//increase circle segments for larger circles, 1.5 is arbitrary.

	const float angleStep = (radEnd - radStart) / (float) numCircleSegments;
	const float thicknessStep = 0.1f;
	//arbitrary value to avoid overlapping lines. Changing THICKNESS_STEP or numCircleSegments might cause artifacts.

	for (float t = thicknessStep; t < thickness - thicknessStep; t += thicknessStep) {
		SDL_FPoint points[numCircleSegments + 1];
		const float clampedRadius = SDL_max(radius - t, 1.0f);

		for (int i = 0; i <= numCircleSegments; i++) {
			const float angle = radStart + i * angleStep;
			points[i] = (SDL_FPoint){
				SDL_roundf(center.x + SDL_cosf(angle) * clampedRadius),
				SDL_roundf(center.y + SDL_sinf(angle) * clampedRadius)
			};
		}
		SDL_BlendMode blendMode = {0};
		SDL_GetRenderDrawBlendMode(renderer, &blendMode);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderLines(renderer, points, numCircleSegments + 1);
		SDL_SetRenderDrawBlendMode(renderer, blendMode);
	}
}

// ReSharper disable once CppParameterMayBeConst
Clay_Dimensions SDL_Clay_MeasureText(
	Clay_StringSlice text,
	/*ReSharper disable once CppParameterNeverUsed*/ Clay_TextElementConfig* config,
	/*ReSharper disable once CppParameterNeverUsed*/ void* userData
) {
	int width = 0;
	TTF_Font* font = Clay_SDL_GetFont();
	const int height = TTF_GetFontHeight(font);
	TTF_MeasureString(font, text.chars, text.length, 0, &width, NULL);
	const Clay_Dimensions result = {
		.height = (float) height,
		.width = (float) width,
	};
	return result;
}

void SDL_Clay_RenderClayCommands(SDL_Renderer* renderer, Clay_RenderCommandArray* rcommands) {
	for (uint32_t i = 0; i < rcommands->length; i++) {
		const Clay_RenderCommand* rcmd = Clay_RenderCommandArray_Get(rcommands, i);
		const Clay_BoundingBox bounding_box = rcmd->boundingBox;
		SDL_FRect frect = {bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height};
		SDL_Rect rect = {(int) frect.x, (int) frect.y, (int) frect.w, (int) frect.h};
		switch (rcmd->commandType) {
			// ====================================================================
			// RECTANGLE
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
				const Clay_RectangleRenderData* config = &rcmd->renderData.rectangle;
				Clay_SDL_DrawColor(renderer, config->backgroundColor);
				SDL_BlendMode blendMode = {0};
				SDL_GetRenderDrawBlendMode(renderer, &blendMode);
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				if (config->cornerRadius.topLeft > 0) {
					SDL_RenderFillRoundedRect(renderer, frect, config->cornerRadius.topLeft, config->backgroundColor);
				} else {
					SDL_RenderFillRect(renderer, &frect);}
				SDL_SetRenderDrawBlendMode(renderer, blendMode);
			}
			break;
			// ====================================================================
			// TEXT
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_TEXT: {
				const Clay_TextRenderData* config = &rcmd->renderData.text;
				const SDL_Color color = {
					(int) config->textColor.r, (int) config->textColor.g,
					(int) config->textColor.b, (int) config->textColor.a
				};
				TTF_Font* font = gFonts[0];
				SDL_Surface* surface = TTF_RenderText_Blended(
					font, config->stringContents.chars,
					config->stringContents.length, color
				);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_RenderTexture(renderer, texture, NULL, &frect);
				SDL_DestroySurface(surface);
				SDL_DestroyTexture(texture);
			}
			break;
			// ====================================================================
			// BORDER
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_BORDER: {
				const Clay_BorderRenderData* config = &rcmd->renderData.border;

				const float minRadius = SDL_min(frect.w, frect.h) / 2.0f;
				const Clay_CornerRadius clampedRadii = {
					.topLeft = SDL_min(config->cornerRadius.topLeft, minRadius),
					.topRight = SDL_min(config->cornerRadius.topRight, minRadius),
					.bottomLeft = SDL_min(config->cornerRadius.bottomLeft, minRadius),
					.bottomRight = SDL_min(config->cornerRadius.bottomRight, minRadius)
				};

				// ====================================================================
				// DRAW EDGE
				Clay_SDL_DrawColor(renderer, config->color);
				SDL_BlendMode blendMode = {0};
				SDL_GetRenderDrawBlendMode(renderer, &blendMode);
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				if (config->width.left > 0) {
					const float starting_y = frect.y + clampedRadii.topLeft;
					const float length = frect.h - clampedRadii.topLeft - clampedRadii.bottomLeft;
					SDL_FRect line = {frect.x, starting_y, config->width.left, length};
					SDL_RenderFillRect(renderer, &line);
				}
				if (config->width.right > 0) {
					const float starting_x = frect.x + frect.w - (float) config->width.right;
					const float starting_y = frect.y + clampedRadii.topRight;
					const float length = frect.h - clampedRadii.topRight - clampedRadii.bottomRight;
					SDL_FRect line = {starting_x, starting_y, config->width.right, length};
					SDL_RenderFillRect(renderer, &line);
				}
				if (config->width.top > 0) {
					const float starting_x = frect.x + clampedRadii.topLeft;
					const float length = frect.w - clampedRadii.topLeft - clampedRadii.topRight;
					SDL_FRect line = {starting_x, frect.y, length, config->width.top};
					SDL_RenderFillRect(renderer, &line);
				}
				if (config->width.bottom > 0) {
					const float starting_x = frect.x + clampedRadii.bottomLeft;
					const float starting_y = frect.y + frect.h - (float) config->width.bottom;
					const float length = frect.w - clampedRadii.bottomLeft - clampedRadii.bottomRight;
					SDL_FRect line = {starting_x, starting_y, length, config->width.bottom};
					SDL_RenderFillRect(renderer, &line);
				}
				//corners
				if (config->cornerRadius.topLeft > 0) {
					const float centerX = frect.x + clampedRadii.topLeft - 1;
					const float centerY = frect.y + clampedRadii.topLeft;
					SDL_RenderArc(renderer, (SDL_FPoint){centerX, centerY}, clampedRadii.topLeft,
					              180.0f, 270.0f, config->width.top, config->color);
				}
				if (config->cornerRadius.topRight > 0) {
					const float centerX = frect.x + frect.w - clampedRadii.topRight - 1;
					const float centerY = frect.y + clampedRadii.topRight;
					SDL_RenderArc(renderer, (SDL_FPoint){centerX, centerY}, clampedRadii.topRight,
					              270.0f, 360.0f, config->width.top, config->color);
				}
				if (config->cornerRadius.bottomLeft > 0) {
					const float centerX = frect.x + clampedRadii.bottomLeft - 1;
					const float centerY = frect.y + frect.h - clampedRadii.bottomLeft - 1;
					SDL_RenderArc(renderer, (SDL_FPoint){centerX, centerY}, clampedRadii.bottomLeft,
					              90.0f, 180.0f, config->width.bottom, config->color);
				}
				if (config->cornerRadius.bottomRight > 0) {
					const float centerX = frect.x + frect.w - clampedRadii.bottomRight - 1;
					//TODO: why need to -1 in all calculations???
					const float centerY = frect.y + frect.h - clampedRadii.bottomRight - 1;
					SDL_RenderArc(renderer, (SDL_FPoint){centerX, centerY}, clampedRadii.bottomRight,
					              0.0f, 90.0f, config->width.bottom, config->color);
				}

				SDL_SetRenderDrawBlendMode(renderer, blendMode);
			}
			break;
			// ====================================================================
			// IMAGE
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
				const Clay_ImageRenderData* config = &rcmd->renderData.image;
				SDL_Texture* texture = config->imageData;
				SDL_RenderTexture(renderer, texture, NULL, &frect);
			}
			break;
			// ====================================================================
			// SCISSOR START
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
				SDL_SetRenderClipRect(renderer, &rect);
			}
			break;
			// ====================================================================
			// SCISSOR END
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
				SDL_SetRenderClipRect(renderer, NULL);
			}
			break;
			default:
				SDL_Log("Unknown render command type: %d", rcmd->commandType);
		}
	}
}
