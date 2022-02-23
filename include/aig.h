/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with aig representation header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include "representation.h"

namespace representation {
	/**
	  * @brief Class for AIG representation of circuic
	  *      Y
	  *      |
	  *      O     0b100
	  *      |
	  *     AND
	  *     / \
	  *    O   O   0b001 0b010
	  *   /     \
	  *  A       B 
	  * And-inverter graph reprezentation where type reprezenting Existing or not Existing 
	  * negation on port. 0bYBA is order in type. Example: 0b101 -> Y = !(B && !A)
	  */
	class aig : public representation {
		public:
			aig(genome::genome* chromosome) : representation(chromosome) {};
			
			/**
			 * @brief Add Yosys::RTLIL cell as AIG to chromosome
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
			Yosys::RTLIL::Cell *get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map, std::string wire_namespace);
			
			/**
			 * @brief Get name of parts in genome
			 * @return string 
			 */
			std::string parts_naming() {
				return "\"gate\": [\"none\", \"A && B\", \"!A && B\", \"A && !B\", \"!A && !B\", \"!(A && B)\", \"!(!A && B)\", \"!(A && !B)\", \"!(!A && !B)\"]";
			}

		private:
			/**
			 * @brief Add AIG mux to genome
			 * @param inputs inputs of mux gate
			 * @param outputs outputs of mux gate
			 * @return output id of mux
			 */
			genome::io_id_t add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);

			/**
			 * @brief Add AIG nmux to genome
			 * @param inputs inputs of nmux gate
			 * @param outputs outputs of nmux gate
			 * @return output id of nmux
			 */
			genome::io_id_t add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG aoi3 to genome
			 * @param inputs inputs of aoi3 gate
			 * @param outputs outputs of aoi3 gate
			 * @return output id of aoi3
			 */
			genome::io_id_t add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG oai3 to genome
			 * @param inputs inputs of oai3 gate
			 * @param outputs outputs of oai3 gate
			 * @return output id of oai3
			 */
			genome::io_id_t add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG aoi4 to genome
			 * @param inputs inputs of aoi4 gate
			 * @param outputs outputs of aoi4 gate
			 * @return output id of aoi4
			 */
			genome::io_id_t add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG oai4 to genome
			 * @param inputs inputs of oai4 gate
			 * @param outputs outputs of oai4 gate
			 * @return output id of oai4
			 */
			genome::io_id_t add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG xor to genome
			 * @param inputs inputs of xor gate
			 * @param outputs outputs of xor gate
			 * @return output id of xor
			 */
			genome::io_id_t add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG xnor to genome
			 * @param inputs inputs of xnor gate
			 * @param outputs outputs of xnor gate
			 * @return output id of xnor
			 */
			genome::io_id_t add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Add AIG gate to chromosome
			 * @param type type of gate
			 * @param I1 input 1 of gate 0b00A
			 * @param I2 input 2 of gate 0b0A0
			 * @return output id of gate 0bA00
			 */
			genome::io_id_t add_aig_gate(uint16_t type, genome::io_id_t I1, genome::io_id_t I2);
			
			/**
			 * @brief Update AIG gate in chromosome
			 * @param id of gate (output id) 0bA00
			 * @param type type of gate
			 * @param I1 input 1 of gate 0b00A
			 * @param I2 input 2 of gate 0b0A0
			 * @return output id of gate 0bA00
			 */
			genome::io_id_t update_aig_gate(genome::io_id_t id, uint16_t type, genome::io_id_t I1, genome::io_id_t I2);

			/**
			 * @brief Add and gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_and(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
			
			/**
			 * @brief Add nand gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_nand(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
			
			/**
			 * @brief Add or gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_or(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
			
			/**
			 * @brief Add nor gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_nor(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
			
			/**
			 * @brief Add !a and b gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @param wire_namespace string namespace for new wires
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_andnota(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map, std::string wire_namespace);
			
			/**
			 * @brief Add !a or b gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @param wire_namespace string namespace for new wires
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_ornota(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map, std::string wire_namespace);
			
			/**
			 * @brief Add a and !b gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_andnot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
			
			/**
			 * @brief Add a or !b gate from chromosome to module
			 * @param id of gate in chromosome (gene)
			 * @param mod RTLIL module
			 * @param assign_map map mapping genes id to wire pointer
			 * @return poiter to created RTLIL::cell
			 */
			Yosys::RTLIL::Cell* rtlil_add_ornot(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<int, Yosys::RTLIL::Wire*> &assign_map);
	};
}