/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with gate representation header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#define AND_GATE_ID    0
#define NOT_GATE_ID    1
#define NAND_GATE_ID   2
#define ANDNOT_GATE_ID 3
#define OR_GATE_ID     4
#define NOR_GATE_ID    5
#define OR_NOT_GATE_ID 6
#define XOR_GATE_ID    7
#define XNOR_GATE_ID   8

#include "representation.h"

namespace representation {
	/**
	  * @brief Class for gates representation of circuic
	  * Gate reprezentation using standart gates in types. Supported gates are:
	  * AND, NOT, NAND, AND_NOT, OR, NOR, OR_NOT, XOR and XNOR 
	  */
	class gates : public representation {
		public:
			gates(genome::genome* chromosome) : representation(chromosome) {
				chromosome->gene_inputs_count = 2;
			};
			
			/**
			 * @brief Add Yosys::RTLIL cell as GATE to chromosome
			 * @param inputs inputs of gate
			 * @param outputs outputs of gate
			 * @return genome::io_id_t id of output
			 */
			genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output);

			/**
			 * @brief Add gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @param wire_namespace string namespace for new wires
			 * @return poiter to created RTLIL::cell Last created
			 */
			Yosys::RTLIL::Cell *get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			
			/**
			 * @brief Perform simulation on reprezentation
			 * @param gates_o input/output data structure. Must have .size() == chromozome.size()
			 * On position of input genes fill with testing input
			 * On outputs pusition  is outputs of circuic
			 */
			void simulate(std::vector<simulation::io_t> &gates_o);

			/**
			 * @brief Perform muatatin operation over chromozome
			 * @param center center of random number of mutations
			 * @param sigma sigma of normal distribution of number of mutations
			 * @param l_back maximal distance of mutation
			 * @return unsigned number of mutations
			 */
			unsigned mutate(unsigned center, unsigned sigma, unsigned l_back);

			/**
			 * @brief Get name of parts in genome
			 * @return string 
			 */
			std::string parts_naming() {
				return "\"gate\": [\"none\", \"A && B\", \"!A\", \"!(A && B)\", \"A && !B\", \"A || B\", \"!(A || B)\", \"A || !B\", \"A ^ B\", \"!(A ^ B)\"]";
			}

			/**
			 * @brief Make deep copy of this object
			 * @return new aig reprezentation 
			 */
			gates* clone() {
				auto repres = new gates(this->chromosome->clone());

				return repres;
			}

			/**
			 * @brief Calculate power consumation of circuic
			 * @return unsigned power consumation
			 */
			unsigned power_loss() {
				return this->chromosome->used_cost(gates::gate_power);
			}

			/**
			 * @brief Calculate delay of circuic
			 * @return unsigned delay
			 */
			unsigned delay() {
				return this->chromosome->delay(gates::gate_power);
			}

			/**
			 * @brief get number of tranzistors of gate tyte
			 * @param gene gene of gate reprezentation
			 * @return unsigned number of trazsitors
			 */
            static unsigned gate_power(genome::gene_t gene) {
				//by ranzistor count in CMOS
                if (gene.type == SAFE_TYPE_ID(AND_GATE_ID))    return 6;
				if (gene.type == SAFE_TYPE_ID(NOT_GATE_ID))    return 2;
				if (gene.type == SAFE_TYPE_ID(NAND_GATE_ID))   return 4;
				if (gene.type == SAFE_TYPE_ID(ANDNOT_GATE_ID)) return 8;
				if (gene.type == SAFE_TYPE_ID(OR_GATE_ID))     return 6;
				if (gene.type == SAFE_TYPE_ID(NOR_GATE_ID))    return 4;
				if (gene.type == SAFE_TYPE_ID(OR_NOT_GATE_ID)) return 8;
				if (gene.type == SAFE_TYPE_ID(XOR_GATE_ID))    return 12;
				if (gene.type == SAFE_TYPE_ID(XNOR_GATE_ID))   return 14;

				return 1;
            }

			/**
			 * @brief Add gate to chromosome
			 * @param type type of gate
			 * @param I1 input 1 of gate
			 * @param I2 input 2 of gate
			 * @return output id of gate
			 */
			genome::io_id_t add_gate(uint16_t type, genome::io_id_t I1, genome::io_id_t I2);
			
			/**
			 * @brief Update gate in chromosome
			 * @param id of gate (output id)
			 * @param type type of gate
			 * @param I1 input 1 of gate
			 * @param I2 input 2 of gate
			 * @return output id of gate
			 */
			genome::io_id_t update_gate(genome::io_id_t id, uint16_t type, genome::io_id_t I1, genome::io_id_t I2);

		private:
			// support functions for RTLIL2GENOME
			genome::io_id_t add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);

			// support functions for GENOME2RTLIL
			Yosys::RTLIL::Cell* rtlil_add_andnot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_xor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_xnor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_not(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_ornot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_and(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_nand(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_or(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
			Yosys::RTLIL::Cell* rtlil_add_nor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map);
	};
}