/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig rtlil2genome inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "mig.h"

#define CONST_0_IN 0
#define CONST_1_IN 1

#include <stdexcept>

namespace representation {
	genome::io_id_t mig::add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) {

		if (type == ID($_BUF_))    return this->update_mig_gate(output, 0b0000, inputs[0], CONST_0_IN, inputs[0]);
		if (type == ID($_NOT_))    return this->update_mig_gate(output, 0b1000, inputs[0], CONST_0_IN, inputs[0]);
		if (type == ID($_AND_))    return this->update_mig_gate(output, 0b0000, inputs[0], CONST_0_IN, inputs[1]);
		if (type == ID($_NAND_))   return this->update_mig_gate(output, 0b1000, inputs[0], CONST_0_IN, inputs[1]);
		if (type == ID($_ANDNOT_)) return this->update_mig_gate(output, 0b0100, inputs[0], CONST_0_IN, inputs[1]);
		if (type == ID($_OR_))     return this->update_mig_gate(output, 0b0000, inputs[0], CONST_1_IN, inputs[1]);
		if (type == ID($_NOR_))    return this->update_mig_gate(output, 0b1000, inputs[0], CONST_1_IN, inputs[1]);
		if (type == ID($_ORNOT_))  return this->update_mig_gate(output, 0b0100, inputs[0], CONST_1_IN, inputs[1]);
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

	unsigned mig::mutate(unsigned center, unsigned sigma) {
		return this->chromosome->mutate(center, sigma, SAFE_TYPE_ID(0b0000), SAFE_TYPE_ID(0b1111));
	}

	genome::io_id_t mig::add_mig_gate(uint16_t type, genome::io_id_t I1, genome::io_id_t I2, genome::io_id_t I3) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(type);
		gene.Inputs[0]   = I1;
		gene.Inputs[1]   = I2;
		gene.Inputs[2]   = I3;

		return this->chromosome->add_gene(gene);
	}

	genome::io_id_t mig::update_mig_gate(genome::io_id_t id, uint16_t type, genome::io_id_t I1, genome::io_id_t I2, genome::io_id_t I3) {
		genome::gene gene;

		gene.type = SAFE_TYPE_ID(type);
		gene.Inputs[0]   = I1;
		gene.Inputs[1]   = I2;
		gene.Inputs[2]   = I3;

		this->chromosome->update_gene(id, gene);

		return id;
	}

	genome::io_id_t mig::add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and1_g = this->add_mig_gate(0b0100, inputs[0], CONST_0_IN, inputs[1]);
		auto and2_g = this->add_mig_gate(0b0001, inputs[0], CONST_0_IN, inputs[1]);

		return this->update_mig_gate(output, 0b0000, and1_g, CONST_1_IN, and2_g);
	}

	genome::io_id_t mig::add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and1_g = this->add_mig_gate(0b0100, inputs[0], CONST_0_IN, inputs[1]);
		auto and2_g = this->add_mig_gate(0b0001, inputs[0], CONST_0_IN, inputs[1]);

		return this->update_mig_gate(output, 0b1000, and1_g, CONST_1_IN, and2_g);
	}

	genome::io_id_t mig::add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and_g = this->add_mig_gate(0b0000, inputs[0], CONST_0_IN, inputs[1]);

		return this->update_mig_gate(output, 0b1000, and_g, CONST_1_IN, inputs[2]);
	}

	genome::io_id_t mig::add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto or_g = this->add_mig_gate(0b0000, inputs[0], CONST_1_IN, inputs[1]);

		return this->update_mig_gate(output, 0b1000, or_g, CONST_0_IN, inputs[2]);
	}

	genome::io_id_t mig::add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto and1_g = this->add_mig_gate(0b0000, inputs[0], CONST_0_IN, inputs[1]);
		auto and2_g = this->add_mig_gate(0b0000, inputs[2], CONST_0_IN, inputs[3]);

		return this->update_mig_gate(output, 0b1000, and1_g, CONST_1_IN, and2_g);
	}

	genome::io_id_t mig::add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto or1_g = this->add_mig_gate(0b0000, inputs[0], CONST_1_IN, inputs[1]);
		auto or2_g = this->add_mig_gate(0b0000, inputs[2], CONST_1_IN, inputs[3]);

		return this->update_mig_gate(output, 0b1000, or1_g, CONST_0_IN, or2_g);
	}

    genome::io_id_t mig::add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto a_sel = this->add_mig_gate(0b0100, inputs[0], CONST_0_IN, inputs[2]);
		auto b_sel = this->add_mig_gate(0b0000, inputs[2], CONST_0_IN, inputs[1]);

		return this->update_mig_gate(output, 0b0000, a_sel, CONST_1_IN, b_sel);
	}

	genome::io_id_t mig::add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output) {
		auto a_sel = this->add_mig_gate(0b0100, inputs[0], CONST_0_IN, inputs[2]);
		auto b_sel = this->add_mig_gate(0b0000, inputs[2], CONST_0_IN, inputs[1]);

		return this->update_mig_gate(output, 0b1000, a_sel, CONST_1_IN, b_sel);
	}
}