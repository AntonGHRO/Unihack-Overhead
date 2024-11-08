#ifndef OH_UTILS_LOG_H
#define OH_UTILS_LOG_H

// ---------------------------------------------------------------------

// Enum that specifies the level of the log
typedef enum oh_log_level {
	OH_LOG_TRACE 		= 0,
	OH_LOG_DEBUG 		= 1,
	OH_LOG_INFO 		= 2,
	OH_LOG_WARN 		= 3,
	OH_LOG_ERROR 		= 4,
	OH_LOG_FATAL 		= 5,
	OH_LOG_LEVEL_COUNT 	= 6,
} oh_log_level;

// ---------------------------------------------------------------------

// Function to log errors. Must be defined.
extern void oh_log(const oh_log_level, const char *const fmt, ...);

// Returns a statically allocated string for terminal color codes mapped by a oh_log_level
// Use OH_LOG_LEVEL_COUNT for reset color code
const char *oh_log_terminal_color_code(oh_log_level level);

// Returns a statically allocated string resembling the oh_log_level
const char *oh_log_level_string(oh_log_level level);

// ---------------------------------------------------------------------

#endif
