#include "../../include/worksheet/grid.h"

// --------------------------------------------------------------------------------------------------------------------------------------
static int32_t oh_unit = OH_WORKSHEET_GRID_UNIT;
static int32_t oh_width = OH_WORKSHEET_GRID_WIDTH;
static uint8_t oh_color_r = 0;
static uint8_t oh_color_g = 0;
static uint8_t oh_color_b = 0;
static uint8_t oh_color_a = 255;

// To avoid circular dependency hehe
extern int32_t oh_control_x();
extern int32_t oh_control_y();

// --------------------------------------------------------------------------------------------------------------------------------------
int oh_worksheet_grid_set_unit(int32_t u) {
	if(u >= OH_WORKSHEET_GRID_UNIT_MIN && u <= OH_WORKSHEET_GRID_UNIT_MAX) {
		if(
			(oh_dependencies_get_display_mode().w % u != 0)
			|| (oh_dependencies_get_display_mode().h % u != 0)
		) {
			oh_log(OH_LOG_WARN, "oh_worksheet_grid_set_unit(): Unit must be divisor of the width and height of the display mode");
			return OH_FALSE;
		}

		oh_unit = u;
		return OH_TRUE;
	} else {
		oh_log(OH_LOG_WARN,
			"oh_worksheet_grid_set_unit(): Invalid range of unit. Must be inside [%d, %d]",
			OH_WORKSHEET_GRID_UNIT_MIN, OH_WORKSHEET_GRID_UNIT_MAX);
		return OH_FALSE;
	}
}

int oh_worksheet_grid_set_width(int32_t w) {
	if(w >= OH_WORKSHEET_GRID_WIDTH_MIN && w <= OH_WORKSHEET_GRID_WIDTH_MAX) {
		oh_width = w;
		return OH_TRUE;
	} else {
		oh_log(OH_LOG_WARN,
			"oh_worksheet_grid_set_width(): Invalid range of width. Must be inside [%d, %d]",
			OH_WORKSHEET_GRID_WIDTH_MIN, OH_WORKSHEET_GRID_WIDTH_MAX);
		return OH_FALSE;
	}
}

void oh_worksheet_grid_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	oh_color_r = r;
	oh_color_g = g;
	oh_color_b = b;
	oh_color_a = a;
}

int oh_worksheet_grid_get_unit() {
	return oh_unit;
}

int oh_worksheet_grid_get_width() {
	return oh_width;
}

int oh_worksheet_grid_get_color(int *r, int *g, int *b, int *a) {
	if(r == NULL || g == NULL || b == NULL || a == NULL) {
		oh_log(OH_LOG_ERROR, "oh_worksheet_grid_get_color(): Passed NULL");
		return OH_FALSE;
	}

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------

// TODO: Should center grid lines have bigger width ?
void oh_worksheet_grid_render() {
	// Will render a few more (16) for safety
	// Tho this "safety" can be calculated depending on the unit and canvas position

	// Set the color
	SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), oh_color_r, oh_color_g, oh_color_b, oh_color_a);

	// Iterator
	int32_t i;
	SDL_Rect rect;

	int32_t x = oh_control_x();
	int32_t y = oh_control_y();

	// Gives the impression of infinite grid
	x = x % oh_unit;
	y = y % oh_unit - 20;

	// --------------------------------------------------------------------------------------------------------------------------------------
	int32_t verical_count_half = (oh_dependencies_get_display_mode().w / oh_unit) / 2 + 8;
	int32_t horizontal_offset = 0;
	int32_t horizontal_base = - x + oh_dependencies_get_display_mode().w / 2;

	int32_t y2_vertical_1 = oh_dependencies_get_display_mode().h - y + 8 * oh_unit;
	int32_t y2_vertical_0 = - y - oh_dependencies_get_display_mode().h / 2 + 8 * oh_unit;

	// Render the first one then the rest (so in total we have 2 * verical_count_half + 1)
	if(oh_width == 1) {
		SDL_RenderDrawLine(oh_dependencies_get_renderer(), horizontal_base, - 8 * oh_unit + y2_vertical_0, horizontal_base, y2_vertical_1);
	} else {
		rect.x = horizontal_base - oh_width / 2;
		rect.y = - 8 * oh_unit + y2_vertical_0;
		rect.w = oh_width;
		rect.h = y2_vertical_1 - y2_vertical_0 + 8 * oh_unit;
		SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);
	}

	// Update the horizontal_offset (the rest are rendered 2 simultaneously at distance horizontal_offset from the first)
	horizontal_offset += oh_unit;

	// Render the rest
	if(oh_width == 1) {
		for(i = 0; i < verical_count_half; i ++) {
			SDL_RenderDrawLine(oh_dependencies_get_renderer(),
				horizontal_base - horizontal_offset, - 8 * oh_unit + y2_vertical_0,
				horizontal_base - horizontal_offset, y2_vertical_1);

			SDL_RenderDrawLine(oh_dependencies_get_renderer(),
				horizontal_base + horizontal_offset, - 8 * oh_unit + y2_vertical_0 ,
				horizontal_base + horizontal_offset, y2_vertical_1);

			// oh_log(OH_LOG_WARN, "%d %d %d", oh_control_x(), horizontal_base - horizontal_offset, horizontal_base + horizontal_offset);

			horizontal_offset += oh_unit;
		}
	} else {
		// Width and height remain constant
		rect.w = oh_width;
		rect.h = y2_vertical_1 - y2_vertical_0 + 8 * oh_unit;

		for(i = 0; i < verical_count_half; i ++) {
			rect.x = horizontal_base - horizontal_offset - oh_width / 2;
			// rect.y = /*- 8 * oh_unit*/ y2_vertical_0;

			SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);

			rect.x = horizontal_base + horizontal_offset - oh_width / 2;

			SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);

			horizontal_offset += oh_unit;
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------
	int32_t horizontal_count_half = (oh_dependencies_get_display_mode().h / oh_unit) / 2 + 8;
	int32_t vertical_offset = 0;
	int32_t vertical_base = - y + oh_dependencies_get_display_mode().h / 2;

	int32_t x2_horizontal_1 = oh_dependencies_get_display_mode().w - x + 8 * oh_unit;
	int32_t x2_horizontal_0 = - x - oh_dependencies_get_display_mode().w / 2 + 8 * oh_unit;

	// Render the first one then the rest (so in total we have 2 * horizontal_count_half + 1)
	if(oh_width == 1) {
		SDL_RenderDrawLine(oh_dependencies_get_renderer(), - 8 * oh_unit + x2_horizontal_0, vertical_base, x2_horizontal_1, vertical_base);
	} else {
		rect.x = - 8 * oh_unit + x2_horizontal_0;
		rect.y = vertical_base - oh_width / 2;
		rect.w = x2_horizontal_1 - x2_horizontal_0 + 8 * oh_unit;
		rect.h = oh_width;
		SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);
	}

	// Update the vertical_offset (the rest are rendered 2 simultaneously at distance vertical_offset from the first)
	vertical_offset += oh_unit;

	// Render the rest
	if(oh_width == 1) {
		for(i = 0; i < horizontal_count_half; i ++) {
			SDL_RenderDrawLine(oh_dependencies_get_renderer(),
				- 8 * oh_unit + x2_horizontal_0, vertical_base - vertical_offset,
				x2_horizontal_1, vertical_base - vertical_offset);

			SDL_RenderDrawLine(oh_dependencies_get_renderer(),
				- 8 * oh_unit + x2_horizontal_0 , vertical_base + vertical_offset,
				x2_horizontal_1, vertical_base + vertical_offset);

			vertical_offset += oh_unit;
		}
	} else {
		rect.w = x2_horizontal_1 - x2_horizontal_0 + 8 * oh_unit;
		rect.h = oh_width;

		for(i = 0; i < horizontal_count_half; i ++) {
			// rect.x = /*- 8 * oh_unit*/ x2_horizontal_0;
			rect.y = vertical_base - vertical_offset - oh_width / 2;

			SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);

			rect.y = vertical_base + vertical_offset - oh_width / 2;

			SDL_RenderFillRect(oh_dependencies_get_renderer(), &rect);

			vertical_offset += oh_unit;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------
