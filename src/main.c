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
extern int32_t oh_control_cursor_x();
extern int32_t oh_control_cursor_y();
extern int32_t oh_control_set_worksheet(oh_worksheet *ws);
extern float oh_control_set_knob_sen(float sen);
extern int32_t oh_control_get_toggle();

// ----------------------------------- LEAKY DEMO
static int32_t toggle_reset = 0;

static float leaky_alpha = 0.0f;
static float leaky_bias = 0.0f;
static float LeakyReLU(float x) {
	x += 0.1 * leaky_bias;
	if (x > 0) return x;
	return 0.1 * leaky_alpha * x;
}

// ----------------------------------- FUNCTION APROX DEMO
static uint32_t rank = 12; // No. of hidden neurons
static float range_mix_x = -6.0f;
static float range_max_x = +6.0f;
static float step = +0.1f;
static float learn_rate = 1.0f;

// Actual NN
static float hw[1024];
static float b[1024];
static float ow[1024];

float randf(float min, float max) {
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

static float target(float x) {
    // return (1 / (1 + powf(2.71828182846f, -x)));
    return 0.1 * x * x;
}

static float lrelu(float x) {
	return x < 0 ? leaky_alpha * x : x;
}

static float feed(float x) {
    float sum = 0.0f;

    for(unsigned i = 0; i < rank; i ++) {
        sum += ow[i] * lrelu(hw[i] * x + b[i]);
    }

    return sum;
}

static float error(float x) {
    float real = feed(x);
    float expected = target(x);

    return (real - expected) * (real - expected);
}

void descent(float xt) {
    float common = 0.001f * learn_rate * 2 * (feed(xt) - target(xt));

    float hw_aux;
    float b_aux;
    float ow_aux;

    for(unsigned i = 0; i < rank; i ++) {
        ow_aux = common * lrelu(hw[i] * xt + b[i]);
        b_aux = common * ow[i] * ((hw[i] * xt + b[i]) < 0 ? leaky_alpha : 1);
        hw_aux = common * ow[i] * xt * ((hw[i] * xt + b[i]) < 0 ? leaky_alpha : 1);

        ow[i] -= ow_aux;
        b[i] -= b_aux;
        hw[i] -= hw_aux;
    }
}

static int32_t convert_x(SDL_Rect rect, float x, float min_x, float max_x) {
    return (int32_t)((x - min_x) / (max_x - min_x) * rect.w + rect.x);
}

static int32_t convert_y(SDL_Rect rect, float y, float min_y, float max_y) {
    return (int32_t)((max_y - y) / (max_y - min_y) * rect.h + rect.y);
}

static int32_t clip_y(SDL_Rect rect, int32_t y) {
    if (y < rect.y) return rect.y;
    if (y > rect.y + rect.h) return rect.y + rect.h;
    return y;
}

static void plot2D(SDL_Rect rect, float (*f)(float), float min_x, float max_x, float min_y, float max_y, float step, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 255, 100, 100, 255);

    int32_t x_axis = convert_y(rect, 0, min_y, max_y);  // Convert y = 0 to pixel position
    SDL_RenderDrawLine(oh_dependencies_get_renderer(), rect.x, x_axis, rect.x + rect.w, x_axis);
    
    int32_t y_axis = convert_x(rect, 0, min_x, max_x);  // Convert x = 0 to pixel position
    SDL_RenderDrawLine(oh_dependencies_get_renderer(), y_axis, rect.y, y_axis, rect.y + rect.h);
    
    SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), r, g, b, 255); 

    float x_prev = min_x;
    float y_prev = f(x_prev);
    
    int32_t px1 = convert_x(rect, x_prev, min_x, max_x);
    int32_t py1 = clip_y(rect, convert_y(rect, y_prev, min_y, max_y));

    for (float x = min_x + step; x <= max_x; x += step) {
        float y = f(x);

        int32_t px2 = convert_x(rect, x, min_x, max_x);
        int32_t py2 = clip_y(rect, convert_y(rect, y, min_y, max_y));

        if (py1 >= rect.y && py1 <= rect.y + rect.h && py2 >= rect.y && py2 <= rect.y + rect.h) {
            SDL_RenderDrawLine(oh_dependencies_get_renderer(), px1, py1, px2, py2);
        }

        px1 = px2;
        py1 = py2;
    }
}

// ============================================ INIT ============================================
int32_t oh_init() {
	// Set random seed
	srand((unsigned int)time(NULL));

	// Randomize params
    for(unsigned i = 0; i < rank; i ++) {
        ow[i] = randf(-1.0f, +1.0f);
        b[i] = randf(-0.5f, +0.5f);
        hw[i] = randf(-1.0f, +1.0f);
    }

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

	oh_element_set_snap_offset(worksheet.dynamic_element + 1, 66, 86);

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
	oh_element_param_str_set_str(worksheet.dynamic_element[17].param_str, " * One input layer with a single neuron X ");
	oh_element_param_str_set_str(worksheet.dynamic_element[18].param_str, " * One hidden layer with \"Leaky ReLU\" activations");
	oh_element_param_str_set_str(worksheet.dynamic_element[19].param_str, " * One output layer with a single neuron Y,");
	oh_element_param_str_set_str(worksheet.dynamic_element[20].param_str, " which will give us our function approximation.");
	oh_element_param_str_set_str(worksheet.dynamic_element[22].param_str, " ------------- Why Use Leaky ReLU ? -------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[23].param_str, " It allows for some gradient flow for negative ");
	oh_element_param_str_set_str(worksheet.dynamic_element[24].param_str, " values avoiding \"dead\" neurons. ");

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
	oh_element_param_str_set_str(worksheet.dynamic_element[32].param_str, " By learning the parameters of each of these ReLU functions, the");
	oh_element_param_str_set_str(worksheet.dynamic_element[33].param_str, " network can \"learn\" a function that closely resembles our ");
	oh_element_param_str_set_str(worksheet.dynamic_element[34].param_str, " target function even if it is nonlinear or complex. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[36].param_str, " ------------------------ Training the Network ------------------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[38].param_str, " To train this network, we will use a process called ");
	oh_element_param_str_set_str(worksheet.dynamic_element[39].param_str, " \"GRADIENT DESCENT\" with \"BACKPROPAGATION\" ");
	oh_element_param_str_set_str(worksheet.dynamic_element[40].param_str, " This process works as follows: ");
	oh_element_param_str_set_str(worksheet.dynamic_element[42].param_str, "   1. The NN applies Leaky ReLU on inputs, and passes it to the output. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[43].param_str, "   2. Measure how close the prediction is to the actual function ");
	oh_element_param_str_set_str(worksheet.dynamic_element[44].param_str, "      value using an \"ERROR FUNCTION\" or \"LOSS FUNCTION\". ");
	oh_element_param_str_set_str(worksheet.dynamic_element[45].param_str, "   3. Adjust each parameters based on the error to improve the NN's. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[47].param_str, " After enough training steps, the neural network will output values");
	oh_element_param_str_set_str(worksheet.dynamic_element[49].param_str, " --------------------------- Demonstration ----------------------------- ");
	oh_element_param_str_set_str(worksheet.dynamic_element[51].param_str, " Let's see how to build this neural network and train it with data ");
	oh_element_param_str_set_str(worksheet.dynamic_element[52].param_str, " points from a function. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[54].param_str, " As it learns, you'll see how the neural network starts");
	oh_element_param_str_set_str(worksheet.dynamic_element[55].param_str, " to approximate the shape of the function, even though");
	oh_element_param_str_set_str(worksheet.dynamic_element[56].param_str, " it starts with no prior knowledge of it.");
	oh_element_param_str_set_str(worksheet.dynamic_element[58].param_str, " ---------------------------- Other notes ------------------------------ ");

	// ---------------------------------------------------------------------------------------------------------------------------------- PLOT LEAKY RELU

	oh_worksheet_create_element(						// 65 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_element_set_position(worksheet.dynamic_element + 65, 0, 700);

	oh_worksheet_create_element(						// 66
		&worksheet, worksheet.dynamic_element + 65,
		OH_ELEMENT_TEXTURE_TEXT_BOX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 66, 66, 86);

	// ---------------------------------------------------------------------------------------------------------------------------------- KNOB ARRAY

	oh_worksheet_create_element(						// 67 Window for knob array
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_200x200,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 26,
		0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 67, 700, 900);
	oh_element_param_str_set_str(worksheet.dynamic_element[67].param_str, "Leaky ReLU");

	oh_worksheet_create_element(						// 68 First knob
		&worksheet, worksheet.dynamic_element + 67,
		OH_ELEMENT_TEXTURE_KNOB,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		10, 70, OH_ELEMENT_PARAM_MODE_NORMAL, 0, 0, 0, 255,
		5, 90, 0, 0, 0, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 68, 50, 80);
	oh_element_param_str_set_str(worksheet.dynamic_element[68].param_str, "alpha");

	oh_worksheet_create_element(						// 69 Second knob
		&worksheet, worksheet.dynamic_element + 67,
		OH_ELEMENT_TEXTURE_KNOB,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		10, 70, OH_ELEMENT_PARAM_MODE_NORMAL, 0, 0, 0, 255,
		10, 90, 0, 0, 0, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 69, 130, 80);
	oh_element_param_str_set_str(worksheet.dynamic_element[69].param_str, "bias");

	// ---------------------------------------------------------------------------------------------------------------------------------- PLOT FEED vs TARGET

	oh_worksheet_create_element(						// 70 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_element_set_position(worksheet.dynamic_element + 70, 1600, 0);

	oh_worksheet_create_element(						// 71
		&worksheet, worksheet.dynamic_element + 70,
		OH_ELEMENT_TEXTURE_TEXT_BOX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 71, 66, 86);

	// ---------------------------------------------------------------------------------------------------------------------------------- KNOB ARRAY 2

	oh_worksheet_create_element(						// 72 Window for knob array
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_200x200,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 26,
		0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 72, 1600, 700);
	oh_element_param_str_set_str(worksheet.dynamic_element[72].param_str, "Learning Rate");

	oh_worksheet_create_element(						// 73 First knob
		&worksheet, worksheet.dynamic_element + 72,
		OH_ELEMENT_TEXTURE_KNOB,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		10, 70, OH_ELEMENT_PARAM_MODE_NORMAL, 0, 0, 0, 255,
		5, 90, 0, 0, 0, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 73, 50, 80);
	oh_element_param_str_set_str(worksheet.dynamic_element[73].param_str, "lrate");

	oh_worksheet_create_element(						// 74 Toggle learn
		&worksheet, worksheet.dynamic_element + 72,
		OH_ELEMENT_TEXTURE_TOGGLE_OFF,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		14, 40, 0, 0, 0, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 74, 130, 70);
	oh_element_param_str_set_str(worksheet.dynamic_element[74].param_str, "learn");

	oh_worksheet_create_element(						// 75 Toggle reset
		&worksheet, worksheet.dynamic_element + 72,
		OH_ELEMENT_TEXTURE_TOGGLE_OFF,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		14, 40, 0, 0, 0, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 75, 130, 140);
	oh_element_param_str_set_str(worksheet.dynamic_element[75].param_str, "reset");

	// static float learn_rate = 0.001f;

	// ---------------------------------------------------------------------------------------------------------------------------------- WORKSHEET

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

	SDL_Rect rect = {
		.x = worksheet.dynamic_element[66].interact.x + worksheet.dynamic_element[66].position.x - oh_control_x(),
		.y = worksheet.dynamic_element[66].interact.y + worksheet.dynamic_element[66].position.y - oh_control_y(),
		.w = worksheet.dynamic_element[66].interact.w,
		.h = worksheet.dynamic_element[66].interact.h,
	};

	oh_control_set_knob_sen(0.01f);
	leaky_alpha = worksheet.dynamic_element[68].angle;

	oh_control_set_knob_sen(0.1f);
	leaky_bias = worksheet.dynamic_element[69].angle;

	plot2D(rect, LeakyReLU, -2.0, +2.0, -2.0, +2.0, 0.1f, 255, 255, 255);

	rect.x = worksheet.dynamic_element[71].interact.x + worksheet.dynamic_element[71].position.x - oh_control_x();
	rect.y = worksheet.dynamic_element[71].interact.y + worksheet.dynamic_element[71].position.y - oh_control_y();

	plot2D(rect, target, range_mix_x, range_max_x, -2.0, +2.0, 0.1f, 100, 255, 100);
	plot2D(rect, feed, range_mix_x, range_max_x, -2.0, +2.0, 0.1f, 255, 255, 255);

	oh_control_set_knob_sen(0.5f);
	leaky_bias = worksheet.dynamic_element[74].angle;

	if(oh_control_get_toggle()) {
		for(uint32_t i = 0; i < (uint32_t)((range_max_x - range_mix_x) / step); i ++) {
	        descent(range_mix_x + ((float)i) * step);
	    }

	    toggle_reset = 0;
	}

	if(!oh_control_get_toggle() && toggle_reset == 0) {
		// Randomize params
	    for(unsigned i = 0; i < rank; i ++) {
	        ow[i] = randf(-1.0f, +1.0f);
	        b[i] = randf(-0.5f, +0.5f);
	        hw[i] = randf(-1.0f, +1.0f);
	    }

	    toggle_reset = 1;
	}

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
