#include "../include/dependencies.h"
#include <SDL2/SDL_render.h>

// --------------------------------------------------------------------------------------------------------------------------------------
// Font
static TTF_Font *oh_font = NULL;

// TTF Font Path and Size
static char oh_ttf_font_path[128] = "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf";
static int 	oh_ttf_font_size = 12;

// Display mode, Window, Renderer, Event
static SDL_Window 		*oh_window;
static SDL_DisplayMode 	oh_display_mode;
static SDL_Renderer 	*oh_renderer;
static SDL_Event 		oh_event;

// --------------------------------------------------------------------------------------------------------------------------------------
int32_t oh_dependencies_set_ttf_font_path(const char *path) {
	if(path != NULL) {
		strncpy(oh_ttf_font_path, path, 128);
		return OH_TRUE;
	} else {
		oh_log(OH_LOG_WARN,
			"oh_dependencies_set_ttf_font_path(): Passed NULL path. Used instead: %s", oh_ttf_font_path);
		return OH_FALSE;
	}
}

int32_t oh_dependencies_set_ttf_font_size(int32_t size) {
	if(size >= OH_DEPENDENCIES_FONT_SIZE_MIN && size <= OH_DEPENDENCIES_FONT_SIZE_MAX) {
		oh_ttf_font_size = size;

		// RE-Initialize TTF font if needed
		if(oh_font != NULL) {
			TTF_CloseFont(oh_font);
			oh_font = NULL;

		    if((oh_font = TTF_OpenFont(oh_ttf_font_path, oh_ttf_font_size)) == NULL) {
		    	oh_log(OH_LOG_FATAL, "oh_dependencies_init(): TTF failed to open:\n\t%s", TTF_GetError());
		    	TTF_Quit();
		    	SDL_Quit();
		        return OH_FALSE;
		    }
		}

		return OH_TRUE;
	} else {
		oh_log(OH_LOG_WARN,
			"oh_dependencies_set_ttf_font_size(): Invalid range of size. Expected [%d, %d]",
			OH_DEPENDENCIES_FONT_SIZE_MIN,
			OH_DEPENDENCIES_FONT_SIZE_MAX
		);
		return OH_FALSE;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------
int32_t oh_dependencies_init() {
	// Initialize SDL video
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        oh_log(OH_LOG_FATAL, "oh_dependencies_init(): SDL failed to initialize:\n\t%s", SDL_GetError());
        return OH_FALSE;
    }

    // Initialize SDL IMG
    if(IMG_Init(IMG_INIT_PNG) == 0) {
    	oh_log(OH_LOG_FATAL, "oh_dependencies_init(): IMG failed to initialize:\n\t%s", IMG_GetError());
    	return OH_FALSE;
    }

    // Initialize SDL TTF
    if(TTF_Init() != 0) {
    	oh_log(OH_LOG_FATAL, "oh_dependencies_init(): TTF failed to initialize:\n\t%s", TTF_GetError());
    	SDL_Quit();
        return OH_FALSE;
    }

    // Initialize TTF font
    if((oh_font = TTF_OpenFont(oh_ttf_font_path, oh_ttf_font_size)) == NULL) {
    	oh_log(OH_LOG_FATAL, "oh_dependencies_init(): TTF failed to open:\n\t%s", TTF_GetError());
    	TTF_Quit();
    	SDL_Quit();
        return OH_FALSE;
    }

     // Get the display mode
	if(SDL_GetDesktopDisplayMode(0, &oh_display_mode) != 0) {
		oh_log(OH_LOG_FATAL, "oh_dependencies_init(): Failed to get display mode:\n\t%s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return OH_FALSE;
	}

    // Initialize the window
	if((oh_window = SDL_CreateWindow(
		"Overhead", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		oh_display_mode.w, oh_display_mode.h,
		SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN)) == NULL)
	{
		oh_log(OH_LOG_FATAL, "oh_dependencies_init(): Window failed to initialize:\n\t%s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
	    return OH_FALSE;
	}

    // Enable linear filtering for scaling textures corectly
    if(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") != SDL_TRUE) {
    	oh_log(OH_LOG_WARN, "oh_dependencies_init(): Linear filtering failed to initialize", SDL_GetError());
    }

    // Create the renderer
	if((oh_renderer = SDL_CreateRenderer(oh_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
		oh_log(OH_LOG_FATAL, "oh_dependencies_init(): Renderer failed to initialize:\n\t%s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return OH_FALSE;
	}

    // Blend mode for transparent textures
	if(SDL_SetRenderDrawBlendMode(oh_renderer, SDL_BLENDMODE_BLEND) == -1) {
		oh_log(OH_LOG_WARN, "oh_dependencies_init(): Blend mode failed to initialize:\n\t%s", SDL_GetError());
	}

	// SDL_RenderSetLogicalSize(oh_renderer, 2560, 1600);

	// Success
	oh_log(OH_LOG_INFO, "oh_dependencies_init(): Window successfully initialized");

	return OH_TRUE;
}

int32_t oh_dependencies_quit() {
	if(oh_window == NULL) {
		oh_log(OH_LOG_ERROR, "oh_dependencies_quit(): First call oh_dependencies_init()");
		return OH_FALSE;
	}

	TTF_CloseFont(oh_font);
	TTF_Quit();
	SDL_Quit();

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------
TTF_Font 		*oh_dependencies_get_font() {
	if(oh_window != NULL) {
		return oh_font;
	} else {
		oh_log(OH_LOG_ERROR, "oh_dependencies_get_font(): First call oh_dependencies_init()");
		return NULL;
	}
}

SDL_Window 		*oh_dependencies_get_window() {
	if(oh_window != NULL) {
		return oh_window;
	} else {
		oh_log(OH_LOG_ERROR, "oh_dependencies_get_window(): First call oh_dependencies_init()");
		return NULL;
	}
}

SDL_DisplayMode oh_dependencies_get_display_mode() {
	if(oh_window == NULL) {
		oh_log(OH_LOG_ERROR, "oh_dependencies_get_display_mode(): First call oh_dependencies_init()");
	}
	
	return oh_display_mode;
}

SDL_Renderer 	*oh_dependencies_get_renderer() {
	if(oh_window != NULL) {
		return oh_renderer;
	} else {
		oh_log(OH_LOG_ERROR, "oh_dependencies_get_renderer(): First call oh_dependencies_init()");
		return NULL;
	}
}

SDL_Event 		*oh_dependencies_get_event() {
	if(oh_window != NULL) {
		return &oh_event;
	} else {
		oh_log(OH_LOG_ERROR, "oh_dependencies_get_event(): First call oh_dependencies_init()");
		return NULL;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------
