#include "../../include/worksheet/worksheet.h"

// --------------------------------------------------------------------------------------------------------------------------------------
int32_t oh_worksheet_init(oh_worksheet *ws, const char *name) {
	// Check for NULL
	if(ws == NULL) {
		oh_log(OH_LOG_WARN, "oh_worksheet_init(), ws:%s: Passed NULL worksheet", name);
		return OH_FALSE;
	}

	// Allocate and check for name
	if((ws->name = calloc(oh_utils_math_nextpow2(strlen(name)), sizeof(char))) == NULL) {
		oh_log(OH_LOG_ERROR, "oh_worksheet_init(), ws:%s: Failed to allocate memory for worksheet", name);
		return OH_FALSE;
	}

	// Copy the name
	strncpy(ws->name, name, strlen(name) + 1);

	// Desc NULL
	ws->desc = NULL;

	// Hover element
	ws->hover = NULL;

	// Canvas position starts at 0
	ws->x = 0;
	ws->y = 0;

	// Default grid width
	ws->grid_width = 1;

	// Static elements buffer
	ws->static_size = 0;
	ws->static_cap = oh_utils_math_nextpow2(OH_WORKSHEET_ELEMENT_BUFFER_CAP);
	ws->static_element = calloc(ws->static_cap, sizeof(oh_element));

	if(ws->static_element == NULL) {
		oh_log(OH_LOG_ERROR, "oh_worksheet_init(): Failed to allocate memory for static elements (%s)", name);
		free(ws->name);
		return OH_FALSE;
	}

	// Dynamic elements buffer
	ws->dynamic_size = 0;
	ws->dynamic_cap = oh_utils_math_nextpow2(OH_WORKSHEET_ELEMENT_BUFFER_CAP);
	ws->dynamic_element = calloc(ws->dynamic_cap, sizeof(oh_element));

	if(ws->static_element == NULL) {
		oh_log(OH_LOG_ERROR, "oh_worksheet_init(): Failed to allocate memory for static elements (%s)", name);
		free(ws->dynamic_element);
		free(ws->name);
		return OH_FALSE;
	}

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------
int32_t oh_worksheet_create_element(
	oh_worksheet *ws,

	oh_element *snap,
	oh_element_texture_type texture_type,
	uint8_t activity,

	int32_t useParam,
	int32_t useParamStr,

	...
) {
	if(ws == NULL) {
		oh_log(OH_LOG_WARN, "oh_worksheet_create_element(): Passed NULL worksheet");
		return OH_FALSE;
	}

	va_list list;
	va_start(list, useParamStr);

	// Create static element
	if(activity == OH_ELEMENT_ACTIVITY_STATIC) {
		// Check if static buffer is full
		if(ws->static_size == ws->static_cap) {
			ws->static_cap <<= 1;

			oh_element *aux = realloc(ws->static_element, ws->static_cap * sizeof(oh_element));

			if(aux == NULL) {
				oh_log(OH_LOG_ERROR, "oh_worksheet_create_element(), ws:%s: Failed to reallocate static element buffer", ws->name);
				return OH_FALSE;
			}
		}

		// Initialize the new element
		if(oh_element_init_ex(
			ws->static_element + (ws->static_size ++),
			snap, texture_type, activity, useParam, useParamStr, list) == OH_FALSE)
		{
			oh_log(OH_LOG_ERROR, "oh_worksheet_create_element(), ws:%s: Failed to initialize new element", ws->name);
			ws->static_size --;
			return OH_FALSE;
		}

		va_end(list);
		return OH_TRUE;

	// Create dynamic element
	} else if(activity == OH_ELEMENT_ACTIVITY_DYNAMIC) {
		// Check if dynamic buffer is full
		if(ws->dynamic_size == ws->dynamic_cap) {
			ws->dynamic_cap <<= 1;

			oh_element *aux = realloc(ws->dynamic_element, ws->dynamic_cap * sizeof(oh_element));

			if(aux == NULL) {
				oh_log(OH_LOG_ERROR, "oh_worksheet_create_element(), ws:%s: Failed to reallocate dynamic element buffer", ws->name);
				return OH_FALSE;
			}
		}

		// Initialize the new element
		if(oh_element_init_ex(
			ws->dynamic_element + (ws->dynamic_size ++),
			snap, texture_type, activity, useParam, useParamStr, list) == OH_FALSE)
		{
			oh_log(OH_LOG_ERROR, "oh_worksheet_create_element(), ws:%s: Failed to initialize new element", ws->name);
			ws->dynamic_size --;
			return OH_FALSE;
		}

		va_end(list);
		return OH_TRUE;
	}

	oh_log(OH_LOG_ERROR, "oh_worksheet_create_element(), ws:%s: Passed invalid range for activity", ws->name);
	va_end(list);
	return OH_FALSE;
}

int32_t oh_worksheet_render(oh_worksheet *ws) {
	if(ws == NULL) {
		oh_log(OH_LOG_WARN, "oh_worksheet_render(): Passed NULL worksheet");
		return OH_FALSE;
	}

	for(uint32_t i = 0; i < ws->static_size; i ++) {
		if(oh_element_render(ws->static_element + i) == OH_FALSE) {
			oh_log(OH_LOG_WARN, "oh_worksheet_render(): oh_element_render() static failed");
			return OH_FALSE;
		}
	}

	for(uint32_t i = 0; i < ws->dynamic_size; i ++) {
		if(oh_element_render(ws->dynamic_element + i) == OH_FALSE) {
			oh_log(OH_LOG_WARN, "oh_worksheet_render(): oh_element_render() dynamic failed");
			return OH_FALSE;
		}
	}

	return OH_TRUE;
}
