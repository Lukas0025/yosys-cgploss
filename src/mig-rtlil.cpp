/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with mig genome2rtlil inmplemenation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#define CONST_0_IN 0
#define CONST_1_IN 1

#include "mig.h"
#include <stdexcept>

#define IS_0_ON_PORT(gene, PORT) (((gene.Inputs[PORT] == CONST_0_IN) && !(UNSAFE_TYPE_ID(gene.type) & (1 << PORT))) || ((gene.Inputs[PORT] == CONST_1_IN) && (UNSAFE_TYPE_ID(gene.type) & (1 << PORT))))
#define IS_1_ON_PORT(gene, PORT) (((gene.Inputs[PORT] == CONST_1_IN) && !(UNSAFE_TYPE_ID(gene.type) & (1 << PORT))) || ((gene.Inputs[PORT] == CONST_0_IN) && (UNSAFE_TYPE_ID(gene.type) & (1 << PORT))))

namespace representation {
	Yosys::RTLIL::Cell* mig::get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {

        auto gene = this->chromosome->get_gene(id);

		if (IS_0_ON_PORT(gene, 0)) return rtlil_add_and_vars(gene.Inputs[1], gene.Inputs[2], NO_PORT_0_TYPE(gene.type), id, mod, assign_map, wire_namespace);
		if (IS_0_ON_PORT(gene, 1)) return rtlil_add_and_vars(gene.Inputs[0], gene.Inputs[2], NO_PORT_1_TYPE(gene.type), id, mod, assign_map, wire_namespace);
		if (IS_0_ON_PORT(gene, 2)) return rtlil_add_and_vars(gene.Inputs[0], gene.Inputs[1], NO_PORT_2_TYPE(gene.type), id, mod, assign_map, wire_namespace);

		if (IS_1_ON_PORT(gene, 0)) return rtlil_add_or_vars(gene.Inputs[1], gene.Inputs[2], NO_PORT_0_TYPE(gene.type), id, mod, assign_map, wire_namespace);
		if (IS_1_ON_PORT(gene, 1)) return rtlil_add_or_vars(gene.Inputs[0], gene.Inputs[2], NO_PORT_1_TYPE(gene.type), id, mod, assign_map, wire_namespace);
		if (IS_1_ON_PORT(gene, 2)) return rtlil_add_or_vars(gene.Inputs[0], gene.Inputs[1], NO_PORT_2_TYPE(gene.type), id, mod, assign_map, wire_namespace);

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

	Yosys::RTLIL::Cell* mig::rtlil_add_or_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		if (type == 0b111) return this->rtlil_add_and(a, b, 0b000, id, mod, assign_map, wire_namespace);
		if (type == 0b011) return this->rtlil_add_and(a, b, 0b100, id, mod, assign_map, wire_namespace);
		return this->rtlil_add_or (a, b,  type, id, mod, assign_map, wire_namespace);
	}

	Yosys::RTLIL::Cell* mig::rtlil_add_and_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		if (type == 0b111) return this->rtlil_add_or (a, b, 0b000, id, mod, assign_map, wire_namespace);
		if (type == 0b011) return this->rtlil_add_or (a, b, 0b100, id, mod, assign_map, wire_namespace);
		return this->rtlil_add_and(a, b,  type, id, mod, assign_map, wire_namespace);
	}

	Yosys::RTLIL::Cell* mig::rtlil_add_and(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {

		Yosys::RTLIL::SigBit a_wire = this->get_rtlil_port(a, assign_map);
		if (type & 0b001) {
			a_wire = mod->addWire(wire_namespace + "a_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, a, assign_map);
			gate_cell->setPort(Yosys::ID::Y, a_wire);
		}

		Yosys::RTLIL::SigBit b_wire = this->get_rtlil_port(b, assign_map);
		if (type & 0b010) {
			b_wire = mod->addWire(wire_namespace + "b_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, b, assign_map);
			gate_cell->setPort(Yosys::ID::Y, b_wire);
		}

		Yosys::RTLIL::Cell* inner_gate_cell;
		if (type & 0b100) {
			inner_gate_cell = mod->addCell(NEW_ID, "$_NAND_");
		} else {
			inner_gate_cell = mod->addCell(NEW_ID, "$_AND_");
		}

		inner_gate_cell->setPort(Yosys::ID::A, a_wire);
		inner_gate_cell->setPort(Yosys::ID::B, b_wire);
		this->set_rtlil_port(inner_gate_cell, Yosys::ID::Y, id, assign_map);

		return inner_gate_cell;
	}

	Yosys::RTLIL::Cell* mig::rtlil_add_or(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) {
		
		Yosys::RTLIL::SigBit a_wire = this->get_rtlil_port(a, assign_map);
		if (type & 0b001) {
			a_wire = mod->addWire(wire_namespace + "a_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, a, assign_map);
			gate_cell->setPort(Yosys::ID::Y, a_wire);
		}

		Yosys::RTLIL::SigBit b_wire = this->get_rtlil_port(b, assign_map);
		if (type & 0b010) {
			b_wire = mod->addWire(wire_namespace + "b_not");
			auto gate_cell = mod->addCell(NEW_ID, "$_NOT_");

			this->set_rtlil_port(gate_cell, Yosys::ID::A, b, assign_map);
			gate_cell->setPort(Yosys::ID::Y, b_wire);
		}

		Yosys::RTLIL::Cell* inner_gate_cell;
		if (type & 0b100) {
			inner_gate_cell = mod->addCell(NEW_ID, "$_NOR_");
		} else {
			inner_gate_cell = mod->addCell(NEW_ID, "$_OR_");
		}

		inner_gate_cell->setPort(Yosys::ID::A, a_wire);
		inner_gate_cell->setPort(Yosys::ID::B, b_wire);
		this->set_rtlil_port(inner_gate_cell, Yosys::ID::Y, id, assign_map);

		return inner_gate_cell;

	}
}