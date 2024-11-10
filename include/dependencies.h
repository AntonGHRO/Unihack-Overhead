#ifndef OH_DEPENDENCIES_H
#define OH_DEPENDENCIES_H

// ---------------------------------------------------------------------
// Standard
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

// ---------------------------------------------------------------------
// GSL
// TODO: Remove SDL, use OpenBLAS
#include <gsl/gsl_math.h>
#include <gsl/gsl_pow_int.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_fft_complex.h>

// ---------------------------------------------------------------------
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// ---------------------------------------------------------------------
// Utils/log
#include "utils/log.h"
#include "utils/math.h"

// ---------------------------------------------------------------------

// Functions return {OH_TRUE / OH_FALSE}
// "Getters" return the value asked for
#define OH_TRUE 	1
#define OH_FALSE 	0

// ---------------------------------------------------------------------

// Bounds of font size
#define OH_DEPENDENCIES_FONT_SIZE_MIN 8
#define OH_DEPENDENCIES_FONT_SIZE_MAX 28

// Default background color (can change)
#define OH_DEPENDENCIES_BACKGROUND_R 255
#define OH_DEPENDENCIES_BACKGROUND_G 255
#define OH_DEPENDENCIES_BACKGROUND_B 255

// ---------------------------------------------------------------------
// Sets the path to the font of the application
// Can be called before initialization
int32_t oh_dependencies_set_ttf_font_path(const char *path);

// Sets the size of the font
// Can be called before and after initialization
int32_t oh_dependencies_set_ttf_font_size(int32_t size);

// Initializes dependencies (mostly SDL)
// Exits in most cases if it fails to initialize basic stuff
int32_t oh_dependencies_init();

// Free dependencies
int32_t oh_dependencies_quit();

// ---------------------------------------------------------------------
// Functions to retrieve relevant information
TTF_Font 		*oh_dependencies_get_font();
SDL_Window 		*oh_dependencies_get_window();
SDL_DisplayMode oh_dependencies_get_display_mode();
SDL_Renderer 	*oh_dependencies_get_renderer();
SDL_Event 		*oh_dependencies_get_event();

#endif
