#include "../../include/utils/log.h"

// String mapped by oh_log_level
static const char *const oh_log_level_str[] = {
	"TRACE", "DEBUG", " INFO",
	" WARN", "ERROR", "FATAL",
};

// Color code mapped by oh_log_level
static const char *const oh_log_color_code[] = {
	"\x1b[0m",		// 0 - Trace
	"\x1b[36m",		// 1 - Debug
	"\x1b[32m",		// 2 - Info
	"\x1b[33m",		// 3 - Warn
	"\x1b[31m",		// 4 - Error
	"\x1b[35m",		// 5 - Fatal
	"\x1b[0m",		// 6 - Reset
};

// Returns a statically allocated string for terminal color codes mapped by a oh_log_level
const char *oh_log_terminal_color_code(oh_log_level level) {
	return oh_log_color_code[level];
}

// Returns a statically allocated string resembling the oh_log_level
const char *oh_log_level_string(oh_log_level level) {
	return oh_log_level_str[level];
}
