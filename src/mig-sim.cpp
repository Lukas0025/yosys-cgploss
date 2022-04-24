/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with MIG simulation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "mig.h"

#define maj3(I1, I2, I3) ((I1 & I2) | (I2 & I3) | (I1 & I3))

namespace representation {
	void mig::simulate(std::vector<simulation::io_t> &gates_out) {
		for (auto id = this->chromosome->last_input + 1; id < this->chromosome->size(); id++) {
			
			auto gene = this->chromosome->get_gene_ptr(id);
			
			     if (gene->type == SAFE_TYPE_ID(0b0000)) gates_out[id].vec =  maj3( gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0001)) gates_out[id].vec =  maj3(~gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0010)) gates_out[id].vec =  maj3( gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0011)) gates_out[id].vec =  maj3(~gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0100)) gates_out[id].vec =  maj3( gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0101)) gates_out[id].vec =  maj3(~gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0110)) gates_out[id].vec =  maj3( gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b0111)) gates_out[id].vec =  maj3(~gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1000)) gates_out[id].vec = ~maj3( gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1001)) gates_out[id].vec = ~maj3(~gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1010)) gates_out[id].vec = ~maj3( gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1011)) gates_out[id].vec = ~maj3(~gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec,  gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1100)) gates_out[id].vec = ~maj3( gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1101)) gates_out[id].vec = ~maj3(~gates_out[gene->Inputs[0]].vec,  gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1110)) gates_out[id].vec = ~maj3( gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);
			else if (gene->type == SAFE_TYPE_ID(0b1111)) gates_out[id].vec = ~maj3(~gates_out[gene->Inputs[0]].vec, ~gates_out[gene->Inputs[1]].vec, ~gates_out[gene->Inputs[2]].vec);

			else throw std::runtime_error("invalid gate type in genome");
		}
	}
}