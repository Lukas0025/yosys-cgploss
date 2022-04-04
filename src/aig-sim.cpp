#include "aig.h"

namespace representation {
	void aig::simulate(std::vector<simulation::io_t> &gates_out) {
		for (auto id = this->chromosome->last_input + 1; id < this->chromosome->size(); id++) {
			
			auto gene = this->chromosome->get_gene_ptr(id);
			
			     if (gene->type == SAFE_TYPE_ID(0b000)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  &  gates_out[gene->Inputs[1]].vec;  //AND
			else if (gene->type == SAFE_TYPE_ID(0b001)) gates_out[id].vec =   ~gates_out[gene->Inputs[0]].vec  &  gates_out[gene->Inputs[1]].vec;  //ANDNOTA
			else if (gene->type == SAFE_TYPE_ID(0b010)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  & ~gates_out[gene->Inputs[1]].vec;  //ANDNOT
			else if (gene->type == SAFE_TYPE_ID(0b011)) gates_out[id].vec = ~( gates_out[gene->Inputs[0]].vec  |  gates_out[gene->Inputs[1]].vec); //NOR
			else if (gene->type == SAFE_TYPE_ID(0b100)) gates_out[id].vec = ~( gates_out[gene->Inputs[0]].vec  &  gates_out[gene->Inputs[1]].vec); //NAND
			else if (gene->type == SAFE_TYPE_ID(0b101)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  | ~gates_out[gene->Inputs[1]].vec;  //ORNOT
			else if (gene->type == SAFE_TYPE_ID(0b110)) gates_out[id].vec =   ~gates_out[gene->Inputs[0]].vec  |  gates_out[gene->Inputs[1]].vec;  //ORNOTA
			else if (gene->type == SAFE_TYPE_ID(0b111)) gates_out[id].vec =    gates_out[gene->Inputs[0]].vec  |  gates_out[gene->Inputs[1]].vec;  //OR
			else throw std::runtime_error("invalid gate type in genome");
		}
	}
}