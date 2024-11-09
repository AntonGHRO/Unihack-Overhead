// Logging
#include "../include/dependencies.h"

// Elements etc
#include "../include/element/param.h"

// Grid, Worksheet etc
#include "../include/worksheet/grid.h"
#include "../include/worksheet/worksheet.h"
#include <stdlib.h>

// For now, only gonna be using one worksheet
static oh_worksheet worksheet;

// To avoid circular dependency hehe
extern int32_t oh_control_x();
extern int32_t oh_control_y();
extern int32_t oh_control_cursor_x();
extern int32_t oh_control_cursor_y();
extern int32_t oh_control_set_worksheet(oh_worksheet *ws);

// ============================================ INIT ============================================
int32_t oh_init() {
	// Set grid width and color
	oh_worksheet_grid_set_width(1);
	oh_worksheet_grid_set_color(0, 0, 0, 255);

	// Font size
	oh_dependencies_set_ttf_font_size(20);

	// Worksheet
	oh_worksheet_init(&worksheet, "main");

	oh_worksheet_create_element(						// 0
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_400x400,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_worksheet_create_element(						// 1
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_worksheet_create_element(						// 2
		&worksheet, worksheet.dynamic_element + 0,
		OH_ELEMENT_TEXTURE_KNOB,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		10, 60, OH_ELEMENT_PARAM_MODE_NORMAL, 0, 0, 0, 255,
		10, 80, 0, 0, 0, 255
	);

	oh_worksheet_create_element(						// 3
		&worksheet, worksheet.dynamic_element + 1,
		OH_ELEMENT_TEXTURE_TEXT_BOX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_worksheet_create_element(						// 4
		&worksheet, worksheet.dynamic_element + 3,
		OH_ELEMENT_TEXTURE_TEXT_LINE,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		0, 0, 255, 255, 255, 255
	);

	oh_worksheet_create_element(						// 5
		&worksheet, worksheet.dynamic_element + 3,
		OH_ELEMENT_TEXTURE_TEXT_LINE,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		0, 0, 255, 255, 255, 255
	);

	oh_worksheet_create_element(						// 6
		&worksheet, worksheet.dynamic_element + 3,
		OH_ELEMENT_TEXTURE_TEXT_LINE,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		0, 0, 255, 255, 255, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 2, 30, 60);
	oh_element_set_snap_offset(worksheet.dynamic_element + 3, 67, 90);

	oh_element_set_position(worksheet.dynamic_element + 1, 500, 0);

	oh_element_param_str_set_str(worksheet.dynamic_element[4].param_str, "Hello World");
	oh_element_param_str_set_str(worksheet.dynamic_element[5].param_str, "I love you Dia!");
	oh_element_param_str_set_str(worksheet.dynamic_element[6].param_str, "I'm bored");

	oh_element_set_snap_offset(worksheet.dynamic_element + 4, 0, 0);
	oh_element_set_snap_offset(worksheet.dynamic_element + 5, 0, 20);
	oh_element_set_snap_offset(worksheet.dynamic_element + 6, 0, 40);

	// Set worksheet to work on
	oh_control_set_worksheet(&worksheet);

	return OH_TRUE;
}

// ============================================ EVENT ============================================
int32_t oh_event(SDL_Event event) {

	return OH_TRUE;
}

// ============================================ UPDATE ============================================
int32_t oh_update() {
	oh_worksheet_render(&worksheet);

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
