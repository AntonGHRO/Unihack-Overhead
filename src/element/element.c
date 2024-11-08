#include "../../include/element/element.h"
// ---------------------------------------------------------------------------------------------------------------------------------

// To avoid circular dependency
extern int32_t oh_control_x();
extern int32_t oh_control_y();

// ---------------------------------------------------------------------------------------------------------------------------------
int32_t oh_element_init(
	oh_element *element,
	oh_element *snap,
	oh_element_texture_type texture_type,
	uint8_t activity,

	int32_t useParam,
	int32_t useParamStr,

	...
) {
	if(activity != 0 && activity != 1) {
		oh_log(OH_LOG_ERROR, "oh_element_init(): Invalid range for activity. Expected OH_ELEMENT_(STATIC/DYNAMIC)");
		return OH_FALSE;
	}

	// Texture
	element->texture = oh_element_texture(texture_type);

	// Default for interact box
	element->interact.w = element->texture->surface->w;
	element->interact.h = element->texture->surface->h;
	element->interact.x = 0;
	element->interact.y = 0;

	// Hardcoded elements for special textures
	switch(texture_type) {
	case OH_ELEMENT_TEXTURE_WIN_200x200:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 140;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_300x300:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 240;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_400x400:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 340;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_600x600:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 540;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_800x800:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 740;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_1200x670:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 1140;
		element->interact.h = 25;
		break;
	case OH_ELEMENT_TEXTURE_WIN_1800x1000:
		element->interact.x = 24;
		element->interact.y = 24;
		element->interact.w = 1740;
		element->interact.h = 25;
		break;
	}

	// Position
	element->position.x = 0;
	element->position.y = 0;

	// Snap offset
	element->snap_offset.x = 0;
	element->snap_offset.y = 0;

	// Snap
	element->snap = snap;

	// Check if we acutally snapped to another element
	if(snap != NULL) {
		// Increase the snapped count of the other element
		snap->snapped_count ++;

		// If yes, check if that element has any snapped elements
		if(snap->snapped != NULL) {
			oh_element **aux = realloc(snap->snapped, oh_utils_math_nextpow2(snap->snapped_count) * sizeof(oh_element *));

			if(aux == NULL) {
				oh_log(OH_LOG_ERROR, "oh_element_init(): Failed to reallocate buffer of snapped elements of element 'snap'");
				return OH_FALSE;
			}

			snap->snapped = aux;
			snap->snapped[snap->snapped_count - 1] = element;
		} else {
			// If no, allocate memory and save this element to its snapped elements
			snap->snapped = malloc(sizeof(oh_element *));
			snap->snapped[0] = element;
		}

		// At the end if all went well, update the position to be an offset of the snap
		element->position.x = snap->position.x;
		element->position.y = snap->position.y;
	}

	// Snapped
	element->snapped = NULL;

	// Activity
	element->activity = activity;

	// State
	element->state = OH_ELEMENT_NORMAL;

	// Angle
	element->angle = 0.0;

	return OH_TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------------------
static int32_t oh_element_render_ex(oh_element *element) {
	if(element == NULL) {
		oh_log(OH_LOG_ERROR, "static oh_element_render_ex(): Received NULL element");
		return OH_FALSE;
	}

	// Doesn't check if element is snapped to something

	SDL_Rect rect = {
		.x = - oh_control_x() + element->position.x,
		.y = - oh_control_y() + element->position.y,
		.w = element->texture->surface->w,
		.h = element->texture->surface->h,
	};

	if(element->angle == 0.0) {
		SDL_RenderCopy(oh_dependencies_get_renderer(), element->texture->texture, NULL, &rect);
	} else {
		SDL_RenderCopyEx(
			oh_dependencies_get_renderer(),
			element->texture->texture, NULL,
			&rect, element->angle,
			NULL, SDL_FLIP_NONE);
	}

	#ifdef OH_DEBUG
		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 200, 0, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);

		rect.x += element->interact.x;
		rect.y += element->interact.y;
		rect.w = element->interact.w;
		rect.h = element->interact.h;

		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 0, 200, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);
	#endif

	// Render snapped elements
	if(element->snapped != NULL) {
		for(uint32_t i = 0; i < element->snapped_count; i ++) {
			if(oh_element_render_ex(element->snapped[i]) == OH_FALSE) {
				oh_log(OH_LOG_ERROR, "oh_element_render(): Static oh_element_render_ex() failed for snapped element: %u", i);
			}
		}
	}

	return OH_TRUE;
}

int32_t oh_element_render(oh_element *element) {
	// Check for NULL element
	if(element == NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_render(): Received NULL element");
		return OH_FALSE;
	}

	// Check if the element is snapped to something. If yes, don't render
	if(element->snap != NULL) {
		oh_log(OH_LOG_WARN, "oh_element_render(): Tried rendering a snapped element");
		return OH_FALSE;
	}

	// Prepare rectangle
	SDL_Rect rect = {
		.x = - oh_control_x() + element->position.x,
		.y = - oh_control_y() + element->position.y,
		.w = element->texture->surface->w,
		.h = element->texture->surface->h,
	};

	// Render with / without angle
	if(element->angle == 0.0) {
		SDL_RenderCopy(oh_dependencies_get_renderer(), element->texture->texture, NULL, &rect);
	} else {
		SDL_RenderCopyEx(
			oh_dependencies_get_renderer(),
			element->texture->texture, NULL,
			&rect, element->angle,
			NULL, SDL_FLIP_NONE);
	}

	#ifdef OH_DEBUG
		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 200, 0, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);

		rect.x += element->interact.x;
		rect.y += element->interact.y;
		rect.w = element->interact.w;
		rect.h = element->interact.h;

		SDL_SetRenderDrawColor(oh_dependencies_get_renderer(), 0, 200, 0, 255);
		SDL_RenderDrawRect(oh_dependencies_get_renderer(), &rect);
	#endif

	// Render snapped elements
	if(element->snapped != NULL) {
		// Iterate through them
		for(uint32_t i = 0; i < element->snapped_count; i ++) {
			// If one fails, log and move on (use static function that doesn't check for snap)
			if(oh_element_render_ex(element->snapped[i]) == OH_FALSE) {
				oh_log(OH_LOG_ERROR, "oh_element_render(): Static oh_element_render_ex() failed for snapped element: %u", i);
			}
		}
	}

	return OH_TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Static auxilary oh_element_set_position() that doesn't check if element is snapped
static void oh_element_set_position_ex(oh_element *element, int32_t x, int32_t y) {
	element->position.x = x + element->snap_offset.x;
	element->position.y = y + element->snap_offset.y;

	// Check if element has snapped elements
	if(element->snapped != NULL) {
		// Move them
		for(uint32_t i = 0; i < element->snapped_count; i ++) {
			oh_element_set_position_ex(element->snapped[i], element->position.x, element->position.y);
		}
	}
}

int32_t oh_element_set_position(oh_element *element, int32_t x, int32_t y) {
	// Cannot move a snapped element
	if(element->snap != NULL) {
		oh_log(OH_LOG_ERROR, "oh_element_set_position(): Cannot move a snapped element");
		return OH_FALSE;
	}

	element->position.x = x + element->snap_offset.x;
	element->position.y = y + element->snap_offset.y;

	// Check if element has snapped elements
	if(element->snapped != NULL) {
		// Move them
		for(uint32_t i = 0; i < element->snapped_count; i ++) {
			oh_element_set_position_ex(element->snapped[i], element->position.x, element->position.y);
		}
	}

	return OH_TRUE;
}

int32_t oh_element_set_snap_offset(oh_element *element, int32_t x, int32_t y) {
	if(element->snap == NULL) {
		oh_log(OH_LOG_WARN, "oh_element_set_snap_offset(): Tried to set snap_offset of a non snapped element");
		return OH_FALSE;
	}

	element->snap_offset.x = x;
	element->snap_offset.y = y;

	element->position.x = element->snap->position.x + element->snap_offset.x;
	element->position.y = element->snap->position.y + element->snap_offset.y;

	// Check if element has snapped elements
	if(element->snapped != NULL) {
		// Move them
		for(uint32_t i = 0; i < element->snapped_count; i ++) {
			oh_element_set_position_ex(element->snapped[i], element->position.x, element->position.y);
		}
	}

	return OH_TRUE;
}

void oh_element_set_interaction(oh_element *element, int32_t x, int32_t y, int32_t w, int32_t h) {
	element->interact.x = x;
	element->interact.y = y;
	element->interact.w = w;
	element->interact.h = h;
}

void oh_element_set_angle (oh_element *element, double angle) {
	element->angle = angle;
}

// ---------------------------------------------------------------------------------------------------------------------------------

int32_t oh_element_is_inside(oh_element *element, int32_t x, int32_t y) {
	// SDL_Point point = {x + oh_control_x(), y + oh_control_y()};
	SDL_Point point = {x, y};
	SDL_Rect rect = element->interact;

	rect.x += element->position.x;
	rect.y += element->position.y;

	return SDL_PointInRect(&point, &rect);
}

// ---------------------------------------------------------------------------------------------------------------------------------
