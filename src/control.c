#include "../include/control.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <string.h>

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

static int32_t is_allowed_key(SDL_Keycode key) {
    // Check if the key is a letter (A-Z or a-z)
    if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
        return 1;
    }

    // Check if the key is one of the allowed symbols
    switch (key) {
        case '!': case '@': case '#': case '$': case '%': case '^': case '&': case '*': case '(': case ')':
        case '_': case '+': case '-': case '=': case ':': case ';': case '\'': case ',': case '.': case '<':
        case '>': case '/': case '?': case '`': case '~': case '[': case ']': case '{': case '}': case '\\':
        case '|': case ' ':
            return 1;
    }

    // If the key is not in the allowed range, return 0
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------------

static int32_t oh_control_running = 1;

static int32_t oh_x = 0;
static int32_t oh_y = 0;

static int32_t oh_cursor_x = 0;
static int32_t oh_cursor_y = 0;

static oh_worksheet *worksheet = NULL;

static int32_t moving = 0;
static int32_t moving_element = 0;
static int32_t last_x = 0;
static int32_t last_y = 0;
static int32_t last_x_element = 0;
static int32_t last_y_element = 0;

static int32_t control_down = 0;

static int32_t knob_rotate = 0;
static double knob_angle = 0.0;

uint8_t oh_color_mod_r = 255;
uint8_t oh_color_mod_g = 255;
uint8_t oh_color_mod_b = 255;

static oh_element *write_to = NULL;

static float knob_sen = 1.0;

static int32_t toggle = 0;
static int32_t button = 0;

// --------------------------------------------------------------------------------------------------------------------------------------

void oh_control_set_knob_sen(float sen) {
	knob_sen = sen;
}

int32_t oh_control_get_toggle() {
	return toggle;
}

int32_t oh_control_get_button() {
	return button;
}

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
	    	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q && control_down)) {
	            oh_control_stop();
	    	}

	    	if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LCTRL) {
	    		control_down = 1;
	    	}

	    	if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_LCTRL) {
	    		control_down = 0;
	    	}

	    	if(event.type == SDL_KEYDOWN && write_to != NULL && is_allowed_key(event.key.keysym.sym)) {
	    		char str[1024] = {0};
	    		strcpy(str, write_to->param_str->data);
	    		int32_t last = strlen(str);
	    		str[last] = event.key.keysym.sym;
	    		oh_element_param_str_set_str(write_to->param_str, str);
	    	}

	    	if(event.type == SDL_KEYDOWN && write_to != NULL && event.key.keysym.sym == SDLK_BACKSPACE) {
	    		if(strlen(write_to->param_str->data) > 1) {
	    			char str[1024] = {0};
	    			strncpy(str, write_to->param_str->data, strlen(write_to->param_str->data) - 1);
	    			oh_element_param_str_set_str(write_to->param_str, str);
	    		}
	    	}

	    	// Move canvas
	    	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
	    		moving = 1;
	    		last_x = event.button.x;
	    		last_y = event.button.y;
	    	}

	    	// Not move Canvas
	    	if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
	    		moving = 0;
	    	}

	    	// Move element
	    	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && control_down) {
	    		last_x = event.button.x;
	    		last_y = event.button.y;

	    		if(worksheet != NULL) {
	    			if(worksheet->hover != NULL) {
	    				moving_element = 1;

	    				last_x_element = worksheet->hover->position.x + worksheet->hover->interact.x;
	    				last_y_element = worksheet->hover->position.y + worksheet->hover->interact.y;
	    			}
	    		}
	    	}

	    	// Click element
	    	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !control_down) {
	    		last_x = event.button.x;
	    		last_y = event.button.y;

	    		if(write_to != NULL) {
	    			oh_element_set_transparent(write_to, 1);
	    			write_to = NULL;
	    		}

	    		if(worksheet != NULL) {
	    			if(worksheet->hover != NULL) {
	    				if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_KNOB) {
	    					knob_rotate = 1;
	    					knob_angle = worksheet->hover->angle;
	    				} else if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_TEXT_LINE || worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_TEXT_LINE_BIG || worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_TEXT_LINE_HUGE) {
	    					write_to = worksheet->hover;
	    					oh_element_set_transparent(write_to, 0);
	    				} else if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_TOGGLE_OFF) {
	    					toggle = 1;
	    					worksheet->hover->texture = oh_element_texture(OH_ELEMENT_TEXTURE_TOGGLE_ON);
	    					worksheet->hover->texture_type = OH_ELEMENT_TEXTURE_TOGGLE_ON;
	    				} else if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_TOGGLE_ON) {
	    					toggle = 0;
	    					worksheet->hover->texture = oh_element_texture(OH_ELEMENT_TEXTURE_TOGGLE_OFF);
	    					worksheet->hover->texture_type = OH_ELEMENT_TEXTURE_TOGGLE_OFF;
	    				} else if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_BUTTON_OFF) {
	    					button = 1;
	    					worksheet->hover->texture = oh_element_texture(OH_ELEMENT_TEXTURE_BUTTON_ON);
	    					worksheet->hover->texture_type = OH_ELEMENT_TEXTURE_BUTTON_ON;
	    				} else if(worksheet->hover->texture_type == OH_ELEMENT_TEXTURE_BUTTON_ON) {
	    					button = 0;
	    					worksheet->hover->texture = oh_element_texture(OH_ELEMENT_TEXTURE_BUTTON_OFF);
	    					worksheet->hover->texture_type = OH_ELEMENT_TEXTURE_BUTTON_OFF;
	    				}
	    			}
	    		}
	    	}

	    	// Not move element
	    	if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
	    		if(worksheet != NULL) {
	    			if(worksheet->hover != NULL) {
	    				moving_element = 0;
	    				knob_rotate = 0;
	    			}
	    		}
	    	}

	    	// Update motion
	    	if(event.type == SDL_MOUSEMOTION) {
	    		oh_cursor_x = event.motion.x;
	    		oh_cursor_y = event.motion.y;

	    		if(moving) {
	    			oh_x -= (oh_cursor_x - last_x);
		    		oh_y -= (oh_cursor_y - last_y);

		    		last_x = event.motion.x;
		    		last_y = event.motion.y;
	    		} else if(moving_element && worksheet->hover != NULL) {
	    			oh_element_set_position(
	    				worksheet->hover,
	    				oh_cursor_x + oh_x - worksheet->hover->interact.x - (last_x + oh_x - last_x_element),
	    				oh_cursor_y + oh_y - worksheet->hover->interact.y - (last_y + oh_y - last_y_element)
					);
	    		} else if(knob_rotate && worksheet->hover != NULL) {
	    			oh_element_set_angle(worksheet->hover, knob_angle + knob_sen * (float)(oh_cursor_x - last_x));

	    			if(worksheet->hover->param != NULL) {
	    				oh_element_param_set_val(worksheet->hover->param, worksheet->hover->angle * 0.1);
	    			}
	    		}
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

        // Worksheet update states
        if(worksheet != NULL) {
        	oh_control_worksheet_update_states();
        }

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

int32_t oh_control_cursor_x() {
	return oh_cursor_x + oh_x;
}

int32_t oh_control_cursor_y() {
	return oh_cursor_y + oh_y;
}

int32_t oh_control_set_worksheet(oh_worksheet *ws) {
	// if(ws == NULL) {
	// 	oh_log(OH_LOG_ERROR, "oh_control_set_worksheet(): Passed NULL worksheet");
	// 	return OH_FALSE;
	// }

	worksheet = ws;

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------

int32_t oh_control_worksheet_update_states() {
	if(worksheet == NULL) {
		oh_log(OH_LOG_ERROR, "oh_control_worksheet_update_states(): No worksheet to work on");
		return OH_FALSE;
	}

	for(int32_t i = worksheet->dynamic_size - 1; i >= 0; i --) {
		if(oh_element_is_inside(worksheet->dynamic_element + i, oh_control_cursor_x(), oh_control_cursor_y())) {
			if(moving_element == 0 && knob_rotate == 0) {
				worksheet->dynamic_element[i].state = OH_ELEMENT_HOVER;
				worksheet->hover = worksheet->dynamic_element + i;
				oh_color_mod_r = 240;
				oh_color_mod_g = 240;
				oh_color_mod_b = 240;
				// SDL_SetTextureColorMod(worksheet->dynamic_element[i].texture->texture, oh_color_mod_r, oh_color_mod_r, oh_color_mod_r);
				return OH_TRUE;
			}
		}
	}

	if(worksheet->hover != NULL) {
		oh_color_mod_r = 255;
		oh_color_mod_g = 255;
		oh_color_mod_b = 255;
		// SDL_SetTextureColorMod(worksheet->hover->texture->texture, oh_color_mod_r, oh_color_mod_g, oh_color_mod_b);
	}

	if(moving_element == 0 && knob_rotate == 0) {
		worksheet->hover = NULL;
	}

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
