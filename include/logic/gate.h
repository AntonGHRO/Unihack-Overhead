#ifndef OH_LOGIC_GATE_H
#define OH_LOGIC_GATE_H

// ---------------------------------------------------------------------

// Standard
#include <stdint.h>

// Dep
#include "../dependencies.h"

// Elements
#include "../element/element.h"

// ---------------------------------------------------------------------

typedef enum {
	AND, NAND,
	OR, NOR,
	XOR, XNOR,
	NOT,
	WIRE,
	OH_LOGIC_GATE_TYPE_COUNT,
} oh_logic_gate_type;

// ---------------------------------------------------------------------

typedef struct oh_logic_sim oh_logic_sim;

// Single gate
typedef struct {
	oh_element *element;
	oh_logic_sim *sim;

	int8_t out;
	int8_t out_next;

	int32_t x_index;
	int32_t y_index;

	oh_logic_gate_type type;
} oh_logic_gate;

// Sim
struct oh_logic_sim{
	oh_logic_gate *gate;
	uint32_t size;
	uint32_t cap;

	uint32_t clock;
};

// ---------------------------------------------------------------------

int32_t oh_logic_sim_init(oh_logic_sim *sim);
int32_t oh_logic_sim_add(oh_logic_sim *sim, oh_element *element, oh_logic_gate_type type);
int32_t oh_logic_sim_compute(oh_logic_sim *sim);

// input_index 1 or 2, not 0
int32_t oh_logic_sim_connect(oh_logic_gate *output, oh_logic_gate *input, int32_t input_index);

// Returns 0 if not
// Returns 1 or 2 depending on which input
int32_t oh_logic_gate_is_inside(oh_logic_gate *gate, int32_t x, int32_t y);

int8_t oh_logic_compute_gate(oh_logic_gate *gate);

#endif
