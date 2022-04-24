/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with simulation of gate reprezenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */


#include "gates.h"

namespace representation {
	void gates::simulate(std::vector<simulation::io_t> &gates_out) {
		for (auto id = this->chromosome->last_input + 1; id < this->chromosome->size(); id++) {
			
			auto gene = this->chromosome->get_gene_ptr(id);
			
			     if (gene->type == SAFE_TYPE_ID(AND_GATE_ID))    gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  &  gates_out[gene->Inputs[1]].vec;  //AND
			else if (gene->type == SAFE_TYPE_ID(NOT_GATE_ID))    gates_out[id].vec =   ~gates_out[gene->Inputs[0]].vec;                                     //NOT
			else if (gene->type == SAFE_TYPE_ID(NAND_GATE_ID))   gates_out[id].vec = ~( gates_out[gene->Inputs[0]].vec  &  gates_out[gene->Inputs[1]].vec); //NAND
			else if (gene->type == SAFE_TYPE_ID(ANDNOT_GATE_ID)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  & ~gates_out[gene->Inputs[1]].vec;  //ANDNOT
			else if (gene->type == SAFE_TYPE_ID(OR_GATE_ID))     gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  |  gates_out[gene->Inputs[1]].vec;  //OR
			else if (gene->type == SAFE_TYPE_ID(NOR_GATE_ID))    gates_out[id].vec = ~( gates_out[gene->Inputs[0]].vec  |  gates_out[gene->Inputs[1]].vec); //NOR
			else if (gene->type == SAFE_TYPE_ID(OR_NOT_GATE_ID)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  | ~gates_out[gene->Inputs[1]].vec;  //ORNOT
			else if (gene->type == SAFE_TYPE_ID(XOR_GATE_ID))    gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  ^  gates_out[gene->Inputs[1]].vec;  //XOR
			else if (gene->type == SAFE_TYPE_ID(XNOR_GATE_ID))   gates_out[id].vec = ~( gates_out[gene->Inputs[0]].vec  ^  gates_out[gene->Inputs[1]].vec); //XNOR
            else throw std::runtime_error("invalid gate type in genome");
		}
	}
}