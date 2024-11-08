#ifndef OH_CONTEXT_H
#define OH_CONTEXT_H

// ---------------------------------------------------------------------

// Standard
#include <stdint.h>

// Map and worksheet
#include "worksheet/worksheet.h"
// #include "map/map.h"

// ---------------------------------------------------------------------

#define OH_CONTEXT_WORKSHEET 	0
#define OH_CONTEXT_MAP 			1

// ---------------------------------------------------------------------

// Returns the OH_CONTEXT_WORKSHEET or OH_CONTEXT_MAP
int32_t oh_context_state();

#endif
