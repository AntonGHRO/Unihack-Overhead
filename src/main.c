// Logging
#include "../include/dependencies.h"

// Elements etc
#include "../include/element/param.h"

// Grid, Worksheet etc
#include "../include/worksheet/grid.h"
#include "../include/worksheet/worksheet.h"
#include <stdint.h>
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

	// ---------------------------------------------------------------------------------------------------------------------------------- FIRST NOTES
	oh_worksheet_create_element(						// 0 Notes 600
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_worksheet_create_element(						// 1
		&worksheet, worksheet.dynamic_element,
		OH_ELEMENT_TEXTURE_TEXT_BOX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 1, 68, 86);

	for(uint32_t i = 2; i < 27; i ++) {
		oh_worksheet_create_element(					// 2-26
			&worksheet, worksheet.dynamic_element + 1,
			OH_ELEMENT_TEXTURE_TEXT_LINE,
			OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
			0, 0, 255, 255, 255, 255
		);

		oh_element_set_snap_offset(worksheet.dynamic_element + i, 0, (i - 2) * 20);
		oh_element_param_str_set_str(worksheet.dynamic_element[i].param_str, " ");
	}

	oh_element_param_str_set_str(worksheet.dynamic_element[3].param_str, " -- Diving into the basics of machine learning --");
	oh_element_param_str_set_str(worksheet.dynamic_element[5].param_str, " With a focus on how we can use neural");
	oh_element_param_str_set_str(worksheet.dynamic_element[6].param_str, " networks to approximate functions");
	oh_element_param_str_set_str(worksheet.dynamic_element[8].param_str, " We'll start with a simple example: approximating");
	oh_element_param_str_set_str(worksheet.dynamic_element[9].param_str, " a mathematical function around a point");
	oh_element_param_str_set_str(worksheet.dynamic_element[11].param_str, " ML models make predictions by recognizing");
	oh_element_param_str_set_str(worksheet.dynamic_element[12].param_str, " patterns in data, adjusting themselves");
	oh_element_param_str_set_str(worksheet.dynamic_element[13].param_str, " to improve accuracy over time");
	oh_element_param_str_set_str(worksheet.dynamic_element[15].param_str, " ----------- Our Neural Network Setup ----------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[17].param_str, " * One input layer with polynomial features");
	oh_element_param_str_set_str(worksheet.dynamic_element[18].param_str, " of our input variable (e.g. x, x^2, x^3)");
	oh_element_param_str_set_str(worksheet.dynamic_element[20].param_str, " * One output layer with a single neuron,");
	oh_element_param_str_set_str(worksheet.dynamic_element[21].param_str, " which will give us our function approximation.");
	oh_element_param_str_set_str(worksheet.dynamic_element[23].param_str, " --------------- Why this works ? --------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[24].param_str, " Our neural network acts similarly");
	oh_element_param_str_set_str(worksheet.dynamic_element[25].param_str, " to a Taylor series approximation!");

	// ---------------------------------------------------------------------------------------------------------------------------------- SECOND NOTES

	oh_worksheet_create_element(						// 27 Notes 800
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_800x800,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_element_set_position(worksheet.dynamic_element + 27, 700, 0);

	oh_worksheet_create_element(						// 28
		&worksheet, worksheet.dynamic_element + 27,
		OH_ELEMENT_TEXTURE_TEXT_BOX_BIG,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 28, 60, 80);

	for(uint32_t i = 29; i < 65; i ++) {
		oh_worksheet_create_element(					// 29 - 64
			&worksheet, worksheet.dynamic_element + 28,
			OH_ELEMENT_TEXTURE_TEXT_LINE_BIG,
			OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
			0, 0, 255, 255, 255, 255
		);

		oh_element_set_snap_offset(worksheet.dynamic_element + i, 0, (i - 29) * 20);
		oh_element_param_str_set_str(worksheet.dynamic_element[i].param_str, " ");
	}

	oh_element_param_str_set_str(worksheet.dynamic_element[30].param_str, " --------------------------- Getting started --------------------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[32].param_str, " By learning the weights on each of these polynomial features, the");
	oh_element_param_str_set_str(worksheet.dynamic_element[33].param_str, " network can \"learn\" a function that closely resembles our ");
	oh_element_param_str_set_str(worksheet.dynamic_element[34].param_str, " target function even if it is nonlinear or complex. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[36].param_str, " ------------------------ Training the Network ------------------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[38].param_str, " To train this network, we will use a process called ");
	oh_element_param_str_set_str(worksheet.dynamic_element[39].param_str, " \"GRADIENT DESCENT\" with \"BACKPROPAGATION\" ");
	oh_element_param_str_set_str(worksheet.dynamic_element[40].param_str, " This process works as follows: ");
	oh_element_param_str_set_str(worksheet.dynamic_element[42].param_str, "   1. The network predicts an output based on the current weights. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[43].param_str, "   2. We measure how close the prediction is to the actual function ");
	oh_element_param_str_set_str(worksheet.dynamic_element[44].param_str, "      value using an \"ERROR FUNCTION\" or \"LOSS FUNCTION\". ");
	oh_element_param_str_set_str(worksheet.dynamic_element[45].param_str, "   3. The network predicts an output based on the current weights. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[47].param_str, " After enough training steps, the neural network will output values");
	oh_element_param_str_set_str(worksheet.dynamic_element[49].param_str, " --------------------------- Demonstration ----------------------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[51].param_str, " Let's see how to build this neural network and train it with data ");
	oh_element_param_str_set_str(worksheet.dynamic_element[52].param_str, " points from a function. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[54].param_str, " As it learns, you'll see how the neural network starts");
	oh_element_param_str_set_str(worksheet.dynamic_element[55].param_str, " to approximate the shape of the function, even though");
	oh_element_param_str_set_str(worksheet.dynamic_element[56].param_str, " it starts with no prior knowledge of it.");
	oh_element_param_str_set_str(worksheet.dynamic_element[58].param_str, " ---------------------------- Other notes ------------------------------ ");

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
