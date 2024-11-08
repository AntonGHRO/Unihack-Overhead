// Logging
#include "../include/dependencies.h"

// Elements etc
#include "../include/element/param.h"

// Grid, Worksheet etc
#include "../include/worksheet/grid.h"
#include "../include/worksheet/worksheet.h"
#include <SDL2/SDL_mouse.h>

// For now, only gonna be using one worksheet
static oh_worksheet worksheet;

// To avoid circular dependency hehe
extern int32_t oh_control_x();
extern int32_t oh_control_y();

// ============================================ INIT ============================================
int32_t oh_init() {
	// Set grid width and color
	oh_worksheet_grid_set_width(1);
	oh_worksheet_grid_set_color(0, 0, 0, 255);

	// Worksheet
	oh_worksheet_init(&worksheet, "main");

	// Font size
	oh_dependencies_set_ttf_font_size(20);

	return OH_TRUE;
}

// ============================================ EVENT ============================================
int32_t oh_event(SDL_Event event) {

	return OH_TRUE;
}

// ============================================ UPDATE ============================================
int32_t oh_update() {

	int32_t x, y;
	SDL_GetMouseState(&x, &y);

	return OH_TRUE;
}

// ============================================ CLEANUP ============================================
int32_t oh_cleanup() {

	return OH_TRUE;
}

// Definition error in log.h
void oh_log(const oh_log_level level, const char *const fmt, ...) {
	// TODO: Watch out for static strings...
	static char format[16384];
	static char time_string[1024];
	static time_t current_time;

	current_time = time(NULL);

	va_list list;
	va_start(list, fmt);

	strftime(time_string, 1024, "%H:%M:%S on %d-%m-%Y", localtime(&current_time));

	snprintf(format, 16384, "%s | %s[%s]%s: %s\n",
		time_string,
		oh_log_terminal_color_code(level),
		oh_log_level_string(level),
		oh_log_terminal_color_code(OH_LOG_LEVEL_COUNT),
		fmt);

	vfprintf(stderr, format, list);

	va_end(list);
}
