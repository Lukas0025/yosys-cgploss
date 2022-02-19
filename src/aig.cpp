#include "aig.h"
#include <stdexcept>

namespace representation {
	genome::io_id_t aig::add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) {

		if (type == ID($_BUF_))    return this->add_and(inputs, output);
		if (type == ID($_NOT_))    return this->add_not(inputs, output);
		if (type == ID($_AND_))    return this->add_and(inputs, output);
		if (type == ID($_NAND_))   return this->add_nand(inputs, output);
		if (type == ID($_ANDNOT_)) return this->add_andnot(inputs, output);
		if (type == ID($_OR_))     return this->add_or(inputs, output);
		if (type == ID($_NOR_))    return this->add_nor(inputs, output);
		if (type == ID($_ORNOT_))  return this->add_ornot(inputs, output);
		if (type == ID($_XOR_))    return this->add_xor(inputs, output);
		if (type == ID($_XNOR_))   return this->add_xnor(inputs, output);
		if (type == ID($_AOI3_))   return this->add_aoi3(inputs, output);
		if (type == ID($_OAI3_))   return this->add_oai3(inputs, output);
		if (type == ID($_OAI4_))   return this->add_oai4(inputs, output);
		if (type == ID($_OAI4_))   return this->add_oai4(inputs, output);
		if (type == ID($_MUX_))    return this->add_mux(inputs, output);
		if (type == ID($_NMUX_))   return this->add_nmux(inputs, output);

		throw std::runtime_error("invalid gate type");

	}

	genome::io_id_t aig::add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b001);
		gene.I1   = inputs[2]; //S
		gene.I2   = inputs[0]; //A

		auto a_sel = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[2]; //S
		gene.I2   = inputs[1]; //B

		auto b_sel = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b111);
		gene.I1   = a_sel;
		gene.I2   = b_sel;

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b001);
		gene.I1   = inputs[2]; //S
		gene.I2   = inputs[0]; //A

		auto a_sel = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[2]; //S
		gene.I2   = inputs[1]; //B

		auto b_sel = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b011);
		gene.I1   = a_sel;
		gene.I2   = b_sel;

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		auto and_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b011);
		gene.I1   = and_g;
		gene.I2   = inputs[2];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b111);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		auto or_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b100);
		gene.I1   = or_g;
		gene.I2   = inputs[2];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		auto and1_g = this->chromosome->add_gene(gene);

		gene.I1   = inputs[2];
		gene.I2   = inputs[3];

		auto and2_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b011);
		gene.I1   = and1_g;
		gene.I2   = and2_g;

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b111);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		auto or1_g = this->chromosome->add_gene(gene);

		gene.I1   = inputs[2];
		gene.I2   = inputs[3];

		auto or2_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b100);
		gene.I1   = or1_g;
		gene.I2   = or2_g;

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_and(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[0];
		gene.I2    = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_andnot(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b010);
		gene.I1   = inputs[0];
		gene.I2    = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_nand(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b100);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_not(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b100);
		gene.I1   = inputs[0];
		gene.I2   = inputs[0];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_or(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b111);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_ornot(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b101);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_nor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b011);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		this->chromosome->update_gene(output, gene);

		return output;
	}

	genome::io_id_t aig::add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
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

		return output;
	}

	genome::io_id_t aig::add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(0b000);
		gene.I1   = inputs[0];
		gene.I2   = inputs[1];

		auto and_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b011);

		auto nor_g = this->chromosome->add_gene(gene);

		gene.type = SAFE_TYPE_ID(0b111);

		gene.I1 = nor_g;
		gene.I2 = and_g;

		this->chromosome->update_gene(output, gene);

		return output;
	}


}