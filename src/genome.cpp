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
		std::swap(this->env[id_a],     this->env[id_b]);
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

	void genome::order(std::set<int> inputs, std::set<int> outputs) {
		//fisrt inputs on top
		int id = 0;
		while (inputs.size() > 0) {
			auto input = *inputs.begin();

			this->swap_id(input, id);

			//swap in inputs
			if (inputs.count(id)) {
				inputs.erase(id);
			} else {
				inputs.erase(input);
			}

			//swap in outputs
			if (!(outputs.count(id) && outputs.count(input))) {
				if (outputs.count(id)) {
					outputs.erase(id);
					outputs.insert(input);
				} else if (outputs.count(input)) {
					outputs.insert(id);
					outputs.erase(input);
				}
			}

			id++;
		}

		this->last_input = id;

		//now sort others gates


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