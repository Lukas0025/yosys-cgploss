/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig genome2rtlil inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "aig.h"
#include <stdexcept>

namespace representation {
	Yosys::RTLIL::Cell* aig::get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		auto gene = this->chromosome->get_gene(id);

		if (gene.type == SAFE_TYPE_ID(0b000)) return this->rtlil_add_and(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(0b001)) return this->rtlil_add_andnota(id, mod, assign_map, wire_namespace);
		if (gene.type == SAFE_TYPE_ID(0b010)) return this->rtlil_add_andnot(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(0b011)) return this->rtlil_add_nor(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(0b100)) return this->rtlil_add_nand(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(0b101)) return this->rtlil_add_ornot(id, mod, assign_map);
		if (gene.type == SAFE_TYPE_ID(0b110)) return this->rtlil_add_ornota(id, mod, assign_map, wire_namespace);
		if (gene.type == SAFE_TYPE_ID(0b111)) return this->rtlil_add_or(id, mod, assign_map);

		throw std::runtime_error("invalid gate type in genome");
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_andnota(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;

		auto not_wire = mod->addWire(wire_namespace + "not");
		
		gate_cell = mod->addCell(NEW_ID, "$_NOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		gate_cell->setPort(Yosys::ID::Y, not_wire);
		
		gate_cell = mod->addCell(NEW_ID, "$_AND_");
		gate_cell->setPort(Yosys::ID::A, not_wire);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_ornota(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;

		auto not_wire = mod->addWire(wire_namespace + "not");
		
		gate_cell = mod->addCell(NEW_ID, "$_NOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		gate_cell->setPort(Yosys::ID::Y, not_wire);
		
		gate_cell = mod->addCell(NEW_ID, "$_OR_");
		gate_cell->setPort(Yosys::ID::A, not_wire);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_andnot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_ANDNOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_ornot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;
		
		gate_cell = mod->addCell(NEW_ID, "$_ORNOT_");
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_and(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;

		if (gene.I1 == gene.I2) {
			gate_cell = mod->addCell(NEW_ID, "$_BUF_");
		} else {
			gate_cell = mod->addCell(NEW_ID, "$_AND_");
			this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		}
		
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_nand(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell;

		if (gene.I1 == gene.I2) {
			gate_cell = mod->addCell(NEW_ID, "$_NOT_");
		} else {
			gate_cell = mod->addCell(NEW_ID, "$_NAND_");
			this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		}
		
		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_or(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell = mod->addCell(NEW_ID, "$_OR_");

		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}

	Yosys::RTLIL::Cell* aig::rtlil_add_nor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
		auto gene = this->chromosome->get_gene(id);
		Yosys::RTLIL::Cell* gate_cell = mod->addCell(NEW_ID, "$_NOR_");

		this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.I1, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::B, gene.I2, assign_map);
		this->set_rtlil_port(gate_cell, Yosys::ID::Y, id, assign_map);

		return gate_cell;
	}
}