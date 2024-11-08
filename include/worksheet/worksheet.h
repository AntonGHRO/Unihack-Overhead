#ifndef OH_WORKSHEET_H
#define OH_WORKSHEET_H

// ---------------------------------------------------------------------

// Utils
#include "../utils/log.h"
#include "../utils/math.h"

// Dependencies
#include "../dependencies.h"

// Element
#include "../element/element.h"

// ---------------------------------------------------------------------

#define OH_WORKSHEET_ELEMENT_BUFFER_CAP 64

// ---------------------------------------------------------------------
// [IMPORTANT] Structure that remembers the "state" of every worksheet
// 
typedef struct {
	// Name, description
	char *name;
	char *desc;

	// TODO: Interface to pass to "context" module for the "map" module

	// Static elements buffer
	oh_element *static_element;
	uint32_t static_size;
	uint32_t static_cap;

	// Dynamic elements buffer
	oh_element *dynamic_element;
	uint32_t dynamic_size;
	uint32_t dynamic_cap;

	// x, y - refers to the canvas position
	// It is remembered even after closing
	int32_t x;
	int32_t y;

	// A unit is the distance in pixels between two grid lines
	int32_t unit; // Has nothing to do with zooming

	// Grid width is dependent on the worksheet in cause
	// Measured in pixels (default = 1px)
	int32_t grid_width;
} oh_worksheet;

// ---------------------------------------------------------------------
// Initializes a worksheet (should comunicate with "map")
int32_t oh_worksheet_init(oh_worksheet *ws, const char *name);

// Create an element in the worksheet
int32_t oh_worksheet_create_element(
	oh_worksheet *ws,
	oh_element *snap,
	oh_element_texture_type texture_type,
	uint8_t activity);

#endif
