#include "../../include/utils/math.h"

// ---------------------------------------------------------------------
uint64_t oh_utils_math_nextpow2(uint64_t x) {
	x --;

	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;

	return x + 1;
}
