/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with gate representation header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once
#include "representation.h"

#define NO_PORT_0_TYPE(X) (UNSAFE_TYPE_ID(X) >> 1)
#define NO_PORT_1_TYPE(X) ((UNSAFE_TYPE_ID(X) & 0b1)  | ((UNSAFE_TYPE_ID(X) & 0b1100) >> 1))
#define NO_PORT_2_TYPE(X) ((UNSAFE_TYPE_ID(X) & 0b11) | ((UNSAFE_TYPE_ID(X) & 0b1000) >> 1))

namespace representation {

	const unsigned mig_and_or_gate_costs[] = {
		6, //000
		8, //001
		8, //010
		4, //011
		4, //100
		8, //101
		8, //110
		6  //111
	};

	class mig : public representation {
		public:
			mig(genome::genome* chromosome) : representation(chromosome) {
				chromosome->gene_inputs_count = 3;
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
			
			void simulate(std::vector<simulation::io_t> &gates_o);

			unsigned mutate(unsigned center, unsigned sigma);

			/**
			 * @brief Get name of parts in genome
			 * @return string 
			 */
			std::string parts_naming() {
				return "\"gate\": [\"none\", \"maj(A,B,C)\", \"maj(!A,B,C)\", \"maj(A,!B,C)\", \"maj(!A,!B,C)\", \"maj(A,B,!C)\", \"maj(!A,B,!C)\", \"maj(A,!B,!C)\", \"maj(!A,!B,!C)\", \"!maj(A,B,C)\", \"!maj(!A,B,C)\", \"!maj(A,!B,C)\", \"!maj(!A,!B,C)\", \"!maj(A,B,!C)\", \"!maj(!A,B,!C)\", \"!maj(A,!B,!C)\", \"!maj(!A,!B,!C)\"]";
			}

			mig* clone() {
				auto repres = new mig(this->chromosome->clone());

				return repres;
			}

			unsigned power_loss() {
				return this->chromosome->used_cost(mig::gate_power);
			}

            static unsigned gate_power(genome::gene_t gene) {
				if (gene.Inputs[0] == 0) return mig_and_or_gate_costs[NO_PORT_0_TYPE(gene.type)];
				if (gene.Inputs[1] == 0) return mig_and_or_gate_costs[NO_PORT_1_TYPE(gene.type)];
				if (gene.Inputs[2] == 0) return mig_and_or_gate_costs[NO_PORT_2_TYPE(gene.type)];

				if (gene.Inputs[0] == 1) return mig_and_or_gate_costs[NO_PORT_0_TYPE(gene.type)];
				if (gene.Inputs[1] == 1) return mig_and_or_gate_costs[NO_PORT_1_TYPE(gene.type)];
				if (gene.Inputs[2] == 1) return mig_and_or_gate_costs[NO_PORT_2_TYPE(gene.type)];

				auto invertions = UNSAFE_TYPE_ID(gene.type);

                return 30 + 2 * ((invertions & 0b1) + ((invertions >> 1) & 0b1) + ((invertions >> 2) & 0b1) + ((invertions >> 3) & 0b1));
            }

			/**
			 * @brief Add gate to chromosome
			 * @param type type of gate
			 * @param I1 input 1 of gate
			 * @param I2 input 2 of gate
			 * @return output id of gate
			 */
			genome::io_id_t add_mig_gate(uint16_t type, genome::io_id_t I1, genome::io_id_t I2, genome::io_id_t I3);
			
			/**
			 * @brief Update gate in chromosome
			 * @param id of gate (output id)
			 * @param type type of gate
			 * @param I1 input 1 of gate
			 * @param I2 input 2 of gate
			 * @return output id of gate
			 */
			genome::io_id_t update_mig_gate(genome::io_id_t id, uint16_t type, genome::io_id_t I1, genome::io_id_t I2, genome::io_id_t I3);

		private:
            genome::io_id_t add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);

            Yosys::RTLIL::Cell* rtlil_add_maj3(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_or_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_and_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_and(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_or(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
	};
}