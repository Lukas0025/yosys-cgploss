/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig genome2rtlil inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#define CONST_0_IN 0
#define CONST_1_IN 1

#include "mig.h"
#include <stdexcept>

namespace representation {
	Yosys::RTLIL::Cell* mig::get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {

        auto gene = this->chromosome->get_gene(id);

		if (gene.type <= SAFE_TYPE_ID(0b1111)) return this->rtlil_add_maj3(id, mod, assign_map, wire_namespace);

		throw std::runtime_error("invalid gate type in genome");
	}

	Yosys::RTLIL::Cell* mig::rtlil_add_maj3(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		auto gene = this->chromosome->get_gene(id);

		Yosys::RTLIL::SigBit a = this->get_rtlil_port(gene.Inputs[0], assign_map);
		if (UNSAFE_TYPE_ID(gene.type) & 0b0001) {
			a = mod->addWire(wire_namespace + "a_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[0], assign_map);
			gate_cell->setPort(Yosys::ID::Y, a);
		}

		Yosys::RTLIL::SigBit b = this->get_rtlil_port(gene.Inputs[1], assign_map);
		if (UNSAFE_TYPE_ID(gene.type) & 0b0010) {
			b = mod->addWire(wire_namespace + "b_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[1], assign_map);
			gate_cell->setPort(Yosys::ID::Y, b);
		}
		
		Yosys::RTLIL::SigBit c = this->get_rtlil_port(gene.Inputs[2], assign_map);
		if (UNSAFE_TYPE_ID(gene.type) & 0b0100) {
			c = mod->addWire(wire_namespace + "c_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, gene.Inputs[2], assign_map);
			gate_cell->setPort(Yosys::ID::Y, c);
		}

		//majoryty inner
		auto and1 = mod->addWire(wire_namespace + "and1");
		auto and2 = mod->addWire(wire_namespace + "and2");
		auto and3 = mod->addWire(wire_namespace + "and3");
		auto or1  = mod->addWire(wire_namespace + "or1");
		
		auto gate_cell_and1 = mod->addCell(NEW_ID, "$_AND_");
		auto gate_cell_and2 = mod->addCell(NEW_ID, "$_AND_");
		auto gate_cell_and3 = mod->addCell(NEW_ID, "$_AND_");
		auto gate_cell_or1  = mod->addCell(NEW_ID, "$_OR_");
		auto gate_cell_or2  = mod->addCell(NEW_ID, "$_OR_");

		gate_cell_and1->setPort(Yosys::ID::A, a);
		gate_cell_and1->setPort(Yosys::ID::B, b);
		gate_cell_and1->setPort(Yosys::ID::Y, and1);

		gate_cell_and2->setPort(Yosys::ID::A, b);
		gate_cell_and2->setPort(Yosys::ID::B, c);
		gate_cell_and2->setPort(Yosys::ID::Y, and2);

		gate_cell_and3->setPort(Yosys::ID::A, a);
		gate_cell_and3->setPort(Yosys::ID::B, c);
		gate_cell_and3->setPort(Yosys::ID::Y, and3);

		gate_cell_or1->setPort(Yosys::ID::A, and1);
		gate_cell_or1->setPort(Yosys::ID::B, and2);
		gate_cell_or1->setPort(Yosys::ID::Y, or1);

		gate_cell_or2->setPort(Yosys::ID::A, or1);
		gate_cell_or2->setPort(Yosys::ID::B, and3);
		
		if (UNSAFE_TYPE_ID(gene.type) & 0b1000) {
			auto maj_out = mod->addWire(wire_namespace + "maj_out");
			gate_cell_or2->setPort(Yosys::ID::Y, maj_out);

			auto gate_cell_not = mod->addCell(NEW_ID, "$_NOT_");
			gate_cell_not->setPort(Yosys::ID::A, maj_out);
			this->set_rtlil_port(gate_cell_not, Yosys::ID::Y, id, assign_map);
			
			return gate_cell_not;
		} else {
			this->set_rtlil_port(gate_cell_or2, Yosys::ID::Y, id, assign_map);
		}

		return gate_cell_or2;
	}
}