#include "../../include/element/texture.h"

// ---------------------------------------------------------------------

static oh_texture 	*oh_texture_map = NULL;

static uint32_t 	oh_capacity = 0;
static uint32_t 	oh_size = OH_ELEMENT_TEXTURE_COUNT;

// Paths mapped by oh_element_texture_type
static const char *path[OH_ELEMENT_TEXTURE_COUNT] = {
	"./resources/overhead.png",
	"./resources/knob.png",
	"./resources/actions.png",
	"./resources/minimize.png",
	"./resources/option.png",
	"./resources/option_header.png",
	"./resources/win_200x200.png",
	"./resources/win_300x300.png",
	"./resources/win_400x400.png",
	"./resources/win_600x600.png",
	"./resources/win_800x800.png",
	"./resources/win_1200x670.png",
	"./resources/win_1800x1000.png",
	"./resources/text_box.png",
	"./resources/text_box_big.png",
	"./resources/text_line.png",
	"./resources/text_line_big.png",
};

// ---------------------------------------------------------------------

int oh_element_texture_init() {
	// Check to see if function already called
	if(oh_capacity != 0) {
		oh_log(OH_LOG_WARN, "oh_element_texture_init(): Already initialized textures");
		return OH_FALSE;
	}

	// Calculating capacity
	oh_capacity = (uint32_t) oh_utils_math_nextpow2(oh_size);

	// Allocating memory
	oh_texture_map = calloc(oh_capacity, sizeof(oh_texture));

	// On failure
	if(oh_texture_map == NULL) {
		oh_log(OH_LOG_FATAL, "oh_element_texture_init(): Initial allocation for texture map failed");
		return OH_FALSE;
	}

	// Windows
	// TODO: Check IMG_Load and SDL_CreateTextureFromSurface
	for(uint32_t i = 0; i < OH_ELEMENT_TEXTURE_COUNT; i ++) {
		oh_texture_map[i].surface = IMG_Load(path[i]);
		oh_texture_map[i].texture = SDL_CreateTextureFromSurface(oh_dependencies_get_renderer(), oh_texture_map[i].surface);
	}

	return OH_TRUE;
}

oh_texture *oh_element_texture(oh_element_texture_type type) {
	return oh_texture_map + type;
}
