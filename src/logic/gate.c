#include "../../include/logic/gate.h"

// --------------------------------------------------------------------------------------------------------------------------------------

void oh_logic_sim_init(oh_logic_sim *sim) {
	sim->clock = 0;
	sim->size = 0;
	sim->cap = 1024;

	sim->gate = malloc(sim->cap * sizeof(oh_logic_sim));
	
	if(sim->gate == NULL) {
		oh_log(OH_LOG_ERROR, "oh_logic_sim_init(): Failed to init gate array");
		return;
	}
}

void oh_logic_sim_add(oh_logic_sim *sim, oh_element *element, oh_logic_gate_type type) {
	sim->gate[sim->size].element = element;
	sim->gate[sim->size].type = type;

	sim->gate[sim->size].out = 0;
	sim->gate[sim->size].out_prev = 0;
	sim->gate[sim->size].x = -1;
	sim->gate[++ sim->size].y = -1;
}

void oh_logic_sim_connect(oh_logic_sim *sim, oh_logic_gate *output, oh_logic_gate *input, int32_t input_index) {
	
}

// --------------------------------------------------------------------------------------------------------------------------------------

int8_t oh_logic_compute_gate(oh_logic_gate *gate) {
    uint8_t x = (gate->x == -1) ? 0 : gates[gate->input1].output;
    uint8_t y = (gate->y == -1) ? 0 : gates[gate->input2].output;

    switch (gate->type) {
        case AND: return input1_val && input2_val;
        case OR: return input1_val || input2_val;
        case NOT: return !input1_val;
        case XOR: return input1_val ^ input2_val;
        case NAND: return !(input1_val && input2_val);
        case NOR: return !(input1_val || input2_val);

        // Flip-flops
        case D_FLIPFLOP:
            return input1_val;  // D flip-flop output follows D input
        case JK_FLIPFLOP:
            if (input1_val && input2_val) return !gate->prev_output; // Toggle if J=1, K=1
            else if (input1_val) return 1; // Set if J=1, K=0
            else if (input2_val) return 0; // Reset if J=0, K=1
            return gate->prev_output; // Hold previous state
        case T_FLIPFLOP:
            return input1_val ? !gate->prev_output : gate->prev_output; // Toggle on T=1, hold on T=0
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
