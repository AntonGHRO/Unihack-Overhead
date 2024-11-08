#include "../../include/element/param.h"
#include <SDL2/SDL_render.h>
#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------------------------

// To avoid circular dependency hehe cheeeeeeeky bastard
extern int32_t oh_control_x();
extern int32_t oh_control_y();

// --------------------------------------------------------------------------------------------------------------------------------------

oh_element_param *oh_element_param_get(int32_t x, int32_t y, oh_param_mode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	oh_element_param *param = malloc(sizeof(oh_element_param));

	if(param == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_get(): Failed to allocate memory for param");
		return NULL;
	}

	param->data = 0.0;
	param->data_old = 0.0;
	param->texture = NULL;
	param->mode = mode;

	param->color.r = r;
	param->color.g = g;
	param->color.b = b;
	param->color.a = a;

	param->rect.x = x;
	param->rect.y = y;
	param->rect.w = oh_dependencies_get_display_mode().w;
	param->rect.h = oh_dependencies_get_display_mode().h;

	return param;
}

void oh_element_param_set_val(oh_element_param *param, long double value) {
	param->data_old = param->data;
	param->data = value;
}

void oh_element_param_set_mode(oh_element_param *param, oh_param_mode mode) {
	param->mode = mode;
}

void oh_element_param_set_color (oh_element_param *param, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	param->color.r = r;
	param->color.g = g;
	param->color.b = b;
	param->color.a = a;
}

void oh_element_param_set_pos(oh_element_param *param, int32_t x, int32_t y) {
	param->rect.x = x;
	param->rect.y = y;
}

int32_t oh_element_param_render(oh_element_param *param) {
	static char string[1024];

	if(param->texture == NULL || param->data != param->data_old) {
		param->data_old = param->data;

		snprintf(string, 1024, "%.2Lf", param->data_old);

		SDL_Surface *surface = TTF_RenderText_Blended(oh_dependencies_get_font(), string, param->color);

		if(surface == NULL) {
			oh_log(OH_LOG_FATAL, "oh_element_param_render(): Failed to initialize param surface");
			return OH_FALSE;
		}

		param->rect.w = surface->w;
		param->rect.h = surface->h;

		if(param->texture != NULL) {
			SDL_DestroyTexture(param->texture);
		}

		param->texture = SDL_CreateTextureFromSurface(oh_dependencies_get_renderer(), surface);

		if(param->texture == NULL) {
			oh_log(OH_LOG_FATAL, "oh_element_param_render(): Failed to initialize performance param texture");
			return OH_FALSE;
		}

		SDL_FreeSurface(surface);
	}

	SDL_Rect rect = {
		.x = param->rect.x - oh_control_x(),
		.y = param->rect.y - oh_control_y(),
		.w = param->rect.w,
		.h = param->rect.h,
	};

	SDL_RenderCopy(oh_dependencies_get_renderer(), param->texture, NULL, &rect);
	
	#ifdef OH_DEBUG
		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 255, 0, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);
	#endif
	
	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------

oh_element_param_str *oh_element_param_str_get(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	oh_element_param_str *param = malloc(sizeof(oh_element_param_str));

	if(param == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_str_get(): Failed to allocate memory for param");
		return NULL;
	}

	param->data = NULL;
	param->data_old = NULL;
	param->size = 0;
	param->size_old = 0;
	param->capacity = 0;
	param->capacity_old = 0;

	param->rect.x = x;
	param->rect.y = y;
	param->rect.w = oh_dependencies_get_display_mode().w;
	param->rect.h = oh_dependencies_get_display_mode().h;

	param->color.r = r;
	param->color.g = g;
	param->color.b = b;
	param->color.a = a;

	param->texture = NULL;

	param->capacity = OH_PARAM_STRING_CAP;
	param->capacity_old = OH_PARAM_STRING_CAP;

	param->data = calloc(param->capacity, sizeof(char));
	param->data_old = calloc(param->capacity_old, sizeof(char));

	if(param->data == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_str_get(): Param string failed to initialize");
		return NULL;
	}

	if(param->data_old == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_str_get(): Param string failed to initialize");
		free(param->data);
		return NULL;
	}

	param->size = strlen(OH_PARAM_STRING_INIT);
	param->size_old = strlen(OH_PARAM_STRING_INIT);

	strncpy(param->data, OH_PARAM_STRING_INIT, param->size + 1);
	strncpy(param->data_old, OH_PARAM_STRING_INIT, param->size_old + 1);

	return param;
}

int32_t oh_element_param_str_set_str(oh_element_param_str *param, const char *data) {
	if(data == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_str_set_str(): Received NULL data");
		return OH_FALSE;
	}

	free(param->data_old);

	param->data_old = param->data;

	param->size = strlen(data);
	param->capacity = oh_utils_math_nextpow2(param->size);
	param->data = calloc(param->capacity, sizeof(char));
	// Calloc used because strncpy does not guarantee null termination

	if(param->data == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_param_str_set_str(): Failed to allocate memory for new data");
		return OH_FALSE;
	}

	strncpy(param->data, data, param->capacity);
	return OH_TRUE;
}

void oh_element_param_str_set_color(oh_element_param_str *param, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	param->color.r = r;
	param->color.g = g;
	param->color.b = b;
	param->color.a = a;
}

void oh_element_param_str_set_pos(oh_element_param_str *param, int32_t x, int32_t y) {
	param->rect.x = x;
	param->rect.y = y;
}

int32_t oh_element_param_str_render(oh_element_param_str *param) {
	if(param->texture == NULL || strcmp(param->data, param->data_old) != 0) {
		SDL_Surface *surface = TTF_RenderText_Blended(oh_dependencies_get_font(), param->data, param->color);

		if(surface == NULL) {
			oh_log(OH_LOG_FATAL, "oh_element_param_str_render(): Failed to initialize param surface");
			return OH_FALSE;
		}

		param->rect.w = surface->w;
		param->rect.h = surface->h;

		if(param->texture != NULL) {
			SDL_DestroyTexture(param->texture);
		}

		param->texture = SDL_CreateTextureFromSurface(oh_dependencies_get_renderer(), surface);

		if(param->texture == NULL) {
			oh_log(OH_LOG_FATAL, "oh_element_param_str_render(): Failed to initialize performance param texture");
			return OH_FALSE;
		}

		SDL_FreeSurface(surface);
	}

	SDL_Rect rect = {
		.x = param->rect.x - oh_control_x(),
		.y = param->rect.y - oh_control_y(),
		.w = param->rect.w,
		.h = param->rect.h,
	};

	SDL_RenderCopy(oh_dependencies_get_renderer(), param->texture, NULL, &rect);

	#ifdef OH_DEBUG
		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 255, 0, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);
	#endif

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------
