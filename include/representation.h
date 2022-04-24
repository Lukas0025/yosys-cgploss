/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * circuic representation interface class file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
#include "simulation.h"
#include <string>
#include <iostream>
#include <fstream>

#define SAFE_TYPE_ID(X) (X + 1)
#define UNSAFE_TYPE_ID(X) (X - 1)

namespace representation {
	/**
	 * @brief Interface for reprezentations of curcuic reprezenting genome
	 */
	class representation {
		public:
			representation(genome::genome* chromosome) {
				this->chromosome = chromosome;
			}

			virtual ~representation() {
				delete this->chromosome;
			}

			/**
			 * @brief Add cell from RTLIL to Genome
			 * @param type of RTLIL cell
			 * @param inputs vecor of mapped inputs
			 * @param output mapped output (id of output gene)
			 * @return genome::io_id_t 
			 */
			virtual genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) = 0;

			/**
			 * @brief Get RTLIL of one gene in Genome
			 * @param id id of gene in chromozome
			 * @param mod RTLIL module for write RTLIL reprezentation
			 * @param assign_map Used SigBits with theirs id in chomozome 
			 * @param wire_namespace string namespace for new RTLIL names
			 * @return Yosys::RTLIL::Cell* 
			 */
			virtual Yosys::RTLIL::Cell* get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) = 0;

			/**
			 * @brief Perform simulation on reprezentation
			 * @param gates_o input/output data structure. Must have .size() == chromozome.size()
			 * On position of input genes fill with testing input
			 * On outputs pusition  is outputs of circuic
			 */
			virtual void simulate(std::vector<simulation::io_t> &gates_o) = 0;

			/**
			 * @brief Calculate power consumation of circuic
			 * @return unsigned power consumation
			 */
			virtual unsigned power_loss() = 0;

			/**
			 * @brief Perform muatatin operation over chromozome
			 * @param center center of random number of mutations
			 * @param sigma sigma of normal distribution of number of mutations
			 * @param l_back maximal distance of mutation
			 * @return unsigned number of mutations
			 */
			virtual unsigned mutate(unsigned center, unsigned sigma, unsigned l_back) = 0;

			/**
			 * @brief Make deep copy of reprezentation
			 * @return new representation object 
			 */
			virtual representation* clone() = 0;
			
			/**
			 * @brief Get names of types in genome (For debug)
			 * @return std::string 
			 */
			virtual std::string parts_naming() {
				return "\"gate\": [\"none\"]";
			}

			/**
			 * @brief Set the rtlil port by assign map and id from chromozome
			 * @param gate RTLIL gate for set port
			 * @param port Name of port to set
			 * @param genome_port id of port in genome (id of input gene in chromozome)
			 * @param assign_map assing map with signals
			 */
			void set_rtlil_port(Yosys::RTLIL::Cell* gate, Yosys::RTLIL::IdString port, genome::io_id_t genome_port, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
				gate->setPort(port, this->get_rtlil_port(genome_port,assign_map));
			}

			/**
			 * @brief Get the rtlil sigbit of genome port (ID of gene input/ output)
			 * @param genome_port ID of output / input gene in Genome
			 * @param assign_map assign map with mapped signals
			 * @return Yosys::RTLIL::SigBit 
			 */
			Yosys::RTLIL::SigBit get_rtlil_port(genome::io_id_t genome_port, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
				if (genome_port == 0) {
					return Yosys::RTLIL::State::S0;
				} else if (genome_port == 1) {
					return Yosys::RTLIL::State::S1;
				} else {
					return assign_map[genome_port];
				}
			}

			/**
			 * @brief Save whole reprezentation with chromozome to file
			 * @param sfile stream of save file
			 */
			void save(std::ofstream& sfile) {
				std::string file_str = "{\n\t\"parts\": [\"gate\", \"input\", \"input\"],\n\t\"partsNaming\": {\n\t\t";
				file_str += this->parts_naming() + "\n\t},";
				file_str += "\n\t\"inputs\": [";

				for (genome::io_id_t i = 0; i <= this->chromosome->last_input; i++) {
					file_str += std::to_string(i) + ",";
				}
				file_str.pop_back();

				file_str += "],";
				file_str += "\n\t\"outputs\": [";

				for (auto output = this->chromosome->wire_out.begin(); output != this->chromosome->wire_out.end(); output++) {
					file_str += std::to_string(output->first) + ",";
				}
				file_str.pop_back();

				file_str += "],\n";
				file_str += this->chromosome->to_string();
				file_str += "\n}";

				sfile << file_str;
			}
			
			genome::genome* chromosome;
	};
}