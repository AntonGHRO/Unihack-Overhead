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
extern int32_t oh_control_get_button();

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
    float common = 0.0001f * learn_rate * 2 * (feed(xt) - target(xt));

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
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 0
	);

	oh_element_param_str_set_str(worksheet.dynamic_element[0].param_str, "Introduction notes");

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

	oh_element_param_str_set_str(worksheet.dynamic_element[3].param_str, " --- Diving into the basics of machine learning -- ");
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
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 27, 700, 0);
	oh_element_param_str_set_str(worksheet.dynamic_element[27].param_str, "Training and Feeding the Neural Network");

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
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 65, 0, 700);
	oh_element_param_str_set_str(worksheet.dynamic_element[65].param_str, "Plot of Leaky ReLU");

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
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 67, 700, 900);
	oh_element_param_str_set_str(worksheet.dynamic_element[67].param_str, "ReLU Params");

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
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 70, 1600, 0);
	oh_element_param_str_set_str(worksheet.dynamic_element[70].param_str, "Plot of target and neural network side by side");

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
		30, 25,
		0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 72, 1600, 700);
	oh_element_param_str_set_str(worksheet.dynamic_element[72].param_str, "Learning");

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

	// ---------------------------------------------------------------------------------------------------------------------------------- Hardcode

	oh_worksheet_create_element(						// 76 Hardcode
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_HARDCODE,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0);

	oh_element_set_position(worksheet.dynamic_element + 76, 1600, -400);

	// ---------------------------------------------------------------------------------------------------------------------------------- Latex

	oh_worksheet_create_element(						// 77 Notes 800
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_800x800,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 77, -900, 0);
	oh_element_param_str_set_str(worksheet.dynamic_element[77].param_str, "LaTeX Integration");

	oh_worksheet_create_element(						// 78
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_TEXT_BOX_BIG,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 78, 60, 80);

	for(uint32_t i = 79; i < 115; i ++) {
		oh_worksheet_create_element(					// 79 - 115
			&worksheet, worksheet.dynamic_element + 78,
			OH_ELEMENT_TEXTURE_TEXT_LINE_BIG,
			OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
			0, 0, 255, 255, 255, 255
		);

		oh_element_set_snap_offset(worksheet.dynamic_element + i, 0, (i - 79) * 20);
		oh_element_param_str_set_str(worksheet.dynamic_element[i].param_str, " ");
	}

	oh_element_param_str_set_str(worksheet.dynamic_element[80].param_str, " LOSS FUNCTION - This function calculates the squared error");
	oh_element_param_str_set_str(worksheet.dynamic_element[81].param_str, " between the network's output y and the target f(x): ");
	oh_element_param_str_set_str(worksheet.dynamic_element[86].param_str, " GRADIENT DESCENT COMMON TERM - This is a factor used to scale ");
	oh_element_param_str_set_str(worksheet.dynamic_element[87].param_str, " each parameter update. It includes the learning rate and the ");
	oh_element_param_str_set_str(worksheet.dynamic_element[88].param_str, " gradient of the loss function with respect to the output. ");
	oh_element_param_str_set_str(worksheet.dynamic_element[93].param_str, " OUTPUT WEIGHT UPDATE ");
	oh_element_param_str_set_str(worksheet.dynamic_element[100].param_str, " HIDDEN BIAS AND WEIGHTS UPDATE ");

	oh_worksheet_create_element(						// 115 Latex
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 115, 46, 130);

	oh_worksheet_create_element(						// 116 Loss
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LOSS,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 116, 200, 165);

	oh_worksheet_create_element(						// 117 Latex
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 117, 46, 270);

	oh_worksheet_create_element(						// 118 Common
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_COMMON,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 118, 200, 305);

	oh_worksheet_create_element(						// 119 Latex
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 119, 46, 365);

	oh_worksheet_create_element(						// 120 OH_ELEMENT_TEXTURE_OW1
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_OW1,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 120, 200, 402);

	oh_worksheet_create_element(						// 121 Latex
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 121, 46, 415);

	oh_worksheet_create_element(						// 122 OH_ELEMENT_TEXTURE_OW2
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_OW2,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 122, 200, 453);

	oh_worksheet_create_element(						// 123 Latex 2
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX2,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 123, 46, 500);

	oh_worksheet_create_element(						// 124 OH_ELEMENT_TEXTURE_BIAS
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_BIAS,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 124, 200, 540);

	oh_worksheet_create_element(						// 125 Latex 2
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX2,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 125, 46, 580);

	oh_worksheet_create_element(						// 126 OH_ELEMENT_TEXTURE_BIAS
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_DLRL,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 126, 200, 610);

	oh_worksheet_create_element(						// 127 Latex 2
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_LATEX2,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 127, 46, 660);

	oh_worksheet_create_element(						// 128 OH_ELEMENT_TEXTURE_BIAS
		&worksheet, worksheet.dynamic_element + 77,
		OH_ELEMENT_TEXTURE_HW,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 128, 140, 698);

	// ---------------------------------------------------------------------------------------------------------------------------------- Plot Loss Function

	oh_worksheet_create_element(						// 129 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_600x600,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 129, 1600, 1000);
	oh_element_param_str_set_str(worksheet.dynamic_element[129].param_str, "Plot of loss function");

	oh_worksheet_create_element(						// 130
		&worksheet, worksheet.dynamic_element + 129,
		OH_ELEMENT_TEXTURE_TEXT_BOX,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 130, 66, 86);

	// ---------------------------------------------------------------------------------------------------------------------------------- Target + button

	oh_worksheet_create_element(						// 131 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_FUNCTION,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		275, 35, OH_ELEMENT_PARAM_MODE_NORMAL, 255, 255, 255, 255,
		98, 35, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 131, -900, 900);
	oh_element_param_set_val(worksheet.dynamic_element[131].param, 0.0);
	oh_element_param_str_set_str(worksheet.dynamic_element[131].param_str, "f(x)= 0.1 * x^2");

	oh_worksheet_create_element(						// 132
		&worksheet, worksheet.dynamic_element + 131,
		OH_ELEMENT_TEXTURE_BUTTON_OFF,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 132, 34, 14);

	oh_worksheet_create_element(						// 133 Knob
		&worksheet, worksheet.dynamic_element + 131,
		OH_ELEMENT_TEXTURE_KNOB,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 1, 1,
		12, 55, OH_ELEMENT_PARAM_MODE_NORMAL, 255, 255, 255, 255,
		5, 72, 255, 255, 255, 255
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 133, 34, 65);
	oh_element_param_str_set_str(worksheet.dynamic_element[133].param_str, "input");

	// ---------------------------------------------------------------------------------------------------------------------------------- Map

	oh_worksheet_create_element(						// 134 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_WIN_1200x670,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		30, 25, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 134, -1300, 1100);
	oh_element_param_str_set_str(worksheet.dynamic_element[134].param_str, "Knowledge Map Concept");

	oh_worksheet_create_element(						// 135 Plot
		&worksheet, worksheet.dynamic_element + 134,
		OH_ELEMENT_TEXTURE_MAP,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 135, 200, 300);

	oh_worksheet_create_element(						// 136
		&worksheet, worksheet.dynamic_element + 134,
		OH_ELEMENT_TEXTURE_TEXT_BOX_HUGE,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 0
	);

	oh_element_set_snap_offset(worksheet.dynamic_element + 136, 60, 100);

	for(uint32_t i = 137; i < 173; i ++) {
		oh_worksheet_create_element(					// 137 - 173
			&worksheet, worksheet.dynamic_element + 136,
			OH_ELEMENT_TEXTURE_TEXT_LINE_HUGE,
			OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
			0, 0, 255, 255, 255, 255
		);

		oh_element_set_snap_offset(worksheet.dynamic_element + i, 0, (i - 137) * 20);
		oh_element_param_str_set_str(worksheet.dynamic_element[i].param_str, " ");
	}

	oh_element_param_str_set_str(worksheet.dynamic_element[138].param_str, " Overhead is a powerful app designed to simplify complex concepts and enhance ");
	oh_element_param_str_set_str(worksheet.dynamic_element[139].param_str, " intuitive understanding through dynamic illustrations and animations.");
	oh_element_param_str_set_str(worksheet.dynamic_element[141].param_str, " With Overhead, learning becomes an interactive experience where users can dive into detailed visual scenarios, ");
	oh_element_param_str_set_str(worksheet.dynamic_element[142].param_str, " manipulating elements to see how changes affect the system in real time. ");

	// ---------------------------------------------------------------------------------------------------------------------------------- Map

	oh_worksheet_create_element(						// 173 Plot
		&worksheet, NULL,
		OH_ELEMENT_TEXTURE_OVERHEAD,
		OH_ELEMENT_ACTIVITY_DYNAMIC, 0, 1,
		0, 70, 0, 0, 0, 255
	);

	oh_element_set_position(worksheet.dynamic_element + 173, 20 + oh_control_x(), oh_control_y());
	oh_element_param_str_set_str(worksheet.dynamic_element[173].param_str, "Overhead");

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
	oh_element_set_position(worksheet.dynamic_element + 173, 20 + oh_control_x(), oh_control_y());

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

	rect.x = worksheet.dynamic_element[130].interact.x + worksheet.dynamic_element[130].position.x - oh_control_x();
	rect.y = worksheet.dynamic_element[130].interact.y + worksheet.dynamic_element[130].position.y - oh_control_y();

	plot2D(rect, error, range_mix_x, range_max_x, -2.0, +2.0, 0.1f, 255, 255, 0);

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

	if(oh_control_get_button()) {
		oh_element_param_set_val(worksheet.dynamic_element[131].param, target(worksheet.dynamic_element[133].param->data));
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
