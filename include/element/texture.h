#ifndef OH_ELEMENT_TEXTURE_H
#define OH_ELEMENT_TEXTURE_H

// ---------------------------------------------------------------------

#include "../dependencies.h"

// ---------------------------------------------------------------------

// Enum for mapping the textures
typedef enum {
	OH_ELEMENT_TEXTURE_OVERHEAD,
	OH_ELEMENT_TEXTURE_KNOB,
	OH_ELEMENT_TEXTURE_ACTIONS,
	OH_ELEMENT_TEXTURE_MINIMIZE,
	OH_ELEMENT_TEXTURE_OPTION,
	OH_ELEMENT_TEXTURE_OPTION_HEADER,
	OH_ELEMENT_TEXTURE_WIN_200x200,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_300x300,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_400x400,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_600x600,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_800x800,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_1200x670,  		// Has interaction box
	OH_ELEMENT_TEXTURE_WIN_1800x1000,  		// Has interaction box
	OH_ELEMENT_TEXTURE_COUNT,
} oh_element_texture_type;

// Small wrapper around SDL_Surface and SDL_Texture
// Reason: To allow for the using of the same texture
// by multiple elements
typedef struct {
	SDL_Surface *surface;
	SDL_Texture *texture;
} oh_texture;

// ---------------------------------------------------------------------

// Initialize all oh_textures from the given path
// Should exit on OH_FALSE
int oh_element_texture_init();

// Returns the oh_texture mapped by type
oh_texture *oh_element_texture(oh_element_texture_type type);

// ---------------------------------------------------------------------

#endif
