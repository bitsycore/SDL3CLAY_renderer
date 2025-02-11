#include "SDL3CLAY.h"

static SDLCLAY_Fun_Logger SDLCLAY_LOG = SDL_Log;
static SDLCLAY_Fun_Malloc SDLCLAY_MALLOC = SDL_malloc;
static SDLCLAY_Fun_Free SDLCLAY_FREE = SDL_free;

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
	const float corner_radius,
	const Clay_Color clay_color
) {
	const SDL_FColor color = {clay_color.r / 255, clay_color.g / 255, clay_color.b / 255, clay_color.a / 255};

	int index_count = 0, vertex_count = 0;

	const float min_radius = SDL_min(rect.w, rect.h) / 2.0f;
	const float clamp_radius = SDL_min(corner_radius, min_radius);

	const int num_circle_segments = SDL_max(SDLCLAY_NUM_SEGMENT_CORNER, (int) clamp_radius * 0.5f);
	const int total_vertices = 4 + 4 * (num_circle_segments * 2) + 2 * 4;
	const int total_indices = 6 + 4 * (num_circle_segments * 3) + 6 * 4;

	SDL_Vertex vertices[total_vertices];
	int indices[total_indices];

	// ==================================
	// Define center rectangle
	// ==================================

	// [0] Center Top Left
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + clamp_radius,
			rect.y + clamp_radius
		},
		color,
		{0, 0}
	};
	// [1] Center Top Right
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w - clamp_radius,
			rect.y + clamp_radius
		},
		color,
		{1, 0}
	};
	// [2] Center Bottom Right
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w - clamp_radius,
			rect.y + rect.h - clamp_radius
		},
		color,
		{1, 1}
	};
	// [3] Center Bottom Left
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + clamp_radius,
			rect.y + rect.h - clamp_radius
		},
		color,
		{0, 1}
	};

	indices[index_count++] = 0;
	indices[index_count++] = 1;
	indices[index_count++] = 3;
	indices[index_count++] = 1;
	indices[index_count++] = 2;
	indices[index_count++] = 3;

	// ==================================
	// Define rounded corners as triangle fans
	// ==================================

	const float step = SDL_PI_F / 2.0f / (float) num_circle_segments;

	for (int i = 0; i < num_circle_segments; i++) {

		const float angle1 = (float) i * step;
		const float angle2 = (float) (i + 1) * step;

		// Iterate over four corners
		for (int j = 0; j < 4; j++) {
			float cx, cy, sign_x, sign_y;

			switch (j) {
				case 0: // Top-left
					cx = rect.x + clamp_radius;
					cy = rect.y + clamp_radius;
					sign_x = -1;
					sign_y = -1;
					break;

				case 1: // Top-right
					cx = rect.x + rect.w - clamp_radius;
					cy = rect.y + clamp_radius;
					sign_x = 1;
					sign_y = -1;
					break;
				case 2: // Bottom-right
					cx = rect.x + rect.w - clamp_radius;
					cy = rect.y + rect.h - clamp_radius;
					sign_x = 1;
					sign_y = 1;
					break;
				case 3: // Bottom-left
					cx = rect.x + clamp_radius;
					cy = rect.y + rect.h - clamp_radius;
					sign_x = -1;
					sign_y = 1;
					break;
				default:
					break;
			}

			vertices[vertex_count++] = (SDL_Vertex){
				{
					cx + SDL_cosf(angle1) * clamp_radius * sign_x,
					cy + SDL_sinf(angle1) * clamp_radius * sign_y
				},
				color,
				{0, 0}
			};

			vertices[vertex_count++] = (SDL_Vertex){
				{
					cx + SDL_cosf(angle2) * clamp_radius * sign_x,
					cy + SDL_sinf(angle2) * clamp_radius * sign_y
				},
				color,
				{0, 0}
			};

			// Connect to corresponding central rectangle vertex
			indices[index_count++] = j;
			indices[index_count++] = vertex_count - 2;
			indices[index_count++] = vertex_count - 1;
		}
	}

	// ==================================
	// Define edges rectangles
	// ==================================

	// ==================================
	// Top
	// ==================================
	// Top Left
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + clamp_radius,
			rect.y
		},
		color,
		{0, 0}
	};
	// Top Right
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w - clamp_radius,
			rect.y
		},
		color,
		{1, 0}
	};

	indices[index_count++] = 0;
	indices[index_count++] = vertex_count - 2; //TL
	indices[index_count++] = vertex_count - 1; //TR
	indices[index_count++] = 1;
	indices[index_count++] = 0;
	indices[index_count++] = vertex_count - 1; //TR

	// ==================================
	// Right
	// ==================================
	// Right Top
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w,
			rect.y + clamp_radius
		},
		color,
		{1, 0}
	};
	// Right Bottom
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w,
			rect.y + rect.h - clamp_radius
		},
		color,
		{1, 1}
	};

	indices[index_count++] = 1;
	indices[index_count++] = vertex_count - 2; //RT
	indices[index_count++] = vertex_count - 1; //RB
	indices[index_count++] = 2;
	indices[index_count++] = 1;
	indices[index_count++] = vertex_count - 1; //RB

	// ==================================
	// Bottom
	// ==================================
	// Bottom Right
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + rect.w - clamp_radius,
			rect.y + rect.h
		},
		color,
		{1, 1}
	};
	// Bottom Left
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x + clamp_radius,
			rect.y + rect.h
		},
		color,
		{0, 1}
	};

	indices[index_count++] = 2;
	indices[index_count++] = vertex_count - 2; //BR
	indices[index_count++] = vertex_count - 1; //BL
	indices[index_count++] = 3;
	indices[index_count++] = 2;
	indices[index_count++] = vertex_count - 1; //BL

	// ==================================
	// Left
	// ==================================
	// Left Bottom
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x,
			rect.y + rect.h - clamp_radius
		},
		color,
		{0, 1}
	};
	// Left Top
	vertices[vertex_count++] = (SDL_Vertex){
		{
			rect.x,
			rect.y + clamp_radius
		},
		color,
		{0, 0}
	};

	indices[index_count++] = 3;
	indices[index_count++] = vertex_count - 2; //LB
	indices[index_count++] = vertex_count - 1; //LT
	indices[index_count++] = 0;
	indices[index_count++] = 3;
	indices[index_count++] = vertex_count - 1; //LT

	// ==================================
	// Draw
	// ==================================

	// TODO: Temp fix, Find why this is needed and it still look bad
	for (int i = 0; i < vertex_count; i++) {
		if (vertices[i].position.y > rect.h) vertices[i].position.y -= 1;
	}

	SDL_RenderGeometry(renderer, NULL, vertices, vertex_count, indices, index_count);
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
// MARK: Misc
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