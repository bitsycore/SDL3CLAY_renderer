#include "clay_renderer_SDL3.h"

#include <float.h>
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


static void SDL_RenderRoundedBorder(
	SDL_Renderer* renderer,
	const SDL_FRect base_rect,
	const float corner_radius,
	const float border_width,
	const Clay_Color color
) {
	SDL_BlendMode blend_mode = {};
	SDL_GetRenderDrawBlendMode(renderer, &blend_mode);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	Clay_SDL_DrawColor(renderer, color);
	SDL_RenderRect(renderer, &base_rect);

	SDL_SetRenderDrawBlendMode(renderer, blend_mode);

	// // Top rectangle
	// const SDL_FRect top_rect = {
	// 	base_rect.x, base_rect.y,
	// 	base_rect.w, border_width
	// };
	// SDL_RenderFillRect(renderer, &top_rect);
	// // Bottom rectangle
	// const SDL_FRect bottom_rect = {
	// 	base_rect.x, base_rect.y + base_rect.h - border_width,
	// 	base_rect.w, border_width
	// };
	// SDL_RenderFillRect(renderer, &bottom_rect);
	// // Left rectangle
	// const SDL_FRect left_rect = {
	// 	base_rect.x, base_rect.y + border_width,
	// 	border_width, base_rect.h - 2 * border_width
	// };
	// SDL_RenderFillRect(renderer, &left_rect);
	// // Right rectangle
	// const SDL_FRect right_rect = {
	// 	base_rect.x + base_rect.w - border_width, base_rect.y + border_width,
	// 	border_width, base_rect.h - 2 * border_width
	// };
	// SDL_RenderFillRect(renderer, &right_rect);
	// SDL_SetRenderDrawBlendMode(renderer, blend_mode);
}

void SDL_RenderArc(SDL_Renderer* renderer, SDL_FPoint center, float radius, float startAngle, float endAngle, float thickness) {
	if (renderer == NULL) {
        SDL_Log("SDL_RenderArc: Renderer is NULL");
        return;
    }

    if (radius <= 0 || thickness <= 0) {
        // Nothing to draw, or invalid parameters
        return;
    }

    // Number of segments to draw the arc. More segments = smoother arc.
    // Adjust this value based on the desired quality and performance.
    int numSegments = NUM_CIRCLE_SEGMENTS; // A reasonable starting point
    if (numSegments < 12) numSegments = 12;  // Minimum segments

    // Convert angles to radians
    float startRad = startAngle * SDL_PI_F / 180.0f;
    float endRad = endAngle * SDL_PI_F / 180.0f;

    // Ensure startAngle is less than endAngle
    if (startRad > endRad) {
    	const float temp = startRad;
    	startRad = endRad;
    	endRad = temp;
    }

    // Calculate the angle increment for each segment
    float angleIncrement = (endRad - startRad) / numSegments;

    // Draw the arc using multiple lines
    for (int i = 0; i < numSegments; ++i) {
        // Calculate the angle for the current segment
        float angle1 = startRad + angleIncrement * i;
        float angle2 = startRad + angleIncrement * (i + 1);

        // Calculate the start and end points of the line segment (outer edge)
        float x1_outer = center.x + radius * SDL_cosf(angle1);
        float y1_outer = center.y + radius * SDL_sinf(angle1);
        float x2_outer = center.x + radius * SDL_cosf(angle2);
        float y2_outer = center.y + radius * SDL_sinf(angle2);

        // Calculate the start and end points of the line segment (inner edge)
        float innerRadius = radius - thickness;
        float x1_inner = center.x + innerRadius * SDL_cosf(angle1);
        float y1_inner = center.y + innerRadius * SDL_sinf(angle1);
        float x2_inner = center.x + innerRadius * SDL_cosf(angle2);
        float y2_inner = center.y + innerRadius * SDL_sinf(angle2);

        // Draw two lines to approximate the segment (outer and inner)
        SDL_RenderLine(renderer, x1_outer, y1_outer, x2_outer, y2_outer);
        SDL_RenderLine(renderer, x1_inner, y1_inner, x2_inner, y2_inner);


        //Connect the ends of the lines to form a filled shape.
        SDL_RenderLine(renderer, x1_outer, y1_outer, x1_inner, y1_inner);
        SDL_RenderLine(renderer, x2_outer, y2_outer, x2_inner, y2_inner);

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

void SDL_Clay_RenderClayCommands(SDL_Renderer* renderer, Clay_RenderCommandArray* commands_array) {
	for (int32_t i = 0; i < commands_array->length; i++) {
		const Clay_RenderCommand* render_command = Clay_RenderCommandArray_Get(commands_array, i);
		const Clay_BoundingBox bounding_box = render_command->boundingBox;

		SDL_FRect f_rect = { bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height };
		SDL_Rect rect = {(int) f_rect.x, (int) f_rect.y, (int) f_rect.w, (int) f_rect.h};

		switch (render_command->commandType) {
			// ====================================================================
			// RECTANGLE
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
				const Clay_RectangleRenderData* config = &render_command->renderData.rectangle;
				Clay_SDL_DrawColor(renderer, config->backgroundColor);
				SDL_BlendMode blendMode = {0};
				SDL_GetRenderDrawBlendMode(renderer, &blendMode);
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				if (config->cornerRadius.topLeft > 0) {
					SDL_RenderFillRoundedRect(renderer, f_rect, config->cornerRadius.topLeft, config->backgroundColor);
				} else {
					SDL_RenderFillRect(renderer, &f_rect);
				}
				SDL_SetRenderDrawBlendMode(renderer, blendMode);
			}
			break;
			// ====================================================================
			// TEXT
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_TEXT: {
				const Clay_TextRenderData* config = &render_command->renderData.text;
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
				SDL_RenderTexture(renderer, texture, NULL, &f_rect);
				SDL_DestroySurface(surface);
				SDL_DestroyTexture(texture);
			}
			break;
			// ====================================================================
			// BORDER
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_BORDER: {
				//SDL_SetRenderClipRect(renderer, &rect);
				const Clay_BorderRenderData* config = &render_command->renderData.border;

				SDL_RenderRoundedBorder(
					renderer,
					f_rect,
					config->cornerRadius.topLeft,
					config->width.top,
					config->color
				);

				//SDL_SetRenderClipRect(renderer, NULL);
			}
			break;
			// ====================================================================
			// IMAGE
			// ====================================================================
			case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
				const Clay_ImageRenderData* config = &render_command->renderData.image;
				SDL_Texture* texture = config->imageData;
				SDL_RenderTexture(renderer, texture, NULL, &f_rect);
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
				SDL_Log("Unknown render command type: %d", render_command->commandType);
		}
	}
}
