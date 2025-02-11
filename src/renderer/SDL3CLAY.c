#include "SDL3CLAY.h"

SDLCLAY_Fun_Logger SDLCLAY_LOG = SDL_Log;
SDLCLAY_Fun_Malloc SDLCLAY_MALLOC = SDL_malloc;
SDLCLAY_Fun_Free SDLCLAY_FREE = SDL_free;

// ===================================================================================
// MARK: FONTS
// ===================================================================================

typedef struct Font {
	TTF_Font* ttf_fonts[SDLCLAY_FONT_MAX_SIZE];
	int init_size;
} Font;

static struct FontsHolder {
	Font* fonts;
	int count;
	struct FontsHolder* next;
	int deepness;
} FONTS_HOLDER = {0};

static void FontHolder_init(struct FontsHolder * font_holder, const int deepness) {
	const size_t size = SDLCLAY_FONT_HOLDER_CAPACITY * sizeof(Font);
	font_holder->fonts = SDLCLAY_MALLOC(size);
	SDL_memset(font_holder->fonts, 0, size);
	font_holder->count = 0;
	font_holder->next = NULL;
	font_holder->deepness = deepness;
}

static void FontHolder_free(struct FontsHolder * font_holder) {
	struct FontsHolder* current = font_holder;
	do {
		struct FontsHolder* next = current->next;
		SDLCLAY_FREE(current->fonts);
		if (current->deepness != 0) {
			SDLCLAY_FREE(current);
		}
		current = next;
	} while (current != NULL);
}

int SDLCLAY_AddFont(const char * font_path, const int init_size) {
	if (font_path == NULL || init_size >= SDLCLAY_FONT_MAX_SIZE) {
		SDLCLAY_LOG("Invalid font path:\"%s\" or size: %d", font_path, init_size);
		return -1;
	}

	TTF_Font* font = TTF_OpenFont(font_path, (float) init_size);

	if (font == NULL) {
		SDLCLAY_LOG("Failed to load font: %s, %s", font_path, SDL_GetError());
		return -1;
	}

	return SDLCLAY_AddFontRaw(font, init_size);
}

int SDLCLAY_AddFontRaw(TTF_Font * font, const int init_size) {
	if (font == NULL || init_size >= SDLCLAY_FONT_MAX_SIZE) {
		SDLCLAY_LOG("Invalid font %p or size: %d", font, init_size);
		return -1;
	}

	if (FONTS_HOLDER.fonts == NULL) {
		FontHolder_init(&FONTS_HOLDER, 0);
	}

	struct FontsHolder* current = &FONTS_HOLDER;
	while (current->count >= SDLCLAY_FONT_HOLDER_CAPACITY) {
		if (current->next == NULL) {
			struct FontsHolder* new = SDLCLAY_MALLOC(sizeof(struct FontsHolder));
			FontHolder_init(new, current->deepness + 1);
			current->next = new;
		}
		current = current->next;
	}

	current->fonts[current->count].ttf_fonts[init_size] = font;
	current->fonts[current->count].init_size = init_size;
	current->count++;

	return current->deepness * SDLCLAY_FONT_HOLDER_CAPACITY + current->count - 1;
}

TTF_Font* SDLCLAY_GetFont(const int font_index, const int size) {
	const struct FontsHolder* current = &FONTS_HOLDER;
	const int deepness = font_index / SDLCLAY_FONT_HOLDER_CAPACITY;
	const int index = font_index % SDLCLAY_FONT_HOLDER_CAPACITY;
	for (int i = 0; i < deepness; i++) {
		current = current->next;
	}
	Font* font = &current->fonts[index];

	if (font->ttf_fonts[size] == NULL) {
		font->ttf_fonts[size] = TTF_CopyFont(font->ttf_fonts[font->init_size]);
		TTF_SetFontSize(font->ttf_fonts[size], (float)size);
	}

	return font->ttf_fonts[size];
}

Clay_Dimensions SDLCLAY_MeasureText(
	Clay_StringSlice text,
	Clay_TextElementConfig* config,
	void* userData
) {
	TTF_Font* font = SDLCLAY_GetFont(config->fontId, config->fontSize);
	const int height = TTF_GetFontHeight(font);
	int width = 0;

	TTF_MeasureString(font, text.chars, text.length, 0, &width, NULL);

	const Clay_Dimensions result = {
		.height = (float) height,
		.width = (float) width,
	};

	return result;
}

// ===================================================================================
// MARK: RENDER
// ===================================================================================

static void SDLCLAY_SetRenderDrawColor(SDL_Renderer* renderer, const Clay_Color color) {
	SDL_SetRenderDrawColor(renderer, (int) color.r, (int) color.g, (int) color.b, (int) color.a);
}

static void SDLCLAY_RenderFillRoundedRect(
	SDL_Renderer* renderer,
	const SDL_FRect rect,
	const float cornerRadius,
	const Clay_Color _color
) {
	const SDL_FColor color = {_color.r / 255, _color.g / 255, _color.b / 255, _color.a / 255};

	int indexCount = 0, vertexCount = 0;

	const float minRadius = SDL_min(rect.w, rect.h) / 2.0f;
	const float clampedRadius = SDL_min(cornerRadius, minRadius);

	const int numCircleSegments = SDL_max(SDLCLAY_NUM_SEGMENT_CORNER, (int) clampedRadius * 0.5f);

	int totalVertices = 4 + 4 * (numCircleSegments * 2) + 2 * 4;
	int totalIndices = 6 + 4 * (numCircleSegments * 3) + 6 * 4;

	SDL_Vertex vertices[totalVertices];
	int indices[totalIndices];

	//define center rectangle
	vertices[vertexCount++] = (SDL_Vertex){
		{rect.x + clampedRadius, rect.y + clampedRadius}, color, {0, 0}
	};
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


	// TODO: Find why this is needed
	for (int i = 0; i < vertexCount; i++) {
		if (vertices[i].position.y > rect.h) vertices[i].position.y -= 1;
	}

	// Render everything
	SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);
}


static void SDLCLAY_RenderRoundedBorder(
	SDL_Renderer* renderer,
	SDL_Texture* target,
	const SDL_FRect base_rect,
	const float corner_radius,
	const float border_width,
	const Clay_Color color
) {
	const bool rounded = corner_radius > 0;

	if (!rounded) {
		SDLCLAY_SetRenderDrawColor(renderer, color);
		SDL_RenderRect(renderer, &base_rect);
		return;
	}

	SDL_Texture * new_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)base_rect.w, (int)base_rect.h);
	SDL_SetRenderTarget(renderer, new_target);
	SDL_SetRenderDrawColor(renderer, 0,0,0,0);
	SDL_RenderClear(renderer);

	const SDL_FRect outer_rect = {
		0,
		0,
		base_rect.w,
		base_rect.h
	};

	SDLCLAY_RenderFillRoundedRect(renderer, outer_rect, corner_radius, color);

	const SDL_FRect inner_rect = {
		0 + border_width,
		0 + border_width,
		base_rect.w - border_width * 2,
		base_rect.h - border_width * 2
	};

	SDLCLAY_RenderFillRoundedRect(renderer, inner_rect, corner_radius, (Clay_Color){255, 0, 255, 0});

	SDL_SetRenderTarget(renderer, target);
	SDL_RenderTexture(renderer, new_target, NULL, &base_rect);
	SDL_DestroyTexture(new_target);
}

void SDLCLAY_RenderCommands(SDL_Renderer* renderer, Clay_RenderCommandArray* commands_array) {
	// Get Current Renderer size
	int w = 0, h = 0;
	SDL_GetCurrentRenderOutputSize(renderer, &w, &h);

	// Create a texture as target
	SDL_Texture* texture_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	SDL_SetRenderTarget(renderer,texture_target);

	// Clear
	SDL_SetRenderDrawColor(renderer, 0,0,0,0);
	SDL_RenderClear(renderer);

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
				SDLCLAY_SetRenderDrawColor(renderer, config->backgroundColor);
				SDL_BlendMode blendMode = {0};
				SDL_GetRenderDrawBlendMode(renderer, &blendMode);
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				if (config->cornerRadius.topLeft > 0) {
					SDLCLAY_RenderFillRoundedRect(renderer, f_rect, config->cornerRadius.topLeft, config->backgroundColor);
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

				SDL_Surface* surface = TTF_RenderText_Blended(
					SDLCLAY_GetFont(config->fontId, config->fontSize),
					config->stringContents.chars,
					config->stringContents.length,
					color
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
				const Clay_BorderRenderData* config = &render_command->renderData.border;
				SDLCLAY_RenderRoundedBorder(
					renderer,
					texture_target,
					f_rect,
					config->cornerRadius.topLeft,
					config->width.top,
					config->color
				);
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
				SDLCLAY_LOG("Unknown render command type: %d", render_command->commandType);
		}
	}

	SDL_BlendMode blend_mode = {0};
	SDL_GetRenderDrawBlendMode(renderer, &blend_mode);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderTexture(renderer, texture_target, NULL, NULL);
	SDL_DestroyTexture(texture_target);

	SDL_SetRenderDrawBlendMode(renderer, blend_mode);
}

// ===================================================================================
// MARK: CORE
// ===================================================================================

void SDLCLAY_SetLogger(const SDLCLAY_Fun_Logger logger) {
	SDLCLAY_LOG = logger;
}

void SDLCLAY_SetAllocator(const SDLCLAY_Fun_Malloc fun_malloc, const SDLCLAY_Fun_Free fun_free) {
	SDLCLAY_MALLOC = fun_malloc;
	SDLCLAY_FREE = fun_free;
}

void SDLCLAY_Quit() {
	FontHolder_free(&FONTS_HOLDER);
}