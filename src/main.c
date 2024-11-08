// Logging
#include "../include/dependencies.h"

// Elements etc
#include "../include/element/param.h"

// Grid, Worksheet etc
#include "../include/worksheet/grid.h"
#include "../include/worksheet/worksheet.h"

// For now, only gonna be using one worksheet
static oh_worksheet worksheet;

// To avoid circular dependency hehe
extern int32_t oh_control_x();
extern int32_t oh_control_y();

// Test
oh_element element;

// ============================================ INIT ============================================
int32_t oh_init() {
	// Set grid width and color
	oh_worksheet_grid_set_width(1);
	oh_worksheet_grid_set_color(0, 0, 0, 255);

	// Worksheet
	oh_worksheet_init(&worksheet, "main");

	// Font size
	oh_dependencies_set_ttf_font_size(20);

	// Test
	oh_element_init(
		&element, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		200, 25, OH_ELEMENT_PARAM_MODE_BINARY, 255, 0, 0, 255,
		30, 25, 0, 0, 0, 255
	);

	oh_element_param_str_set_str(element.param_str, "Hello World");
	oh_element_param_set_val(element.param, -12.1);

	return OH_TRUE;
}

// ============================================ EVENT ============================================
int32_t oh_event(SDL_Event event) {

	return OH_TRUE;
}

// ============================================ UPDATE ============================================
int32_t oh_update() {
	// Test
	oh_element_render(&element);

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
