/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig genome2rtlil inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#define UNUSED(expr) do { (void)(expr); } while (0)
#include "gates.h"
#include <stdexcept>

namespace representation {
	Yosys::RTLIL::Cell* gates::get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		UNUSED(wire_namespace);

        auto gene = this->chromosome->get_gene(id);

		if (gene.type == SAFE_TYPE_ID(AND_GATE_ID))    return this->rtlil_add_and(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(NOT_GATE_ID))    return this->rtlil_add_not(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(NAND_GATE_ID))   return this->rtlil_add_nand(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(ANDNOT_GATE_ID)) return this->rtlil_add_andnot(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(OR_GATE_ID))     return this->rtlil_add_or(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(NOR_GATE_ID))    return this->rtlil_add_nor(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(OR_NOT_GATE_ID)) return this->rtlil_add_ornot(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(XOR_GATE_ID))    return this->rtlil_add_xor(id, mod, assign_map);
        if (gene.type == SAFE_TYPE_ID(XNOR_GATE_ID))   return this->rtlil_add_xnor(id, mod, assign_map);

		throw std::runtime_error("invalid gate type in genome");
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_andnot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_ANDNOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

    Yosys::RTLIL::Cell* gates::rtlil_add_xor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_XOR_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

    Yosys::RTLIL::Cell* gates::rtlil_add_xnor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_XNOR_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_not(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_NOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_ornot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_ORNOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_and(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);

		Yosys::RTLIL::Cell* gate_cell = mod->addCell(NEW_ID, "$_AND_");
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_nand(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;

		
		gate_cell = mod->addCell(NEW_ID, "$_NAND_");

		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_or(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell = mod->addCell(NEW_ID, "$_OR_");

		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* gates::rtlil_add_nor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell = mod->addCell(NEW_ID, "$_NOR_");

		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.Inputs[1], assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}
}