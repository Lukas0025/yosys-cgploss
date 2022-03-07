/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig rtlil2genome inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "aig.h"
#include <stdexcept>

namespace representation {
	genome::io_id_t aig::add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) {

		if (type == ID($_BUF_))    return this->update_aig_gate(output, 0b000, inputs[0], inputs[0]);
		if (type == ID($_NOT_))    return this->update_aig_gate(output, 0b100, inputs[0], inputs[0]);
		if (type == ID($_AND_))    return this->update_aig_gate(output, 0b000, inputs[0], inputs[1]);
		if (type == ID($_NAND_))   return this->update_aig_gate(output, 0b100, inputs[0], inputs[1]);
		if (type == ID($_ANDNOT_)) return this->update_aig_gate(output, 0b010, inputs[0], inputs[1]);
		if (type == ID($_OR_))     return this->update_aig_gate(output, 0b111, inputs[0], inputs[1]);
		if (type == ID($_NOR_))    return this->update_aig_gate(output, 0b011, inputs[0], inputs[1]);
		if (type == ID($_ORNOT_))  return this->update_aig_gate(output, 0b101, inputs[0], inputs[1]);
		if (type == ID($_XOR_))    return this->add_xor(inputs, output);
		if (type == ID($_XNOR_))   return this->add_xnor(inputs, output);
		if (type == ID($_AOI3_))   return this->add_aoi3(inputs, output);
		if (type == ID($_OAI3_))   return this->add_oai3(inputs, output);
		if (type == ID($_OAI4_))   return this->add_oai4(inputs, output);
		if (type == ID($_OAI4_))   return this->add_oai4(inputs, output);
		if (type == ID($_MUX_))    return this->add_mux(inputs, output);
		if (type == ID($_NMUX_))   return this->add_nmux(inputs, output);

		throw std::runtime_error("invalid gate type in RTLIL");

	}

	unsigned aig::mutate(unsigned center, unsigned sigma) {
		return this->chromosome->mutate(center, sigma, SAFE_TYPE_ID(0b000), SAFE_TYPE_ID(0b111));
	}

	genome::io_id_t aig::add_aig_gate(uint16_t type, genome::io_id_t I1, genome::io_id_t I2) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(type);
		gene.I1   = I1;
		gene.I2   = I2;

		return this->chromosome->add_gene(gene);
	}

	genome::io_id_t aig::update_aig_gate(genome::io_id_t id, uint16_t type, genome::io_id_t I1, genome::io_id_t I2) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(type);
		gene.I1   = I1;
		gene.I2   = I2;

		this->chromosome->update_gene(id, gene);

		return id;
	}

	genome::io_id_t aig::add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto a_sel = this->add_aig_gate(0b001, inputs[2], inputs[0]);
		auto b_sel = this->add_aig_gate(0b000, inputs[2], inputs[1]);

		return this->update_aig_gate(output, 0b111, a_sel, b_sel);
	}

	genome::io_id_t aig::add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto a_sel = this->add_aig_gate(0b001, inputs[2], inputs[0]);
		auto b_sel = this->add_aig_gate(0b000, inputs[2], inputs[1]);

		return this->update_aig_gate(output, 0b011, a_sel, b_sel);
	}

	genome::io_id_t aig::add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and_g = this->add_aig_gate(0b000, inputs[0], inputs[1]);

		return this->update_aig_gate(output, 0b011, and_g, inputs[2]);
	}

	genome::io_id_t aig::add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto or_g = this->add_aig_gate(0b111, inputs[0], inputs[1]);

		return this->update_aig_gate(output, 0b100, or_g, inputs[2]);
	}

	genome::io_id_t aig::add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and1_g = this->add_aig_gate(0b000, inputs[0], inputs[1]);
		auto and2_g = this->add_aig_gate(0b000, inputs[2], inputs[3]);

		return this->update_aig_gate(output, 0b011, and1_g, and2_g);
	}

	genome::io_id_t aig::add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto or1_g = this->add_aig_gate(0b111, inputs[0], inputs[1]);
		auto or2_g = this->add_aig_gate(0b111, inputs[2], inputs[3]);

		return this->update_aig_gate(output, 0b100, or1_g, or2_g);
	}

	genome::io_id_t aig::add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and_g = this->add_aig_gate(0b000, inputs[0], inputs[1]);
		auto nor_g = this->add_aig_gate(0b011, inputs[0], inputs[1]);

		return this->update_aig_gate(output, 0b011, nor_g, and_g);
	}

	genome::io_id_t aig::add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and_g = this->add_aig_gate(0b000, inputs[0], inputs[1]);
		auto nor_g = this->add_aig_gate(0b011, inputs[0], inputs[1]);

		return this->update_aig_gate(output, 0b111, nor_g, and_g);
	}
}