#ifndef OH_CONTROL_H
#define OH_CONTROL_H

// ---------------------------------------------------------------------

// Standard
#include <stdint.h>

// Logging
#include "../include/utils/log.h"

// Dependencies
#include "../include/dependencies.h"

// Element
#include "../include/element/texture.h"
#include "../include/element/element.h"

// Worksheet
#include "../include/worksheet/worksheet.h"
#include "../include/worksheet/grid.h"

// ---------------------------------------------------------------------

// This is the main scene that has to be defined in your main source file
extern int32_t oh_init();		// If it returns OH_FALSE,
//								nothing else will be called
extern int32_t oh_event(SDL_Event event);
extern int32_t oh_update();
extern int32_t oh_cleanup();

// ---------------------------------------------------------------------

// Change the scene
void oh_control_set_scene(
	int32_t (*init)(),
	int32_t (*event)(SDL_Event event),
	int32_t (*update)(),
	int32_t (*cleanup)());

// Returns the state of the program (running or not)
int32_t oh_control_state();

// Stops the program
void oh_control_stop();

// Set the canvas position
void oh_control_set_x(int32_t x);
void oh_control_set_y(int32_t y);

// Returns the canvas position
int32_t oh_control_x();
int32_t oh_control_y();

// Returns mouse position in infinite grid
int32_t oh_control_cursor_x();
int32_t oh_control_cursor_y();

// Set a worksheet to work on
int32_t oh_control_set_worksheet(oh_worksheet *ws);

// ---------------------------------------------------------------------

int32_t oh_control_worksheet_update_states();

void oh_control_set_knob_sen(float sen);

int32_t oh_control_get_toggle();

#endif
