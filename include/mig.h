/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with mig representation header
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

	/**
	  * @brief Class for MIG representation of circuic
	  *       Y
	  *       |
	  *       O      0b1000
	  *       |
	  *     MAJ(3)
	  *     / | \
	  *    O  O  O   0b0001 0b0010 0b0100
	  *   /   |   \
	  *  A    B    C 
	  * Majority-Inverter Graph reprezentation where type reprezenting Existing or not Existing 
	  * negation on port. 0bYCBA is order in type. Example: 0b1101 -> Y = !MAJ3(!A, B, !C)
	  */
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
				return "\"gate\": [\"none\", \"maj(A,B,C)\", \"maj(!A,B,C)\", \"maj(A,!B,C)\", \"maj(!A,!B,C)\", \"maj(A,B,!C)\", \"maj(!A,B,!C)\", \"maj(A,!B,!C)\", \"maj(!A,!B,!C)\", \"!maj(A,B,C)\", \"!maj(!A,B,C)\", \"!maj(A,!B,C)\", \"!maj(!A,!B,C)\", \"!maj(A,B,!C)\", \"!maj(!A,B,!C)\", \"!maj(A,!B,!C)\", \"!maj(!A,!B,!C)\"]";
			}

			/**
			 * @brief Make deep copy of this object
			 * @return new aig reprezentation 
			 */
			mig* clone() {
				auto repres = new mig(this->chromosome->clone());

				return repres;
			}

			/**
			 * @brief Calculate power consumation of circuic
			 * @return unsigned power consumation
			 */
			unsigned power_loss() {
				return this->chromosome->used_cost(mig::gate_power);
			}

			/**
			 * @brief Calculate delay of circuic
			 * @return unsigned delay
			 */
			unsigned delay() {
				return this->chromosome->delay(mig::gate_power);
			}

			/**
			 * @brief get number of tranzistors of gate tyte
			 * @param gene gene of gate reprezentation
			 * @return unsigned number of trazsitors
			 */
            static unsigned gate_power(genome::gene_t gene) {
				#ifdef REAL_MIG_BACKEND
					return 1;
				#endif

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
			//support functions for add RTLIL cell to genome
            genome::io_id_t add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
            genome::io_id_t add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);

			//support functions for add gene to RTLIL
            Yosys::RTLIL::Cell* rtlil_add_maj3(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_or_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_and_vars(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_and(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
			Yosys::RTLIL::Cell* rtlil_add_or(genome::io_id_t a, genome::io_id_t b, unsigned type, genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace);
	};
}