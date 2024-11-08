#include "../../include/map/map.h"

// --------------------------------------------------------------------------------------------------------------------------------------

// Static structure for the map
static struct {
	// Worksheet buffer
	oh_worksheet *worksheet;
	uint32_t size;
	uint32_t cap;

	// Conncetions based on indices in the ws buffer: out -> in
	// They share the same size and cap
	uint32_t *ws_out;
	uint32_t *ws_in;

	// Positions of worksheets
	// They share the same size and cap
	int32_t *ws_x;
	int32_t *ws_y;
} oh_map;

// --------------------------------------------------------------------------------------------------------------------------------------
