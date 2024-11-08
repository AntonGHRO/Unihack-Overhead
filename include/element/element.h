#ifndef OH_ELEMENT_H
#define OH_ELEMENT_H

// ---------------------------------------------------------------------

#include "../dependencies.h"
#include "texture.h"

// ---------------------------------------------------------------------

#define OH_ELEMENT_ACTIVITY_STATIC 	0
#define OH_ELEMENT_ACTIVITY_DYNAMIC 1

// ---------------------------------------------------------------------
// Enum to determine the state of interaction of the element
typedef enum {
	OH_ELEMENT_NORMAL,			// Cursor not on top of element
	OH_ELEMENT_HOVER,			// Cursor is on top of element
	OH_ELEMENT_MOUSE_DOWN,		// SDL_MOUSEBUTTONDOWN
	OH_ELEMENT_MOUSE_HOLD,		// SDL_MOUSEMOTION
	OH_ELEMENT_MOUSE_UP,		// SDL_MOUSEBUTTONUP
} oh_element_state;

// Element (static - cannot interact with, dynamic - can interact with)
typedef struct oh_element oh_element;
struct oh_element {
	// An element can be snapped to another element (knob to a window)
	oh_element *snap;

	// Keeps track of snapped elements
	oh_element **snapped;
	uint32_t snapped_count;

	// Texture
	oh_texture *texture;

	// Interaction area
	// x,y - offset from the current position
	// w,h - size of the interaction area
	SDL_Rect interact;

	// Position (if element is snapped, it's just an offset)
	SDL_Point position;

	// Offset from the snapped element
	SDL_Point snap_offset;

	// State
	oh_element_state state;

	// Angle
	double angle;

	// Activity flag (0 - Static, 1 - Dynamic)
	uint8_t activity;
};

// ---------------------------------------------------------------------

// Initializes an interface element
// activity - OH_ELEMENT_STATIC / OH_ELEMENT_DYNAMIC
// snap - can be set to NULL
int32_t oh_element_init(
	oh_element *element,
	oh_element *snap,
	oh_element_texture_type texture_type,
	uint8_t activity);

// Similar to oh_element_init() but also allocates the element on the heap
// oh_element *oh_element_get(
// 	oh_element *snap,
// 	oh_element_texture_type texture_type,
// 	uint8_t activity);

// snap_back = 0 -> Will clear the element
// 					and all the other elements snapped to it
// snap_back = 1 -> Will clear the element
// 					and snap the elements (snapped to it) to the back
// 					if the element had a snap
// int32_t oh_element_clear(oh_element *element, int32_t snap_back);

// Similar to oh_element_clear() but also calls free on the elements
// int32_t oh_element_free(oh_element *element, int32_t snap_back);

// Renders an element
int32_t oh_element_render(oh_element *element);

// Setters
int32_t oh_element_set_position 	(oh_element *element,int32_t x, int32_t y);
int32_t oh_element_set_snap_offset 	(oh_element *element, int32_t x, int32_t y);
void 	oh_element_set_interaction 	(oh_element *element, int32_t x, int32_t y, int32_t w, int32_t h);
void 	oh_element_set_angle 		(oh_element *element, double angle);

// ---------------------------------------------------------------------

#endif
