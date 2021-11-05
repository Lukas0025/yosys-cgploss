#include "genome.h"

namespace genome {
	genome::genome() {

	}

	void genome::swap_id(int id_a, int id_b) {

	}

	void genome::add_cell(cell_t cell) {
		this->genomes.push_back(cell.genome);
		this->environment.push_back(cell.type);
	}

	void genome::add_dummy_cell() {
		cell_t dummy_cell;

		dummy_cell.type = GATE_DUMMY;

		this->add_cell(dummy_cell);
	}

	int genome::size() {
		return this->genomes.size();
	}

	void genome::update_cell(cell_t cell) {
		this->genomes[cell.id]     = cell.genome;
		this->environment[cell.id] = cell.type;
	}

	void genome::order(std::set<int> inputs, std::set<int> outputs) {
		//fisrt inputs on top
		int id = 0;
		for (auto input : inputs) {
			this->swap_id(input, id);
			id++;
		}


	}

	cell_t genome::get_cell(int id) {
		cell_t cell;

		cell.genome = this->genomes[id];
		cell.type   = this->environment[id];
		cell.id     = id;

		return cell;
	}
}