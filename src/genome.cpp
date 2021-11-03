#include "genome.h"

namespace genome {
	genome::genome() {

	}

	void genome::add_cell(cell_t cell) {
		this->genomes.push_back(cell.genome);
		this->types.push_back(cell.type);
	}

	cell_t genome::get_cell(int id) {
		cell_t cell;

		cell.genome = this->genomes[id];
		cell.type   = this->types[id];

		return cell;
	}
}