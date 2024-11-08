#ifndef OH_ELEMENT_PARAM_H
#define OH_ELEMENT_PARAM_H

// ---------------------------------------------------------------------

// Element
#include "../dependencies.h"

// ---------------------------------------------------------------------

#define OH_PARAM_STRING_CAP 			64
#define OH_PARAM_STRING_INIT			"Label"

#define OH_PARAM_KNOB_LABEL_OFFSET 		68
#define OH_PARAM_KNOB_PARAM_OFFSET 		84

// ---------------------------------------------------------------------

// Enum for showing how to render params
typedef enum {
	OH_ELEMENT_PARAM_MODE_NORMAL,
	OH_ELEMENT_PARAM_MODE_BINARY,
	OH_ELEMENT_PARAM_MODE_HEXADEC,
} oh_param_mode;

// Floating point param
typedef struct {
	long double data;
	long double data_old;

	oh_param_mode mode;
	SDL_Color color;

	SDL_Texture *texture;
	SDL_Rect 	rect;
} oh_element_param;

oh_element_param 	*oh_element_param_get 			(int32_t x, int32_t y, oh_param_mode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int32_t 			oh_element_param_set_val 		(oh_element_param *param, long double value);
int32_t				oh_element_param_set_mode 		(oh_element_param *param, oh_param_mode mode);
int32_t 			oh_element_param_set_color 		(oh_element_param *param, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int32_t 			oh_element_param_set_pos 		(oh_element_param *param, int32_t x, int32_t y);
int32_t 			oh_element_param_render  		(oh_element_param *param);

// ---------------------------------------------------------------------

// String point param
typedef struct {
	char 		*data;
	char 		*data_old;

	uint32_t 	size;
	uint32_t 	capacity;

	uint32_t 	size_old;
	uint32_t 	capacity_old;

	SDL_Color 	color;

	SDL_Texture *texture;
	SDL_Rect 	rect;
} oh_element_param_str;

oh_element_param_str 	*oh_element_param_str_get 		(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int32_t 				oh_element_param_str_set_str 	(oh_element_param_str *param, const char *data);
int32_t 				oh_element_param_str_set_color 	(oh_element_param_str *param, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int32_t 				oh_element_param_str_set_pos 	(oh_element_param_str *param, int32_t x, int32_t y);
int32_t 				oh_element_param_str_render 	(oh_element_param_str *param);

// ---------------------------------------------------------------------

#endif
