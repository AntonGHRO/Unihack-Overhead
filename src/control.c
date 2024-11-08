#include "../include/control.h"
#include <stdlib.h>

// --------------------------------------------------------------------------------------------------------------------------------------

// Structure of a scene
// init 	= initializes the scene
// event 	= handle input from keyboard etc
// update 	= copy to render
// cealnup 	= free data
static struct {
	int32_t (*init)();
	int32_t (*event)(SDL_Event event);
	int32_t (*update)();
	int32_t (*cleanup)();
} oh_control_scene;

// --------------------------------------------------------------------------------------------------------------------------------------

static int32_t oh_control_running = 1;

static int32_t oh_x = 0;
static int32_t oh_y = 0;

// --------------------------------------------------------------------------------------------------------------------------------------

void oh_control_set_scene(
	int32_t (*init)(),
	int32_t (*event)(SDL_Event event),
	int32_t (*update)(),
	int32_t (*cleanup)())
{
	// Cleanup the old scene
	oh_control_scene.cleanup();

	// Set the current one
    oh_control_scene.init = init;
    oh_control_scene.event = event;
    oh_control_scene.update = update;
    oh_control_scene.cleanup = cleanup;

    // Initialize the other one
    oh_control_scene.init();
}

// --------------------------------------------------------------------------------------------------------------------------------------

// Main entry point
int32_t main(void) {
	// Initialize the dependencies, the window & renderer
	if(oh_dependencies_init() == OH_FALSE) {
		exit(EXIT_FAILURE);
		// TODO: Control should have onFatal()...
	}

	// Initialize textures
	if(oh_element_texture_init() == OH_FALSE) {
		exit(EXIT_FAILURE);
	}

	int32_t moving = 0;
	int32_t last_x = 0;
	int32_t last_y = 0;

	oh_x = - oh_dependencies_get_display_mode().w / 2;
	oh_y = - oh_dependencies_get_display_mode().h / 2;

	// Call user entry point
	oh_control_scene.init = oh_init;
	oh_control_scene.event = oh_event;
	oh_control_scene.update = oh_update;
	oh_control_scene.cleanup = oh_cleanup;

	// Call init
	if(oh_control_scene.init() == OH_FALSE) {
		oh_log(OH_LOG_FATAL, "main(): oh_init() returned OH_FALSE");
		exit(EXIT_FAILURE);
	}

	// Main loop
	SDL_Event event;

	while(oh_control_state()) {
		// Poll event
		while(SDL_PollEvent(oh_dependencies_get_event())) {
			event = *oh_dependencies_get_event();

			// For safety, exit upon pressing Q or if SDL wants to
	    	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
	            oh_control_stop();
	    	}

	    	// Move canvas
	    	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
	    		moving = 1;
	    		last_x = event.button.x;
	    		last_y = event.button.y;
	    	}

	    	if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
	    		moving = 0;
	    	}

	    	if(event.type == SDL_MOUSEMOTION && moving) {
	    		oh_x -= (event.motion.x - last_x);
	    		oh_y -= (event.motion.y - last_y);

	    		last_x = event.motion.x;
	    		last_y = event.motion.y;
	    	}

	    	// Check event user
	    	if(oh_control_scene.event(event) == OH_FALSE) {
	    		oh_log(OH_LOG_ERROR, "main(): oh_control_scene.event() failed");
	    	}
        }

        // Clear screen
		SDL_SetRenderDrawColor(
			oh_dependencies_get_renderer(),
			OH_DEPENDENCIES_BACKGROUND_R, OH_DEPENDENCIES_BACKGROUND_G, OH_DEPENDENCIES_BACKGROUND_B,
			255);

        SDL_RenderClear(oh_dependencies_get_renderer());

        // Rendering starts here --------------------------------------------------------------------------------------

        // Only for worksheet context...
        oh_worksheet_grid_render();

        // User rendering
        if(oh_control_scene.update() == OH_FALSE) {
	    	oh_log(OH_LOG_ERROR, "main(): oh_control_scene.update() failed. Stopping");
	    	oh_control_stop();
        }

        // Rendering ends here ----------------------------------------------------------------------------------------

        // Update screen
        SDL_RenderPresent(oh_dependencies_get_renderer());
	}

	// User cleanup
	if(oh_control_scene.cleanup() == OH_FALSE) {
		oh_log(OH_LOG_ERROR, "main(): oh_control_scene.cleanup() failed");
	}

	// Dependencies free
	oh_dependencies_quit();
	return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------------

int32_t oh_control_state() {
	return oh_control_running;
}

void oh_control_stop() {
	oh_control_running = 0;
}

void oh_control_set_x(int32_t x) {
	oh_x = x;
}

void oh_control_set_y(int32_t y) {
	oh_y = y;
}

int32_t oh_control_x() { return oh_x; }
int32_t oh_control_y() { return oh_y; }

// --------------------------------------------------------------------------------------------------------------------------------------
