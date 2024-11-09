#include "../../include/logic/gate.h"
#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------------------------

int32_t oh_logic_sim_init(oh_logic_sim *sim) {
	if(sim == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_init(): Received NULL sim");
		return OH_FALSE;
	}

	sim->clock = 0;
	sim->size = 0;
	sim->cap = 1024;

	sim->gate = malloc(sim->cap * sizeof(oh_logic_sim));
	
	if(sim->gate == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_init(): Failed to init gate array");
		return OH_FALSE;
	}

	return OH_TRUE;
}

int32_t oh_logic_sim_add(oh_logic_sim *sim, oh_element *element, oh_logic_gate_type type) {
	if(sim == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_add(): Received NULL sim");
		return OH_FALSE;
	}

	sim->gate[sim->size].element = element;
	sim->gate[sim->size].sim = sim;
	sim->gate[sim->size].type = type;

	sim->gate[sim->size].out = 0;
	sim->gate[sim->size].out_next = 0;

	sim->gate[sim->size].x_index = -1;
	sim->gate[++ sim->size].y_index = -1;

	return OH_TRUE;
}

int32_t oh_logic_sim_connect(oh_logic_gate *output, oh_logic_gate *input, int32_t input_index) {
	if(output == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_connect(): Received NULL output");
		return OH_FALSE;
	}

	if(input == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_connect(): Received NULL input");
		return OH_FALSE;
	}

	if(input->sim != output->sim) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_connect(): input sim doesn't match output sim");
		return OH_FALSE;
	}

	int32_t output_index = -1;

	for(uint32_t i = 0; i < output->sim->size; i ++) {
		if(output->sim->gate + i == output) {
			output_index = i;
		}
	}

	if(output_index == -1) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_connect(): Failed to find output in sim");
		return OH_FALSE;
	}

	if(input->type == WIRE || NOT) {
		input->x_index = output_index;
		return OH_TRUE;
	}

	if(input_index == 1) {
		input->x_index = output_index;
	} else if(input_index == 0) {
		input->y_index = output_index;
	}

	return OH_TRUE;
}

// --------------------------------------------------------------------------------------------------------------------------------------

int8_t oh_logic_compute_gate(oh_logic_gate *gate) {
    uint8_t x = (gate->x_index == -1) ? 0 : gate->sim->gate[gate->x_index].out;
    uint8_t y = (gate->y_index == -1) ? 0 : gate->sim->gate[gate->y_index].out;

    switch (gate->type) {
        case AND: return x && y;
        case OR: return x || y;
        case NOT: return !x;
        case XOR: return x ^ y;
        case NAND: return !(x && y);
        case NOR: return !(x || y);
        default:
            return 0;
    }
}

// --------------------------------------------------------------------------------------------------------------------------------------

int32_t oh_logic_gate_is_inside(oh_logic_gate *gate, int32_t x, int32_t y) {
	if(gate == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_gate_is_inside(): Received NULL gate");
		return 0;
	}

	if(oh_element_is_inside(gate->element, x, y) == OH_TRUE) {
		// if()
		// TODO
	}

	return OH_FALSE;
}

// --------------------------------------------------------------------------------------------------------------------------------------

int32_t oh_logic_sim_compute(oh_logic_sim *sim) {
	if(sim == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_compute(): Received NULL sim");
		return OH_FALSE;
	}

	for(uint32_t i = 0; i < sim->size; i ++) {
		sim->gate[i].out = sim->gate[i].out_next;
	}

	for(uint32_t i = 0; i < sim->size; i ++) {
		sim->gate[i].out_next = oh_logic_compute_gate(sim->gate + i);
	}

	sim->clock ++;
}

// --------------------------------------------------------------------------------------------------------------------------------------
