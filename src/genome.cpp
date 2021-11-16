#include "genome.h"

namespace genome {
	genome::genome() {

	}

	void genome::swap_id(int id_a, int id_b) {

		for(auto gene = this->chromosome.begin(); gene < this->chromosome.end(); gene++) {
			if (gene->I1 == id_a) {
				gene->I1 = id_b;
			} else if (gene->I1 == id_b) {
				gene->I1 = id_a;
			}

			if (gene->I2 == id_a) {
				gene->I2 = id_b;
			} else if (gene->I2 == id_b) {
				gene->I2 = id_a;
			}

			if (gene->I3 == id_a) {
				gene->I3 = id_b;
			} else if (gene->I3 == id_b) {
				gene->I3 = id_a;
			}

			if (gene->I4 == id_a) {
				gene->I4 = id_b;
			} else if (gene->I4 == id_b) {
				gene->I4 = id_a;
			}
		}

		std::swap(this->chromosome[id_a], this->chromosome[id_b]);
		std::swap(this->env[id_a],        this->env[id_b]);
	}

	void genome::add_cell(cell_t cell) {
		this->chromosome.push_back(cell.gene);
		this->env.push_back(cell.type);
	}

	void genome::add_dummy_cell() {
		cell_t dummy_cell;

		dummy_cell.type = GATE_DUMMY;

		this->add_cell(dummy_cell);
	}

	int genome::size() {
		return this->chromosome.size();
	}

	void genome::update_cell(cell_t cell) {
		this->chromosome[cell.id] = cell.gene;
		this->env[cell.id]        = cell.type;
	}

	bool genome::order(std::map<int, void*> inputs, std::map<int, void*> outputs) {
		//clear input/output maps
		this->wire_in.clear();
		this->wire_out.clear();

		//fisrt inputs on top
		int id = 0;
		while (inputs.size()) {

			auto input = inputs.begin();

			this->swap_id(input->first, id);
			this->wire_in[id] = input->second;

			//swap in inputs
			if (inputs.count(id)) {
				inputs.erase(id);
			} else {
				inputs.erase(input->first);
			}

			//swap in outputs
			if (outputs.count(id) && outputs.count(input->first)) {

				//std::swap(outputs.at(id), outputs.at(input->first));

			} else if (outputs.count(id)) {

				outputs[input->first] = outputs[id];
				outputs.erase(id);

			} else if (outputs.count(input->first)) {

				outputs[id] = outputs[input->first];
				outputs.erase(input->first);

			}

			id++;
		}

		this->last_input = id - 1;

		//now sort others gates
		int only_inputs_to = this->last_input;
		while (id < this->size()) {

			int to_swap = -1;
			for (auto i = id; i < this->size(); i++) {
				if (this->chromosome[i].I1 <= only_inputs_to && 
				    this->chromosome[i].I2 <= only_inputs_to &&
					this->chromosome[i].I3 <= only_inputs_to &&
					this->chromosome[i].I4 <= only_inputs_to) {
						to_swap = i;
						break;
					}
			}

			if (to_swap == -1) {
				if (id > (only_inputs_to + 1)) {
					only_inputs_to++;
					continue;
				}

				//same cell have as input own output
				return false;
			}

			this->swap_id(id, to_swap);

			//outputs mapping fix
			if (outputs.count(id) && outputs.count(to_swap)) {

				//std::swap(outputs.at(id), outputs.at(to_swap));

			} else if (outputs.count(id)) {

				outputs[to_swap] = outputs[id];
				outputs.erase(id);

			} else if (outputs.count(to_swap)) {

				outputs[id] = outputs[to_swap];
				outputs.erase(to_swap);

			}
			
			id++;
		}

		//set outputs wires
		for (auto output = outputs.begin(); output != outputs.end(); output++) {
			this->wire_out[output->first] = output->second;
		}

		return true;
	}

	void genome::print(void (*log)(const char* format, ...)) {
		log("chromosome: [ ");

		for (auto gene: this->chromosome) {
			log(" %d %d %d %d    ", gene.I1, gene.I2, gene.I3, gene.I4);
		}

		log("]\n\n");

		log("env: [ ");
		
		for (auto enviroment : this->env) {
			log(" %d    ", enviroment);
		}

		log("]\n\n");

		log("last input %d\n", this->last_input);
	}

	cell_t genome::get_cell(int id) {
		cell_t cell;

		cell.gene   = this->chromosome[id];
		cell.type   = this->env[id];
		cell.id     = id;

		return cell;
	}

	cell_t genome::pop_cell_front() {
		cell_t cell;
		cell.id = -1;
		
		if (this->size() > 0) {
			cell = this->get_cell(0);
			this->chromosome.erase(this->chromosome.begin());
			this->env.erase(this->env.begin());
		}

		return cell;

	}
}