#ifndef OH_WORKSHEET_GRID_H
#define OH_WORKSHEET_GRID_H

// ---------------------------------------------------------------------

// Dependencies
#include "../dependencies.h"

// ---------------------------------------------------------------------

// Default unit and bounds
#define OH_WORKSHEET_GRID_UNIT 40
#define OH_WORKSHEET_GRID_UNIT_MIN 8
#define OH_WORKSHEET_GRID_UNIT_MAX 240

// Default width and bounds
#define OH_WORKSHEET_GRID_WIDTH 1
#define OH_WORKSHEET_GRID_WIDTH_MIN 1
#define OH_WORKSHEET_GRID_WIDTH_MAX 12

// ---------------------------------------------------------------------

// Variables of having a grid:
// Unit (Tells the space between grid lines)
// Width (How thick the grid lines are, depending on worksheet in cause)
// Canvas position (Tells where the center is in the coords system)

// ---------------------------------------------------------------------

// Set the unit and width to be used from now on
int 	oh_worksheet_grid_set_unit(int32_t u);
// For w = 1px, will render a SDL line, instead of filled rect
int 	oh_worksheet_grid_set_width(int32_t w);
// Set the color of the grid
void 	oh_worksheet_grid_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Get unit
int oh_worksheet_grid_get_unit();
// Get width
int oh_worksheet_grid_get_width();
// Get colors
int oh_worksheet_grid_get_color(int *r, int *g, int *b, int *a);

// Render the grid (x, y are the canvas position)
void oh_worksheet_grid_render();

#endif
