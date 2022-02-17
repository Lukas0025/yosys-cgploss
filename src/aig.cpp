#include "aig.h"

namespace representation {
	genome::io_id_t aig::add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) {

		if (type == ID($_AND_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b000);
			gene.I1   = inputs[0];
			gene.I2   = inputs[1];

			this->chromosome->update_gene(output, gene);
		} else if (type == ID($_NAND_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b100);
			gene.I1   = inputs[0];
			gene.I2   = inputs[1];

			this->chromosome->update_gene(output, gene);
		} else if (type == ID($_NOT_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b100);
			gene.I1   = inputs[0];
			gene.I2   = inputs[0];

			this->chromosome->update_gene(output, gene);
		} else if (type == ID($_OR_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b111);
			gene.I1   = inputs[0];
			gene.I2   = inputs[1];

			this->chromosome->update_gene(output, gene);
		} else if (type == ID($_NOR_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b011);
			gene.I1   = inputs[0];
			gene.I2   = inputs[1];

			this->chromosome->update_gene(output, gene);
		} else if (type == ID($_XOR_)) {
			genome::gene gene;

			gene.type = SAFE_TYPE_ID(0b000);
			gene.I1   = inputs[0];
			gene.I2   = inputs[1];

			auto and_g = this->chromosome->add_gene(gene);

			gene.type = SAFE_TYPE_ID(0b011);

			auto nor_g = this->chromosome->add_gene(gene);

			gene.I1 = nor_g;
			gene.I2 = and_g;

			this->chromosome->update_gene(output, gene);
		}

		return output;
	}
}